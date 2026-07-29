#include "filter.h"

#include "handle.h"
#include "log\format.h"
#include "log\sink.h"
#include "records.h"
#include "text.h"

#include <Windows.h>

#include <algorithm>
#include <array>
#include <atomic>
#include <cstdlib>
#include <cwctype>
#include <filesystem>
#include <format>
#include <iostream>
#include <memory>
#include <optional>
#include <span>
#include <string>
#include <system_error>

using namespace mimo;

namespace {

    struct Parameters final {
        std::optional<std::wstring> attach;
        std::optional<std::wstring> attachAll;
        std::optional<std::wstring> file;
        bool unload = false;
        bool split = false;
    };

    struct Flag final {
        std::wstring_view name;
        bool Parameters::* member;
    };

    struct Option final {
        std::wstring_view name;
        std::optional<std::wstring> Parameters::* member;
    };

    constexpr Flag FLAG_TABLE[] = {
        { L"/u", &Parameters::unload },
        { L"/s", &Parameters::split },
    };

    constexpr Option OPTION_TABLE[] = {
        { L"/a", &Parameters::attach },
        { L"/m", &Parameters::attachAll },
        { L"/f", &Parameters::file },
    };

    std::atomic<bool> stop{ false };

    void ConfigureOutputEncoding() {
        SetConsoleOutputCP(CP_UTF8);

        return;
    }


    std::optional<Parameters> ParseParameters(int argc, wchar_t* argv[]) {
        Parameters params{};

        for (int i = 0; i < argc; ++i) {
            const std::wstring_view arg = argv[i];
            bool matched = false;

            for (const Flag& f : FLAG_TABLE) {

                if (std::ranges::equal(arg, f.name, [](wchar_t x, wchar_t y) { return std::towlower(x) == std::towlower(y); })) {

                    if (params.*(f.member)) return std::nullopt;

                    params.*(f.member) = true;
                    matched = true;

                    break;
                }

            }

            if (matched) continue;

            for (const Option& o : OPTION_TABLE) {

                if (std::ranges::equal(arg, o.name, [](wchar_t x, wchar_t y) { return std::towlower(x) == std::towlower(y); })) {

                    if ((params.*(o.member)).has_value()) return std::nullopt;

                    if (i + 1 >= argc) return std::nullopt;

                    params.*(o.member) = argv[++i];
                    matched = true;

                    break;
                }

            }

            if (matched) continue;

            return std::nullopt;
        }

        return params;
    }


    bool ValidateParameters(const Parameters& params) {
        int actions = 0;

        if (params.unload) ++actions;

        if (params.attach.has_value()) ++actions;

        if (params.attachAll.has_value()) ++actions;

        const bool capture = params.file.has_value() || params.split;

        if (actions > 1) return false;

        if (actions > 0 && capture) return false;

        if (params.split && !params.file.has_value()) return false;

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


    void DisplayError(HRESULT code) {
        std::cerr << std::format("Error: 0x{:08X}, ", static_cast<ULONG>(code));

        std::array<wchar_t, 1024u> buffer{};
        const DWORD count = FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, code, 0u, buffer.data(), static_cast<DWORD>(buffer.size()), nullptr);

        if (count) {
            std::cerr << text::ConvertToUtf8(buffer.data());
        }
        else {
            std::cerr << "failed to translate error code";
        }

        std::cerr << "\n";

        return;
    }


    int ReportResult(HRESULT hRes, std::string_view errorMsg) {

        if (SUCCEEDED(hRes)) return EXIT_SUCCESS;

        std::cerr << errorMsg << "\n";
        DisplayError(hRes);

        return EXIT_FAILURE;
    }


    void DisplayAdminRightsError() {
        std::cerr << "This operation requires administrator rights - run from an elevated command prompt.\n";

        return;
    }


    void DisplayFilterUnloadedError() {
        std::cerr << "The driver does not appear to be loaded. Load and attach it first (e.g. MiniMonClient /a <volume>).\n";

        return;
    }


    bool DoesParentDirectoryExist(const std::wstring& filePath) {
        const std::filesystem::path parent = std::filesystem::path{ filePath }.parent_path();

        if (parent.empty()) return true;

        std::error_code ec;

        return std::filesystem::is_directory(parent, ec);
    }


    std::unique_ptr<log::Sink> MakeSink(const Parameters& params) {

        if (!params.file.has_value()) return std::make_unique<log::ConsoleSink>(std::cout, log::format::GetHeader());

        if (!DoesParentDirectoryExist(*params.file)) return nullptr;

        return std::make_unique<log::FileSink>(*params.file, params.split, log::format::GetHeader());
    }


    BOOL WINAPI CtrlHandler(DWORD ctrlType) {

        switch (ctrlType) {
            case CTRL_C_EVENT:
            case CTRL_BREAK_EVENT:
            case CTRL_CLOSE_EVENT:
            case CTRL_LOGOFF_EVENT:
            case CTRL_SHUTDOWN_EVENT:
                stop.store(true, std::memory_order_relaxed);

                return TRUE;
        }

        return FALSE;
    }


    bool CaptureLoop(const InvHandle& port, log::Sink& sink) {
        constexpr uint32_t BUFFER_SIZE = 1000 * sizeof(protocol::Record);
        constexpr DWORD POLL_INTERVAL_MS = 200u;

        AlignedBuffer buffer{ BUFFER_SIZE };
        uint32_t droppedReported = 0u;

        while (!stop.load(std::memory_order_relaxed)) {
            buffer.Resize(BUFFER_SIZE);
            const HRESULT hRes = filter::GetRecords(port, buffer);

            if (FAILED(hRes)) {
                std::cerr << "Failed to get log records from filter\n";
                DisplayError(hRes);

                return false;
            }

            if (buffer.Size() > 0u) {
                const std::optional<std::span<const protocol::Record>> records = records::Parse(buffer);

                if (!records) {
                    std::cerr << "Received a malformed record buffer from the filter\n";

                    return false;
                }

                for (const protocol::Record& record : *records) {

                    if (!sink.Write(record.data.altitude, log::format::Render(record))) {
                        std::cerr << "Failed to write log records\n";

                        return false;
                    }

                }

                sink.Flush();

                const uint32_t droppedTotal = records->back().droppedRecords;

                if (droppedTotal > droppedReported) {
                    std::cerr << "Warning: the driver dropped " << (droppedTotal - droppedReported) << " record(s) (out of memory or memory cap reached)\n";
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

    const std::optional<Parameters> params = ParseParameters(argc - 1, &argv[1]);

    if (!params.has_value() || !ValidateParameters(*params)) {
        PrintUsage();

        return EXIT_FAILURE;
    }

    if (params->attach.has_value()) {

        const HRESULT hRes = filter::Attach(*params->attach);
        const int result = ReportResult(hRes, "Failed to attach to volume");

        // the only privilege the client adjusts is SE_LOAD_DRIVER_NAME, so this code always means "not elevated"
        if (hRes == HRESULT_FROM_WIN32(ERROR_NOT_ALL_ASSIGNED)) {
            DisplayAdminRightsError();
        }

        return result;
    }

    if (params->attachAll.has_value()) {

        const HRESULT hRes = filter::AttachAll(*params->attachAll);
        const int result = ReportResult(hRes, "Failed to attach all installed instances to volume");

        if (hRes == HRESULT_FROM_WIN32(ERROR_NOT_ALL_ASSIGNED)) {
            DisplayAdminRightsError();
        }

        return result;
    }

    if (params->unload) {

        const HRESULT hRes = filter::Unload();
        const int result = ReportResult(hRes, "Failed to unload driver");

        if (hRes == HRESULT_FROM_WIN32(ERROR_NOT_ALL_ASSIGNED)) {
            DisplayAdminRightsError();
        }

        return result;
    }

    std::unique_ptr<log::Sink> sink = MakeSink(*params);

    if (!sink) {

        return ReportResult(HRESULT_FROM_WIN32(ERROR_PATH_NOT_FOUND), "Failed to open output file");
    }

    InvHandle port;
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
