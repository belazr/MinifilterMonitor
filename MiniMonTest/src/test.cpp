#include "test.h"

#include "test\operations\create.h"
#include "test\operations\readwrite.h"
#include "test\report.h"

#include "error.h"

#include <Windows.h>

#include <filesystem>
#include <format>
#include <iostream>
#include <optional>
#include <string>
#include <string_view>
#include <system_error>

using namespace mimo;

namespace {

    bool RemoveWorkDirectory(std::wstring_view path) {
        std::error_code errorCode;
        std::filesystem::remove_all(path, errorCode);

        if (errorCode.value() != ERROR_SUCCESS) {
            std::cerr << "Failed to remove the work directory\n";
            error::Display(static_cast<DWORD>(errorCode.value()));

            return false;
        }

        return true;
    }


    std::optional<std::wstring> CreateWorkDirectory(std::wstring_view directory) {
        constexpr std::wstring_view NAME = L"MiniMonTest.tmp";
        const std::filesystem::path path = std::filesystem::path(directory) / NAME;

        if (!RemoveWorkDirectory(path.wstring())) return std::nullopt;

        std::error_code errorCode;
        std::filesystem::create_directory(path, errorCode);

        if (errorCode.value() != ERROR_SUCCESS) {
            std::cerr << "Failed to create the work directory\n";
            error::Display(static_cast<DWORD>(errorCode.value()));

            return std::nullopt;
        }

        return path.wstring();
    }


    std::optional<std::wstring> ResolveNtDirectory(const std::wstring& path) {
        const DWORD bufferWcharCount = GetFullPathNameW(path.c_str(), 0u, nullptr, nullptr);
        std::wstring fullPath(bufferWcharCount, L'\0');
        const DWORD pathWcharCount = GetFullPathNameW(path.c_str(), bufferWcharCount, fullPath.data(), nullptr);

        if (!pathWcharCount) {
            const DWORD lastError = GetLastError();
            std::cerr << "Failed to resolve the directory path\n";
            error::Display(lastError);

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

namespace mimo {

    namespace test {

        bool Run(std::wstring_view directory) {
            const std::optional<std::wstring> workDirectory = CreateWorkDirectory(directory);

            if (!workDirectory.has_value()) return false;

            const std::optional<std::wstring> ntDirectory = ResolveNtDirectory(*workDirectory);

            if (!ntDirectory.has_value()) {
                RemoveWorkDirectory(*workDirectory);

                return false;
            }

            report::Counts counts{};
            const bool completed = operations::create::Run(*ntDirectory, counts) && operations::readwrite::Run(*ntDirectory, counts);
            const bool removed = RemoveWorkDirectory(*workDirectory);
            std::cout << std::format("Operations: {}, not executed: {}, unexpected: {}\n", counts.operations, counts.notExecuted, counts.unexpected);

            return completed && !counts.notExecuted && !counts.unexpected && removed;
        }

    }

}
