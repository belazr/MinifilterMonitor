#include "info.h"

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
#include <span>
#include <string_view>
#include <vector>

extern "C" NTSYSAPI NTSTATUS NTAPI NtQueryInformationFile(
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
        { { L"query-basic",         STATUS_SUCCESS         }, kernel::FileBasicInformation,               1024u },
        { { L"query-standard",      STATUS_SUCCESS         }, kernel::FileStandardInformation,            1024u },
        { { L"query-internal",      STATUS_SUCCESS         }, kernel::FileInternalInformation,            1024u },
        { { L"query-ea",            STATUS_SUCCESS         }, kernel::FileEaInformation,                  1024u },
        { { L"query-name",          STATUS_SUCCESS         }, kernel::FileNameInformation,                1024u },
        { { L"query-name-overflow", STATUS_BUFFER_OVERFLOW }, kernel::FileNameInformation,                8u    },
        { { L"query-position",      STATUS_SUCCESS         }, kernel::FilePositionInformation,            1024u },
        { { L"query-all",           STATUS_SUCCESS         }, kernel::FileAllInformation,                 1024u },
        { { L"query-stream",        STATUS_SUCCESS         }, kernel::FileStreamInformation,              1024u },
        { { L"query-compression",   STATUS_SUCCESS         }, kernel::FileCompressionInformation,         1024u },
        { { L"query-network-open",  STATUS_SUCCESS         }, kernel::FileNetworkOpenInformation,         1024u },
        { { L"query-attribute-tag", STATUS_SUCCESS         }, kernel::FileAttributeTagInformation,        1024u },
        { { L"query-hard-link",     STATUS_SUCCESS         }, kernel::FileHardLinkInformation,            1024u },
        { { L"query-process-ids",   STATUS_SUCCESS         }, kernel::FileProcessIdsUsingFileInformation, 1024u },
        { { L"query-standard-link", STATUS_SUCCESS         }, kernel::FileStandardLinkInformation,        1024u },
        { { L"query-volume-name",   STATUS_SUCCESS         }, kernel::FileVolumeNameInformation,          1024u },
        { { L"query-id",            STATUS_SUCCESS         }, kernel::FileIdInformation,                  1024u },
        { { L"query-stat",          STATUS_SUCCESS         }, kernel::FileStatInformation,                1024u },
        { { L"query-stat-lx",       STATUS_SUCCESS         }, kernel::FileStatLxInformation,              1024u },
    };

    NTSTATUS Query(
        const test::Target& target,
        uint32_t infoClass,
        std::span<std::byte> data
    ) {
        IO_STATUS_BLOCK ioStatus{};

        return NtQueryInformationFile(target.Get(), &ioStatus, data.data(), static_cast<ULONG>(data.size()), static_cast<ULONG>(infoClass));
    }


    constexpr std::string_view QUERY_MAJOR = "Query information";

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

            std::vector<std::byte> data(operation.size);
            const NTSTATUS queryStatus = Query(target, operation.infoClass, data);
            test::report::Add(counts, QUERY_MAJOR, operation.description, queryStatus);
        }

        return;
    }

}

namespace mimo {

    namespace test {

        namespace operations {

            namespace info {

                bool Run(std::wstring_view ntDirectory, report::Counts& counts) {
                    RunQueryOperations(ntDirectory, counts);

                    return true;
                }

            }

        }

    }

}
