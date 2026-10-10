#include "filter.h"

#include "trace\format.h"

#include "handle.h"
#include "records.h"
#include "sink.h"
#include "text.h"

#include "..\..\inc\protocol.h"

#include <Windows.h>

#include <algorithm>
#include <array>
#include <atomic>
#include <cstdint>
#include <cstdlib>
#include <cwctype>
#include <filesystem>
#include <format>
#include <iostream>
#include <memory>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <system_error>

using namespace mimo;

namespace {

    void ConfigureOutputEncoding() {
        SetConsoleOutputCP(CP_UTF8);

        return;
    }


    bool IsSameName(std::wstring_view arg, std::wstring_view name) {

        return std::ranges::equal(arg, name, [](wchar_t left, wchar_t right) { return std::towlower(left) == std::towlower(right); });
    }


    struct Parameters {
        std::optional<std::wstring> attach;
        std::optional<std::wstring> attachAll;
        std::optional<std::wstring> file;
        bool unload = false;
        bool split = false;
    };

    struct Flag {
        std::wstring_view name;
        bool Parameters::* member;
    };

    struct Option {
        std::wstring_view name;
        std::optional<std::wstring> Parameters::* member;
    };

    constexpr Flag FLAG_TABLE[]{
        { L"/u", &Parameters::unload },
        { L"/s", &Parameters::split },
    };

    constexpr Option OPTION_TABLE[]{
        { L"/a", &Parameters::attach },
        { L"/m", &Parameters::attachAll },
        { L"/f", &Parameters::file },
    };

    std::optional<Parameters> ParseParameters(int argc, wchar_t* argv[]) {
        Parameters parameters{};

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

            return std::nullopt;
        }

        return parameters;
    }


    bool ValidateParameters(const Parameters& parameters) {
        int actions = 0;

        if (parameters.unload) actions++;

        if (parameters.attach.has_value()) actions++;

        if (parameters.attachAll.has_value()) actions++;

        const bool capture = parameters.file.has_value() || parameters.split;

        if (actions > 1) return false;

        if (actions > 0 && capture) return false;

        if (parameters.split && !parameters.file.has_value()) return false;

        return true;
    }


    void PrintUsage() {
        std::cerr << "Usage: MiniMonClient [/a <volume> | /m <volume> | /u | /f <output_file> [/s]]\n";
        std::cerr << "    /a <volume>      loads the driver and attaches the default filter instance to <volume>\n";
        std::cerr << "    /m <volume>      loads the driver and attaches every installed filter instance to <volume>\n";
        std::cerr << "    /u               unloads the driver\n";
        std::cerr << "    /f <output_file> writes captured log records to <output_file> (default: stdout)\n";
        std::cerr << "    /s               with /f, writes each altitude to its own file (<base>.<altitude>.<ext>)\n";

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


    bool DoesParentDirectoryExist(const std::wstring& filePath) {
        const std::filesystem::path parent = std::filesystem::path{ filePath }.parent_path();

        if (parent.empty()) return true;

        std::error_code errorCode;

        return std::filesystem::is_directory(parent, errorCode);
    }


    std::unique_ptr<Sink> MakeSink(const Parameters& parameters) {

        if (!parameters.file.has_value()) return std::make_unique<ConsoleSink>(std::cout, trace::format::GetHeader());

        if (!DoesParentDirectoryExist(*parameters.file)) return nullptr;

        return std::make_unique<FileSink>(*parameters.file, parameters.split, trace::format::GetHeader());
    }


    void DisplayFilterUnloadedError() {
        std::cerr << "The driver does not appear to be loaded. Load and attach it first (e.g. MiniMonClient /a <volume>).\n";

        return;
    }


    std::atomic<bool> stopRequested{ false };

    BOOL WINAPI CtrlHandler(DWORD ctrlType) {

        switch (ctrlType) {

            case CTRL_C_EVENT:
            case CTRL_BREAK_EVENT:
            case CTRL_CLOSE_EVENT:
            case CTRL_LOGOFF_EVENT:
            case CTRL_SHUTDOWN_EVENT:
                stopRequested.store(true, std::memory_order_relaxed);

                return TRUE;
        }

        return FALSE;
    }


    bool CaptureLoop(const InvalidHandle& port, Sink& sink) {
        constexpr uint32_t BUFFER_SIZE = 1000u * sizeof(protocol::Record);
        constexpr DWORD POLL_INTERVAL_MS = 200u;

        AlignedBuffer buffer{ BUFFER_SIZE };
        uint32_t droppedReported = 0u;

        while (!stopRequested.load(std::memory_order_relaxed)) {
            buffer.Resize(BUFFER_SIZE);
            const HRESULT hRes = filter::GetRecords(port, buffer);

            if (FAILED(hRes)) {
                std::cerr << "Failed to get log records from filter\n";
                DisplayError(hRes);

                return false;
            }

            if (buffer.Size()) {
                const std::optional<std::span<const protocol::Record>> records = records::Parse(buffer);

                if (!records.has_value()) {
                    std::cerr << "Received a malformed record buffer from the filter\n";

                    return false;
                }

                for (const protocol::Record& record : *records) {

                    if (!sink.Write(record.data.altitude, trace::format::Render(record))) {
                        std::cerr << "Failed to write log records\n";

                        return false;
                    }

                }

                sink.Flush();

                const uint32_t droppedTotal = records->back().droppedRecords;

                if (droppedTotal > droppedReported) {
                    std::cerr << std::format("Warning: the driver dropped {} record(s) (out of memory or memory cap reached)\n", droppedTotal - droppedReported);
                    droppedReported = droppedTotal;
                }

            }

            if (buffer.Size() < BUFFER_SIZE) {
                Sleep(POLL_INTERVAL_MS);
            }

        }

        return true;
    }

}

int wmain(int argc, wchar_t* argv[]) {
    ConfigureOutputEncoding();

    const std::optional<Parameters> parameters = ParseParameters(argc - 1, &argv[1u]);

    if (!parameters.has_value() || !ValidateParameters(*parameters)) {
        PrintUsage();

        return EXIT_FAILURE;
    }

    if (parameters->attach.has_value()) return ReportActionResult(filter::Attach(*parameters->attach), "Failed to attach to volume");

    if (parameters->attachAll.has_value()) return ReportActionResult(filter::AttachAll(*parameters->attachAll), "Failed to attach all installed instances to volume");

    if (parameters->unload) return ReportActionResult(filter::Unload(), "Failed to unload driver");

    const std::unique_ptr<Sink> sink = MakeSink(*parameters);

    if (!sink) {

        return ReportResult(HRESULT_FROM_WIN32(ERROR_PATH_NOT_FOUND), "Failed to open output file");
    }

    InvalidHandle port;
    const HRESULT hRes = filter::Connect(port);

    if (FAILED(hRes)) {
        const int result = ReportResult(hRes, "Failed to connect to filter port");

        // the comms port only exists while the driver is loaded, so a missing port means it is not running
        if (hRes == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND)) {
            DisplayFilterUnloadedError();
        }

        return result;
    }

    if (!SetConsoleCtrlHandler(CtrlHandler, TRUE)) {

        return ReportResult(HRESULT_FROM_WIN32(GetLastError()), "Failed to install console control handler");
    }

    return CaptureLoop(port, *sink) ? EXIT_SUCCESS : EXIT_FAILURE;
}
