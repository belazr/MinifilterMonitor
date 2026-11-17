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
#include <iostream>
#include <optional>
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
    };

    constexpr QueryOperation QUERY_OPERATIONS[]{
        { { L"directory-control-query-directory",        STATUS_SUCCESS }, kernel::FileDirectoryInformation              },
        { { L"directory-control-query-full",             STATUS_SUCCESS }, kernel::FileFullDirectoryInformation          },
        { { L"directory-control-query-both",             STATUS_SUCCESS }, kernel::FileBothDirectoryInformation          },
        { { L"directory-control-query-names",            STATUS_SUCCESS }, kernel::FileNamesInformation                  },
        { { L"directory-control-query-id-both",          STATUS_SUCCESS }, kernel::FileIdBothDirectoryInformation        },
        { { L"directory-control-query-id-full",          STATUS_SUCCESS }, kernel::FileIdFullDirectoryInformation        },
        { { L"directory-control-query-id-global-tx",     STATUS_SUCCESS }, kernel::FileIdGlobalTxDirectoryInformation    },
        { { L"directory-control-query-id-extd",          STATUS_SUCCESS }, kernel::FileIdExtdDirectoryInformation        },
        { { L"directory-control-query-id-extd-both",     STATUS_SUCCESS }, kernel::FileIdExtdBothDirectoryInformation    },
        { { L"directory-control-query-id-64-extd",       STATUS_SUCCESS }, kernel::FileId64ExtdDirectoryInformation      },
        { { L"directory-control-query-id-64-extd-both",  STATUS_SUCCESS }, kernel::FileId64ExtdBothDirectoryInformation  },
        { { L"directory-control-query-id-all-extd",      STATUS_SUCCESS }, kernel::FileIdAllExtdDirectoryInformation     },
        { { L"directory-control-query-id-all-extd-both", STATUS_SUCCESS }, kernel::FileIdAllExtdBothDirectoryInformation },
    };

    NTSTATUS Query(const test::Target& target, uint32_t infoClass) {
        constexpr size_t BUFFER_SIZE = 4096u;
        std::vector<std::byte> buffer(BUFFER_SIZE);
        IO_STATUS_BLOCK ioStatus{};

        return NtQueryDirectoryFile(target.Get(), nullptr, nullptr, nullptr, &ioStatus, buffer.data(), static_cast<ULONG>(buffer.size()), static_cast<ULONG>(infoClass), FALSE, nullptr, FALSE);
    }


    constexpr std::string_view MAJOR = "Directory control";

    void RunQueryOperations(std::wstring_view ntDirectory, test::report::Counts& counts) {
        constexpr std::wstring_view ENTRY_NAME = L"entry";

        for (const QueryOperation& operation : QUERY_OPERATIONS) {
            test::Target target{ ntDirectory, operation.description.name };
            const NTSTATUS createStatus = target.Create(FILE_LIST_DIRECTORY, false, true);

            if (!NT_SUCCESS(createStatus)) {
                std::cerr << "Failed to create the target directory\n";
                error::Display(RtlNtStatusToDosError(createStatus));
                test::report::Add(counts, MAJOR, operation.description, std::nullopt);

                continue;
            }

            test::Target entry{ target.GetPath(), ENTRY_NAME };
            const NTSTATUS entryStatus = entry.Create(FILE_READ_ATTRIBUTES);

            if (!NT_SUCCESS(entryStatus)) {
                std::cerr << "Failed to create the target directory entry\n";
                error::Display(RtlNtStatusToDosError(entryStatus));
                test::report::Add(counts, MAJOR, operation.description, std::nullopt);

                continue;
            }

            const NTSTATUS queryStatus = Query(target, operation.infoClass);
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
