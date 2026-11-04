#include "filter.h"

#include "trace\format.h"

#include "handle.h"
#include "queue.h"
#include "sink.h"
#include "text.h"

#include "..\..\inc\protocol.h"

#include <Windows.h>

#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cwctype>
#include <filesystem>
#include <format>
#include <future>
#include <ios>
#include <iostream>
#include <optional>
#include <ostream>
#include <span>
#include <string>
#include <string_view>
#include <system_error>
#include <utility>
#include <vector>

using namespace mimo;

namespace {

    void ConfigureOutput() {
        SetConsoleOutputCP(CP_UTF8);
        std::cout.setf(std::ios::unitbuf);

        return;
    }


    bool IsSameName(std::wstring_view arg, std::wstring_view name) {

        return std::ranges::equal(arg, name, [](wchar_t left, wchar_t right) { return std::towlower(left) == std::towlower(right); });
    }


    bool IsSwitch(std::wstring_view arg) {

        return arg.starts_with(L'/');
    }


    struct Parameters {
        std::optional<std::wstring> file;
        std::vector<std::wstring> volumes;
        bool split = false;
        bool help = false;
    };

    struct Flag {
        std::wstring_view name;
        bool Parameters::* member;
    };

    struct Option {
        std::wstring_view name;
        std::optional<std::wstring> Parameters::* member;
    };

    struct List {
        std::wstring_view name;
        std::vector<std::wstring> Parameters::* member;
    };

    constexpr Flag FLAG_TABLE[]{
        { L"/s", &Parameters::split },
        { L"/?", &Parameters::help },
    };

    constexpr Option OPTION_TABLE[]{
        { L"/f", &Parameters::file },
    };

    constexpr List LIST_TABLE[]{
        { L"/v", &Parameters::volumes },
    };

    std::optional<Parameters> ParseParameters(int argc, wchar_t* argv[]) {
        Parameters parameters;

        for (int i = 0; i < argc; i++) {
            const std::wstring_view arg = argv[i];
            bool matched = false;

            for (const Flag& flag : FLAG_TABLE) {

                if (IsSameName(arg, flag.name)) {

                    if (parameters.*(flag.member)) return std::nullopt;

                    parameters.*(flag.member) = true;
                    matched = true;

                    break;
                }

            }

            if (matched) continue;

            for (const Option& option : OPTION_TABLE) {

                if (IsSameName(arg, option.name)) {

                    if ((parameters.*(option.member)).has_value()) return std::nullopt;

                    if (i + 1 >= argc) return std::nullopt;

                    i++;
                    parameters.*(option.member) = argv[i];
                    matched = true;

                    break;
                }

            }

            if (matched) continue;

            for (const List& list : LIST_TABLE) {

                if (IsSameName(arg, list.name)) {

                    if (!(parameters.*(list.member)).empty()) return std::nullopt;

                    while (i + 1 < argc && !IsSwitch(argv[i + 1])) {
                        i++;
                        (parameters.*(list.member)).emplace_back(argv[i]);
                    }

                    if ((parameters.*(list.member)).empty()) return std::nullopt;

                    matched = true;

                    break;
                }

            }

            if (matched) continue;

            return std::nullopt;
        }

        return parameters;
    }


    bool ValidateParameters(const Parameters& parameters) {

        if (parameters.help) return !parameters.file.has_value() && parameters.volumes.empty() && !parameters.split;

        return parameters.file.has_value();
    }


    void PrintUsage(std::ostream& stream) {
        stream << "Usage: MiniMonClient /f <output_file> [/v <volume>...] [/s]\n";
        stream << "       MiniMonClient /?\n";
        stream << "    /f <output_file> loads the driver, attaches every installed instance to every volume with a file system, writes the captured records to <output_file> until Ctrl+C and unloads the driver\n";
        stream << "    /v <volume>...   attaches to the named volumes only\n";
        stream << "    /s               writes each altitude to its own file (<base>.<altitude>.<ext>)\n";
        stream << "    /?               prints this usage\n";

        return;
    }


    void DisplayError(HRESULT hRes) {
        constexpr uint32_t MESSAGE_WCHAR_COUNT = 1024u;

        std::cerr << std::format("Error: 0x{:08X}, ", static_cast<uint32_t>(hRes));

        std::array<wchar_t, MESSAGE_WCHAR_COUNT> buffer{};
        const DWORD count = FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, static_cast<DWORD>(hRes), 0u, buffer.data(), static_cast<DWORD>(buffer.size()), nullptr);

        if (count) {
            std::cerr << text::ConvertToUtf8(buffer.data());
        }
        else {
            std::cerr << "failed to translate error code";
        }

        std::cerr << "\n";

        return;
    }


    int ReportResult(HRESULT hRes, std::string_view errorMessage) {

        if (SUCCEEDED(hRes)) return EXIT_SUCCESS;

        std::cerr << errorMessage << "\n";
        DisplayError(hRes);

        return EXIT_FAILURE;
    }


    void DisplayAdminRightsError() {
        std::cerr << "This operation requires administrator rights - run from an elevated command prompt.\n";

        return;
    }


    int ReportActionResult(HRESULT hRes, std::string_view errorMessage) {
        const int result = ReportResult(hRes, errorMessage);

        if (hRes == HRESULT_FROM_WIN32(ERROR_NOT_ALL_ASSIGNED)) {
            DisplayAdminRightsError();
        }

        return result;
    }


    bool DoesParentDirectoryExist(std::wstring_view filePath) {
        const std::filesystem::path parent = std::filesystem::path{ filePath }.parent_path();

        if (parent.empty()) return true;

        std::error_code errorCode;

        return std::filesystem::is_directory(parent, errorCode);
    }


    std::wstring RenderVersion(const protocol::Version& version) {

        return std::format(L"{}.{}.{}", version.major, version.minor, version.patch);
    }


    constexpr bool IsSameVersion(const protocol::Version& left, const protocol::Version& right) {

        return left.major == right.major && left.minor == right.minor && left.patch == right.patch;
    }


    bool VerifyVersion(const InvalidHandle& port) {
        protocol::Version driverVersion{};
        const HRESULT hRes = filter::GetVersion(port, driverVersion);

        if (FAILED(hRes)) {
            std::cerr << "Failed to query the driver version\n";
            DisplayError(hRes);

            return false;
        }

        const std::string clientText = text::ConvertToUtf8(RenderVersion(protocol::MINIMON_VERSION));
        const std::string driverText = text::ConvertToUtf8(RenderVersion(driverVersion));

        if (!IsSameVersion(driverVersion, protocol::MINIMON_VERSION)) {
            std::cerr << std::format("Version mismatch: MiniMonClient {}, MiniMonFlt {}\n", clientText, driverText);

            return false;
        }

        std::cout << std::format("MiniMonClient {}, MiniMonFlt {}\n", clientText, driverText);

        return true;
    }


    std::atomic<bool> stopRequested;

    BOOL WINAPI CtrlHandler(DWORD ctrlType) {
        constexpr DWORD CLOSE_WAIT_MS = 4000u;

        switch (ctrlType) {

            case CTRL_C_EVENT:
            case CTRL_BREAK_EVENT:
                stopRequested.store(true, std::memory_order_relaxed);

                return TRUE;

            case CTRL_CLOSE_EVENT:
            case CTRL_LOGOFF_EVENT:
            case CTRL_SHUTDOWN_EVENT:
                stopRequested.store(true, std::memory_order_relaxed);
                // the system terminates the process on return or when the handler's budget elapses (5 s for close), so the wait has to happen here
                // the main thread exits the process once the backlog is written, which cuts the sleep short
                Sleep(CLOSE_WAIT_MS);
                filter::Unload();

                return TRUE;
        }

        return FALSE;
    }


    bool WriteRecords(Sink& sink, std::span<const protocol::Record> records) {

        for (const protocol::Record& record : records) {

            if (!sink.Write(text::Extract(record.data.altitude), trace::format::Render(record))) return false;

        }

        sink.Flush();

        return true;
    }


    using RecordBatchQueue = BlockingQueue<std::vector<protocol::Record>>;

    std::atomic<size_t> pendingRecordCount;

    bool WriteLoop(RecordBatchQueue& queue, Sink& sink) {

        while (true) {
            const std::optional<std::vector<protocol::Record>> records = queue.Pop();

            if (!records.has_value()) return true;

            if (!WriteRecords(sink, *records)) {
                stopRequested.store(true, std::memory_order_relaxed);
                queue.Close();

                return false;
            }

            pendingRecordCount.fetch_sub(records->size(), std::memory_order_relaxed);
        }

    }


    bool DrainLoop(const InvalidHandle& port, RecordBatchQueue& queue) {
        constexpr size_t RECORD_COUNT = 1000u;
        constexpr DWORD POLL_INTERVAL_MS = 200u;
        std::vector<protocol::Record> records(RECORD_COUNT);
        uint32_t droppedReported = 0u;

        while (!stopRequested.load(std::memory_order_relaxed)) {
            records.resize(RECORD_COUNT);
            const HRESULT hRes = filter::GetRecords(port, records);

            if (FAILED(hRes)) {
                std::cerr << "Failed to get log records from filter\n";
                DisplayError(hRes);

                return false;
            }

            const bool full = records.size() == RECORD_COUNT;

            if (!records.empty()) {
                const uint32_t droppedTotal = records.back().droppedRecords;
                pendingRecordCount.fetch_add(records.size(), std::memory_order_relaxed);

                if (!queue.Push(std::move(records))) break;

                if (droppedTotal > droppedReported) {
                    std::cerr << std::format("Warning: the driver dropped {} record(s) (out of memory or memory cap reached)\n", droppedTotal - droppedReported);
                    droppedReported = droppedTotal;
                }

            }

            if (!full) {
                Sleep(POLL_INTERVAL_MS);
            }

        }

        return true;
    }


    bool CaptureRecords(const InvalidHandle& port, Sink& sink) {
        constexpr size_t BATCH_COUNT = 256u;
        constexpr std::chrono::milliseconds REPORT_INTERVAL{ 250 };
        RecordBatchQueue queue{ BATCH_COUNT };
        std::future<bool> writer = std::async(std::launch::async, [&queue, &sink]() noexcept { return WriteLoop(queue, sink); });
        const bool drained = DrainLoop(port, queue);
        queue.Close();

        while (writer.wait_for(REPORT_INTERVAL) == std::future_status::timeout) {
            std::cout << std::format("Records left to write: {}\n", pendingRecordCount.load(std::memory_order_relaxed));
        }

        const bool written = writer.get();

        if (!written) {
            std::cerr << "Failed to write log records\n";
        }

        return drained && written;
    }


    int RunSession(std::span<const std::wstring> volumeNames, Sink& sink) {
        InvalidHandle port;
        const HRESULT hConnectRes = filter::Connect(port);

        if (FAILED(hConnectRes)) return ReportResult(hConnectRes, "Failed to connect to filter port");

        if (!VerifyVersion(port)) return EXIT_FAILURE;

        const HRESULT hAttachRes = volumeNames.empty() ? filter::AttachAll() : filter::Attach(volumeNames);

        if (FAILED(hAttachRes)) return ReportResult(hAttachRes, "Failed to attach to volume");

        if (!SetConsoleCtrlHandler(CtrlHandler, TRUE)) return ReportResult(HRESULT_FROM_WIN32(GetLastError()), "Failed to install console control handler");

        return CaptureRecords(port, sink) ? EXIT_SUCCESS : EXIT_FAILURE;
    }

}

int wmain(int argc, wchar_t* argv[]) {
    ConfigureOutput();

    const std::optional<Parameters> parameters = ParseParameters(argc - 1, &argv[1u]);

    if (!parameters.has_value() || !ValidateParameters(*parameters)) {
        PrintUsage(std::cerr);

        return EXIT_FAILURE;
    }

    if (parameters->help) {
        PrintUsage(std::cout);

        return EXIT_SUCCESS;
    }

    if (!DoesParentDirectoryExist(*parameters->file)) return ReportResult(HRESULT_FROM_WIN32(ERROR_PATH_NOT_FOUND), "Failed to open output file");

    Sink sink{ *parameters->file, parameters->split, trace::format::GetHeader() };

    const HRESULT hLoadRes = filter::Load();

    if (FAILED(hLoadRes)) return ReportActionResult(hLoadRes, "Failed to load driver");

    const int sessionResult = RunSession(parameters->volumes, sink);
    const int unloadResult = ReportResult(filter::Unload(), "Failed to unload driver");

    return sessionResult ? sessionResult : unloadResult;
}
