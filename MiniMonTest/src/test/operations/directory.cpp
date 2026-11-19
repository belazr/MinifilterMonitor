#include "directory.h"

#include "..\report.h"
#include "..\target.h"

#include "..\..\error.h"

#include "..\..\..\..\inc\kernel.h"

#include <Windows.h>
#undef WIN32_NO_STATUS
#include <ntstatus.h>
#include <winternl.h>

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <format>
#include <iostream>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>

extern "C" NTSYSAPI NTSTATUS NTAPI NtQueryDirectoryFile(
    HANDLE hFile,
    HANDLE hEvent,
    PIO_APC_ROUTINE pApcRoutine,
    void* pApcContext,
    IO_STATUS_BLOCK* pIoStatus,
    void* pBuffer,
    ULONG bufferSize,
    ULONG infoClass,
    BOOLEAN returnSingleEntry,
    UNICODE_STRING* pFileName,
    BOOLEAN restartScan
);

extern "C" NTSYSAPI NTSTATUS NTAPI NtNotifyChangeDirectoryFile(
    HANDLE hFile,
    HANDLE hEvent,
    PIO_APC_ROUTINE pApcRoutine,
    void* pApcContext,
    IO_STATUS_BLOCK* pIoStatus,
    void* pBuffer,
    ULONG bufferSize,
    ULONG completionFilter,
    BOOLEAN watchTree
);

extern "C" NTSYSAPI NTSTATUS NTAPI NtNotifyChangeDirectoryFileEx(
    HANDLE hFile,
    HANDLE hEvent,
    PIO_APC_ROUTINE pApcRoutine,
    void* pApcContext,
    IO_STATUS_BLOCK* pIoStatus,
    void* pBuffer,
    ULONG bufferSize,
    ULONG completionFilter,
    BOOLEAN watchTree,
    ULONG infoClass
);

using namespace mimo;

namespace {

    struct QueryOperation {
        test::report::Description description;
        uint32_t infoClass;
        uint32_t entryCount;
        bool single;
        bool filtered;
    };

    constexpr QueryOperation QUERY_OPERATIONS[]{
        { { L"directory-control-query-directory",        STATUS_SUCCESS }, kernel::FileDirectoryInformation,              1u, false, false },
        { { L"directory-control-query-full",             STATUS_SUCCESS }, kernel::FileFullDirectoryInformation,          1u, false, false },
        { { L"directory-control-query-both",             STATUS_SUCCESS }, kernel::FileBothDirectoryInformation,          1u, false, false },
        { { L"directory-control-query-names",            STATUS_SUCCESS }, kernel::FileNamesInformation,                  1u, false, false },
        { { L"directory-control-query-id-both",          STATUS_SUCCESS }, kernel::FileIdBothDirectoryInformation,        1u, false, false },
        { { L"directory-control-query-id-full",          STATUS_SUCCESS }, kernel::FileIdFullDirectoryInformation,        1u, false, false },
        { { L"directory-control-query-id-global-tx",     STATUS_SUCCESS }, kernel::FileIdGlobalTxDirectoryInformation,    1u, false, false },
        { { L"directory-control-query-id-extd",          STATUS_SUCCESS }, kernel::FileIdExtdDirectoryInformation,        1u, false, false },
        { { L"directory-control-query-id-extd-both",     STATUS_SUCCESS }, kernel::FileIdExtdBothDirectoryInformation,    1u, false, false },
        { { L"directory-control-query-id-64-extd",       STATUS_SUCCESS }, kernel::FileId64ExtdDirectoryInformation,      1u, false, false },
        { { L"directory-control-query-id-64-extd-both",  STATUS_SUCCESS }, kernel::FileId64ExtdBothDirectoryInformation,  1u, false, false },
        { { L"directory-control-query-id-all-extd",      STATUS_SUCCESS }, kernel::FileIdAllExtdDirectoryInformation,     1u, false, false },
        { { L"directory-control-query-id-all-extd-both", STATUS_SUCCESS }, kernel::FileIdAllExtdBothDirectoryInformation, 1u, false, false },
        { { L"directory-control-query-oversized",        STATUS_SUCCESS }, kernel::FileDirectoryInformation,              8u, false, false },
        { { L"directory-control-query-single",           STATUS_SUCCESS }, kernel::FileDirectoryInformation,              1u, true,  false },
        { { L"directory-control-query-filter",           STATUS_SUCCESS }, kernel::FileDirectoryInformation,              1u, false, true  },
    };

    NTSTATUS CreateEntries(std::wstring_view ntDirectory, uint32_t count) {
        constexpr std::wstring_view NAME = L"entry";

        for (uint32_t i = 1u; i <= count; i++) {
            test::Target entry{ ntDirectory, std::format(L"{}{}", NAME, i) };
            const NTSTATUS status = entry.Create(FILE_READ_ATTRIBUTES);

            if (!NT_SUCCESS(status)) return status;

        }

        return STATUS_SUCCESS;
    }


    NTSTATUS Query(
        const test::Target& target,
        uint32_t infoClass,
        bool single,
        std::wstring_view filter
    ) {
        constexpr size_t BUFFER_SIZE = 4096u;
        std::vector<std::byte> buffer(BUFFER_SIZE);
        std::wstring filterBuffer{ filter };
        UNICODE_STRING fileName{};
        fileName.Length = static_cast<USHORT>(filterBuffer.size() * sizeof(wchar_t));
        fileName.MaximumLength = fileName.Length;
        fileName.Buffer = filterBuffer.data();

        IO_STATUS_BLOCK ioStatus{};

        return NtQueryDirectoryFile(target.Get(), nullptr, nullptr, nullptr, &ioStatus, buffer.data(), static_cast<ULONG>(buffer.size()), static_cast<ULONG>(infoClass), static_cast<BOOLEAN>(single), filter.empty() ? nullptr : &fileName, FALSE);
    }


    constexpr std::string_view MAJOR = "Directory control";

    void RunQueryOperations(std::wstring_view ntDirectory, test::report::Counts& counts) {
        constexpr std::wstring_view FILTER = L"entry*";

        for (const QueryOperation& operation : QUERY_OPERATIONS) {
            test::Target target{ ntDirectory, operation.description.name };
            const NTSTATUS createStatus = target.Create(FILE_LIST_DIRECTORY, false, true);

            if (!NT_SUCCESS(createStatus)) {
                std::cerr << "Failed to create the target directory\n";
                error::Display(RtlNtStatusToDosError(createStatus));
                test::report::Add(counts, MAJOR, operation.description, std::nullopt);

                continue;
            }

            const NTSTATUS entriesStatus = CreateEntries(target.GetPath(), operation.entryCount);

            if (!NT_SUCCESS(entriesStatus)) {
                std::cerr << "Failed to create the target directory entries\n";
                error::Display(RtlNtStatusToDosError(entriesStatus));
                test::report::Add(counts, MAJOR, operation.description, std::nullopt);

                continue;
            }

            const std::wstring_view filter = operation.filtered ? FILTER : std::wstring_view{};
            const NTSTATUS queryStatus = Query(target, operation.infoClass, operation.single, filter);
            test::report::Add(counts, MAJOR, operation.description, queryStatus);
        }

        return;
    }


    struct NotifyOperation {
        test::report::Description description;
        uint32_t infoClass;
        bool watchTree;
    };

    constexpr NotifyOperation NOTIFY_OPERATIONS[]{
        { { L"directory-control-notify",             STATUS_SUCCESS }, 0u,                                         false },
        { { L"directory-control-notify-watch-tree",  STATUS_SUCCESS }, 0u,                                         true  },
        { { L"directory-control-notify-ex",          STATUS_SUCCESS }, kernel::DirectoryNotifyInformation,         false },
        { { L"directory-control-notify-ex-extended", STATUS_SUCCESS }, kernel::DirectoryNotifyExtendedInformation, false },
        { { L"directory-control-notify-ex-full",     STATUS_SUCCESS }, kernel::DirectoryNotifyFullInformation,     false },
    };

    NTSTATUS NotifyEx(
        const test::Target& target,
        IO_STATUS_BLOCK& ioStatus,
        std::span<std::byte> buffer,
        uint32_t infoClass
    ) {

        return NtNotifyChangeDirectoryFileEx(target.Get(), nullptr, nullptr, nullptr, &ioStatus, buffer.data(), static_cast<ULONG>(buffer.size()), FILE_NOTIFY_CHANGE_FILE_NAME, FALSE, static_cast<ULONG>(infoClass));
    }


    NTSTATUS Notify(
        const test::Target& target,
        IO_STATUS_BLOCK& ioStatus,
        std::span<std::byte> buffer,
        bool watchTree
    ) {

        return NtNotifyChangeDirectoryFile(target.Get(), nullptr, nullptr, nullptr, &ioStatus, buffer.data(), static_cast<ULONG>(buffer.size()), FILE_NOTIFY_CHANGE_FILE_NAME, static_cast<BOOLEAN>(watchTree));
    }


    bool WaitForCompletion(const test::Target& target) {
        constexpr std::chrono::seconds TIMEOUT{ 5 };
        const DWORD waitResult = WaitForSingleObject(target.Get(), static_cast<DWORD>(std::chrono::milliseconds{ TIMEOUT }.count()));

        if (waitResult == WAIT_FAILED) {
            const DWORD lastError = GetLastError();
            std::cerr << "Failed to wait for the notify completion\n";
            error::Display(lastError);

            return false;
        }

        if (waitResult != WAIT_OBJECT_0) {
            std::cerr << "The notify did not complete in time\n";

            return false;
        }

        return true;
    }


    void RunNotifyOperations(std::wstring_view ntDirectory, test::report::Counts& counts) {
        constexpr size_t BUFFER_SIZE = 4096u;

        for (const NotifyOperation& operation : NOTIFY_OPERATIONS) {
            // closing the target completes a waiting notify into the buffer and the status block, so they have to outlive it
            std::vector<std::byte> buffer(BUFFER_SIZE);
            IO_STATUS_BLOCK ioStatus{};
            ioStatus.Status = STATUS_PENDING;
            test::Target target{ ntDirectory, operation.description.name };
            const NTSTATUS createStatus = target.Create(FILE_LIST_DIRECTORY, true, true);

            if (!NT_SUCCESS(createStatus)) {
                std::cerr << "Failed to create the target directory\n";
                error::Display(RtlNtStatusToDosError(createStatus));
                test::report::Add(counts, MAJOR, operation.description, std::nullopt);

                continue;
            }

            const NTSTATUS notifyStatus = operation.infoClass ? NotifyEx(target, ioStatus, buffer, operation.infoClass) : Notify(target, ioStatus, buffer, operation.watchTree);

            if (notifyStatus != STATUS_PENDING) {
                test::report::Add(counts, MAJOR, operation.description, notifyStatus);

                continue;
            }

            const NTSTATUS entriesStatus = CreateEntries(target.GetPath(), 1u);

            if (!NT_SUCCESS(entriesStatus)) {
                std::cerr << "Failed to create the target directory entries\n";
                error::Display(RtlNtStatusToDosError(entriesStatus));
                test::report::Add(counts, MAJOR, operation.description, std::nullopt);

                continue;
            }

            if (!WaitForCompletion(target)) {
                test::report::Add(counts, MAJOR, operation.description, std::nullopt);

                continue;
            }

            test::report::Add(counts, MAJOR, operation.description, ioStatus.Status);
        }

        return;
    }

}

namespace mimo {

    namespace test {

        namespace operations {

            namespace directory {

                bool Run(std::wstring_view ntDirectory, report::Counts& counts) {
                    RunQueryOperations(ntDirectory, counts);
                    RunNotifyOperations(ntDirectory, counts);

                    return true;
                }

            }

        }

    }

}
