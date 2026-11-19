#include "directory.h"

#include "..\report.h"
#include "..\target.h"

#include "..\..\error.h"

#include "..\..\..\..\inc\kernel.h"

#include <Windows.h>
#undef WIN32_NO_STATUS
#include <ntstatus.h>
#include <winternl.h>

#include <cstddef>
#include <cstdint>
#include <format>
#include <iostream>
#include <optional>
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

}

namespace mimo {

    namespace test {

        namespace operations {

            namespace directory {

                bool Run(std::wstring_view ntDirectory, report::Counts& counts) {
                    RunQueryOperations(ntDirectory, counts);

                    return true;
                }

            }

        }

    }

}
