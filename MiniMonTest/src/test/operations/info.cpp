#include "info.h"

#include "..\report.h"
#include "..\target.h"

#include "..\..\error.h"

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

    constexpr uint32_t FileBasicInformation               = 4u;
    constexpr uint32_t FileStandardInformation            = 5u;
    constexpr uint32_t FileInternalInformation            = 6u;
    constexpr uint32_t FileEaInformation                  = 7u;
    constexpr uint32_t FileNameInformation                = 9u;
    constexpr uint32_t FilePositionInformation            = 14u;
    constexpr uint32_t FileAllInformation                 = 18u;
    constexpr uint32_t FileStreamInformation              = 22u;
    constexpr uint32_t FileCompressionInformation         = 28u;
    constexpr uint32_t FileNetworkOpenInformation         = 34u;
    constexpr uint32_t FileAttributeTagInformation        = 35u;
    constexpr uint32_t FileHardLinkInformation            = 46u;
    constexpr uint32_t FileProcessIdsUsingFileInformation = 47u;
    constexpr uint32_t FileStandardLinkInformation        = 54u;
    constexpr uint32_t FileVolumeNameInformation          = 58u;
    constexpr uint32_t FileIdInformation                  = 59u;
    constexpr uint32_t FileStatInformation                = 68u;
    constexpr uint32_t FileStatLxInformation              = 70u;

    struct QueryOperation {
        test::report::Description description;
        uint32_t infoClass;
        uint32_t size;
    };

    constexpr QueryOperation QUERY_OPERATIONS[]{
        { { L"query-basic",         STATUS_SUCCESS         }, FileBasicInformation,               1024u },
        { { L"query-standard",      STATUS_SUCCESS         }, FileStandardInformation,            1024u },
        { { L"query-internal",      STATUS_SUCCESS         }, FileInternalInformation,            1024u },
        { { L"query-ea",            STATUS_SUCCESS         }, FileEaInformation,                  1024u },
        { { L"query-name",          STATUS_SUCCESS         }, FileNameInformation,                1024u },
        { { L"query-name-overflow", STATUS_BUFFER_OVERFLOW }, FileNameInformation,                8u    },
        { { L"query-position",      STATUS_SUCCESS         }, FilePositionInformation,            1024u },
        { { L"query-all",           STATUS_SUCCESS         }, FileAllInformation,                 1024u },
        { { L"query-stream",        STATUS_SUCCESS         }, FileStreamInformation,              1024u },
        { { L"query-compression",   STATUS_SUCCESS         }, FileCompressionInformation,         1024u },
        { { L"query-network-open",  STATUS_SUCCESS         }, FileNetworkOpenInformation,         1024u },
        { { L"query-attribute-tag", STATUS_SUCCESS         }, FileAttributeTagInformation,        1024u },
        { { L"query-hard-link",     STATUS_SUCCESS         }, FileHardLinkInformation,            1024u },
        { { L"query-process-ids",   STATUS_SUCCESS         }, FileProcessIdsUsingFileInformation, 1024u },
        { { L"query-standard-link", STATUS_SUCCESS         }, FileStandardLinkInformation,        1024u },
        { { L"query-volume-name",   STATUS_SUCCESS         }, FileVolumeNameInformation,          1024u },
        { { L"query-id",            STATUS_SUCCESS         }, FileIdInformation,                  1024u },
        { { L"query-stat",          STATUS_SUCCESS         }, FileStatInformation,                1024u },
        { { L"query-stat-lx",       STATUS_SUCCESS         }, FileStatLxInformation,              1024u },
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
