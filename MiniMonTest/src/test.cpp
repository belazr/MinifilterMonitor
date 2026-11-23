#include "test.h"

#include "test\operations\create.h"
#include "test\operations\deviceio.h"
#include "test\operations\directory.h"
#include "test\operations\ea.h"
#include "test\operations\filesystem.h"
#include "test\operations\flush.h"
#include "test\operations\info.h"
#include "test\operations\lock.h"
#include "test\operations\quota.h"
#include "test\operations\readwrite.h"
#include "test\operations\security.h"
#include "test\operations\volume.h"
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

    constexpr std::wstring_view EXTENDED_PREFIX = L"\\\\?\\";
    constexpr std::wstring_view DEVICE_PREFIX   = L"\\\\.\\";

    std::optional<std::wstring> ResolveExtendedDirectory(const std::wstring& path) {
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

        if (fullPath.starts_with(EXTENDED_PREFIX)) return fullPath;

        if (fullPath.starts_with(DEVICE_PREFIX)) return std::format(L"{}{}", EXTENDED_PREFIX, fullPath.substr(DEVICE_PREFIX.size()));

        if (fullPath.starts_with(L"\\\\")) return std::format(L"{}UNC\\{}", EXTENDED_PREFIX, fullPath.substr(2u));

        return std::format(L"{}{}", EXTENDED_PREFIX, fullPath);
    }


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
        const std::wstring path = std::format(L"{}\\{}", directory, NAME);

        if (!RemoveWorkDirectory(path)) return std::nullopt;

        std::error_code errorCode;
        std::filesystem::create_directory(path, errorCode);

        if (errorCode.value() != ERROR_SUCCESS) {
            std::cerr << "Failed to create the work directory\n";
            error::Display(static_cast<DWORD>(errorCode.value()));

            return std::nullopt;
        }

        return path;
    }

}

namespace mimo {

    namespace test {

        bool Run(std::wstring_view directory) {
            const std::optional<std::wstring> extendedDirectory = ResolveExtendedDirectory(std::wstring{ directory });

            if (!extendedDirectory.has_value()) return false;

            const std::optional<std::wstring> workDirectory = CreateWorkDirectory(*extendedDirectory);

            if (!workDirectory.has_value()) return false;

            const std::wstring ntDirectory = std::format(L"\\??\\{}", std::wstring_view{ *workDirectory }.substr(EXTENDED_PREFIX.size()));
            report::Counts counts{};
            const bool completed = operations::create::Run(ntDirectory, counts) && operations::readwrite::Run(ntDirectory, counts) && operations::info::Run(ntDirectory, counts) && operations::ea::Run(ntDirectory, counts) && operations::flush::Run(ntDirectory, counts) && operations::volume::Run(ntDirectory, counts) && operations::directory::Run(ntDirectory, counts) && operations::filesystem::Run(ntDirectory, counts) && operations::deviceio::Run(ntDirectory, counts) && operations::lock::Run(ntDirectory, counts) && operations::security::Run(ntDirectory, counts) && operations::quota::Run(ntDirectory, counts);
            const bool removed = RemoveWorkDirectory(*workDirectory);
            std::cout << std::format("Operations: {}, not executed: {}, unexpected: {}\n", counts.operations, counts.notExecuted, counts.unexpected);

            return completed && !counts.notExecuted && !counts.unexpected && removed;
        }

    }

}
