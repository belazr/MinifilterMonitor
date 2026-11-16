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
        uint32_t size;
    };

    constexpr QueryOperation QUERY_OPERATIONS[]{
        { { L"query-volume-volume",             STATUS_SUCCESS         }, kernel::FileFsVolumeInformation,     1024u },
        { { L"query-volume-size",               STATUS_SUCCESS         }, kernel::FileFsSizeInformation,       1024u },
        { { L"query-volume-attribute",          STATUS_SUCCESS         }, kernel::FileFsAttributeInformation,  1024u },
        { { L"query-volume-attribute-overflow", STATUS_BUFFER_OVERFLOW }, kernel::FileFsAttributeInformation,  16u   },
        { { L"query-volume-control",            STATUS_SUCCESS         }, kernel::FileFsControlInformation,    1024u },
        { { L"query-volume-full-size",          STATUS_SUCCESS         }, kernel::FileFsFullSizeInformation,   1024u },
        { { L"query-volume-object-id",          STATUS_SUCCESS         }, kernel::FileFsObjectIdInformation,   1024u },
        { { L"query-volume-sector-size",        STATUS_SUCCESS         }, kernel::FileFsSectorSizeInformation, 1024u },
        { { L"query-volume-data-copy",          STATUS_SUCCESS         }, kernel::FileFsDataCopyInformation,   1024u },
        { { L"query-volume-full-size-ex",       STATUS_SUCCESS         }, kernel::FileFsFullSizeInformationEx, 1024u },
    };

    NTSTATUS Query(
        const test::Target& target,
        uint32_t infoClass,
        uint32_t size
    ) {
        std::vector<std::byte> buffer(size);
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

            const NTSTATUS queryStatus = Query(target, operation.infoClass, operation.size);
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
