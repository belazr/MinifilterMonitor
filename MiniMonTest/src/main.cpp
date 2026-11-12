#include "error.h"
#include "test.h"

#include <Windows.h>

#include <algorithm>
#include <cstdlib>
#include <cwctype>
#include <ios>
#include <iostream>
#include <optional>
#include <ostream>
#include <string>
#include <string_view>

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


    bool VerifyDirectory(const std::wstring& path) {
        const DWORD attributes = GetFileAttributesW(path.c_str());

        if (attributes == INVALID_FILE_ATTRIBUTES) {
            const DWORD lastError = GetLastError();
            std::cerr << "Failed to query directory attributes\n";
            error::Display(lastError);

            return false;
        }

        if (!(attributes & FILE_ATTRIBUTE_DIRECTORY)) {
            std::cerr << "Not a directory\n";

            return false;
        }

        return true;
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

    return test::Run(*parameters->directory) ? EXIT_SUCCESS : EXIT_FAILURE;
}
