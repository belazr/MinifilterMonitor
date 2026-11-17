#include "volume.h"

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

extern "C" NTSYSAPI NTSTATUS NTAPI NtQueryVolumeInformationFile(
    HANDLE hFile,
    IO_STATUS_BLOCK* pIoStatus,
    void* pBuffer,
    ULONG bufferSize,
    ULONG infoClass
);

using namespace mimo;

namespace {

    struct QueryOperation {
        test::report::Description description;
        uint32_t infoClass;
    };

    constexpr QueryOperation QUERY_OPERATIONS[]{
        { { L"query-volume-info-volume",       STATUS_SUCCESS }, kernel::FileFsVolumeInformation     },
        { { L"query-volume-info-size",         STATUS_SUCCESS }, kernel::FileFsSizeInformation       },
        { { L"query-volume-info-attribute",    STATUS_SUCCESS }, kernel::FileFsAttributeInformation  },
        { { L"query-volume-info-control",      STATUS_SUCCESS }, kernel::FileFsControlInformation    },
        { { L"query-volume-info-full-size",    STATUS_SUCCESS }, kernel::FileFsFullSizeInformation   },
        { { L"query-volume-info-object-id",    STATUS_SUCCESS }, kernel::FileFsObjectIdInformation   },
        { { L"query-volume-info-sector-size",  STATUS_SUCCESS }, kernel::FileFsSectorSizeInformation },
        { { L"query-volume-info-data-copy",    STATUS_SUCCESS }, kernel::FileFsDataCopyInformation   },
        { { L"query-volume-info-full-size-ex", STATUS_SUCCESS }, kernel::FileFsFullSizeInformationEx },
    };

    NTSTATUS Query(const test::Target& target, uint32_t infoClass) {
        constexpr size_t BUFFER_SIZE = 1024u;
        std::vector<std::byte> buffer(BUFFER_SIZE);
        IO_STATUS_BLOCK ioStatus{};

        return NtQueryVolumeInformationFile(target.Get(), &ioStatus, buffer.data(), static_cast<ULONG>(buffer.size()), static_cast<ULONG>(infoClass));
    }


    constexpr std::string_view QUERY_MAJOR = "Query volume information";

    void RunQueryOperations(std::wstring_view ntDirectory, test::report::Counts& counts) {

        for (const QueryOperation& operation : QUERY_OPERATIONS) {
            test::Target target{ ntDirectory, operation.description.name };
            const NTSTATUS createStatus = target.Create(FILE_GENERIC_READ);

            if (!NT_SUCCESS(createStatus)) {
                std::cerr << "Failed to create the target file\n";
                error::Display(RtlNtStatusToDosError(createStatus));
                test::report::Add(counts, QUERY_MAJOR, operation.description, std::nullopt);

                continue;
            }

            const NTSTATUS queryStatus = Query(target, operation.infoClass);
            test::report::Add(counts, QUERY_MAJOR, operation.description, queryStatus);
        }

        return;
    }

}

namespace mimo {

    namespace test {

        namespace operations {

            namespace volume {

                bool Run(std::wstring_view ntDirectory, report::Counts& counts) {
                    RunQueryOperations(ntDirectory, counts);

                    return true;
                }

            }

        }

    }

}
