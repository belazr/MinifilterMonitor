#include "operations\create.h"

#include "test.h"
#include "text.h"

#include <Windows.h>

#include <algorithm>
#include <array>
#include <cstdint>
#include <cstdlib>
#include <cwctype>
#include <filesystem>
#include <format>
#include <ios>
#include <iostream>
#include <optional>
#include <ostream>
#include <string>
#include <string_view>
#include <system_error>

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


    struct Parameters {
        std::optional<std::wstring> directory;
        bool help = false;
    };

    std::optional<Parameters> ParseParameters(int argc, wchar_t* argv[]) {
        Parameters parameters;

        for (int i = 0; i < argc; i++) {
            const std::wstring_view arg = argv[i];

            if (IsSameName(arg, L"/?")) {

                if (parameters.help) return std::nullopt;

                parameters.help = true;

                continue;
            }

            if (IsSameName(arg, L"/d")) {

                if (parameters.directory.has_value()) return std::nullopt;

                if (i + 1 >= argc) return std::nullopt;

                i++;
                parameters.directory = argv[i];

                continue;
            }

            return std::nullopt;
        }

        return parameters;
    }


    bool ValidateParameters(const Parameters& parameters) {

        if (parameters.help) return !parameters.directory.has_value();

        return parameters.directory.has_value() && !parameters.directory->empty();
    }


    void PrintUsage(std::ostream& stream) {
        stream << "Usage: MiniMonTest /d <directory>\n";
        stream << "       MiniMonTest /?\n";
        stream << "    /d <directory> runs the file operations in a work directory inside <directory>\n";
        stream << "    /?             prints this usage\n";

        return;
    }


    void DisplayError(HRESULT hRes) {
        constexpr uint32_t MESSAGE_WCHAR_COUNT = 1024u;
        std::array<wchar_t, MESSAGE_WCHAR_COUNT> buffer{};
        const DWORD wcharCount = FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, static_cast<DWORD>(hRes), 0u, buffer.data(), static_cast<DWORD>(buffer.size()), nullptr);
        const std::string message = wcharCount ? text::ConvertToUtf8(buffer.data()) : "failed to translate error code";
        std::cerr << std::format("Error: 0x{:08X}, {}\n", static_cast<uint32_t>(hRes), message);

        return;
    }


    bool VerifyDirectory(const std::wstring& path) {
        const DWORD attributes = GetFileAttributesW(path.c_str());

        if (attributes == INVALID_FILE_ATTRIBUTES) {
            std::cerr << "Failed to query directory attributes\n";
            DisplayError(HRESULT_FROM_WIN32(GetLastError()));

            return false;
        }

        if (!(attributes & FILE_ATTRIBUTE_DIRECTORY)) {
            std::cerr << "Not a directory\n";

            return false;
        }

        return true;
    }


    bool RemoveWorkDirectory(std::wstring_view path) {
        std::error_code error;
        std::filesystem::remove_all(path, error);

        if (error.value() != ERROR_SUCCESS) {
            std::cerr << "Failed to remove the work directory\n";
            DisplayError(HRESULT_FROM_WIN32(static_cast<DWORD>(error.value())));

            return false;
        }

        return true;
    }


    std::optional<std::wstring> CreateWorkDirectory(std::wstring_view directory) {
        constexpr std::wstring_view NAME = L"MiniMonTest.tmp";
        const std::filesystem::path path = std::filesystem::path(directory) / NAME;

        if (!RemoveWorkDirectory(path.wstring())) return std::nullopt;

        std::error_code error;
        std::filesystem::create_directory(path, error);

        if (error.value() != ERROR_SUCCESS) {
            std::cerr << "Failed to create the work directory\n";
            DisplayError(HRESULT_FROM_WIN32(static_cast<DWORD>(error.value())));

            return std::nullopt;
        }

        return path.wstring();
    }


    std::optional<std::wstring> ResolveNtDirectory(const std::wstring& path) {
        const DWORD bufferWcharCount = GetFullPathNameW(path.c_str(), 0u, nullptr, nullptr);
        std::wstring fullPath(bufferWcharCount, L'\0');
        const DWORD pathWcharCount = GetFullPathNameW(path.c_str(), bufferWcharCount, fullPath.data(), nullptr);

        if (!pathWcharCount) {
            std::cerr << "Failed to resolve the directory path\n";
            DisplayError(HRESULT_FROM_WIN32(GetLastError()));

            return std::nullopt;
        }

        fullPath.resize(pathWcharCount);

        if (fullPath.ends_with(L'\\')) {
            fullPath.pop_back();
        }

        if (fullPath.starts_with(L"\\\\?\\")) return std::format(L"\\??\\{}", fullPath.substr(4u));

        if (fullPath.starts_with(L"\\\\")) return std::format(L"\\??\\UNC\\{}", fullPath.substr(2u));

        return std::format(L"\\??\\{}", fullPath);
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

    if (!VerifyDirectory(*parameters->directory)) return EXIT_FAILURE;

    const std::optional<std::wstring> workDirectory = CreateWorkDirectory(*parameters->directory);

    if (!workDirectory.has_value()) return EXIT_FAILURE;

    const std::optional<std::wstring> ntDirectory = ResolveNtDirectory(*workDirectory);

    if (!ntDirectory.has_value()) {
        RemoveWorkDirectory(*workDirectory);

        return EXIT_FAILURE;
    }

    test::Counts counts{};
    operations::create::Run(*ntDirectory, counts);
    const bool removed = RemoveWorkDirectory(*workDirectory);
    std::cout << std::format("Operations: {}, unexpected: {}\n", counts.operations, counts.unexpected);

    return counts.unexpected || !removed ? EXIT_FAILURE : EXIT_SUCCESS;
}
