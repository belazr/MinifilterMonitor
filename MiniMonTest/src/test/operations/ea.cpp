#include "ea.h"

#include "..\report.h"
#include "..\target.h"

#include "..\..\error.h"

#include <Windows.h>
#undef WIN32_NO_STATUS
#include <ntstatus.h>
#include <winternl.h>

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <optional>
#include <span>
#include <string_view>
#include <vector>

extern "C" NTSYSAPI NTSTATUS NTAPI NtSetEaFile(
    HANDLE hFile,
    IO_STATUS_BLOCK* pIoStatus,
    void* pBuffer,
    ULONG bufferSize
);

extern "C" NTSYSAPI NTSTATUS NTAPI NtQueryEaFile(
    HANDLE hFile,
    IO_STATUS_BLOCK* pIoStatus,
    void* pBuffer,
    ULONG bufferSize,
    BOOLEAN returnSingleEntry,
    void* pEaList,
    ULONG eaListSize,
    ULONG* pEaIndex,
    BOOLEAN restartScan
);

using namespace mimo;

namespace {

    struct QueryOperation {
        test::report::Description description;
        uint16_t valueSize;
        uint32_t size;
        bool listed;
    };

    constexpr QueryOperation QUERY_OPERATIONS[]{
        { { L"query-ea",           STATUS_SUCCESS        }, 8u,    1024u, false },
        { { L"query-ea-list",      STATUS_SUCCESS        }, 8u,    1024u, true  },
        { { L"query-ea-oversized", STATUS_SUCCESS        }, 2048u, 4096u, false },
        { { L"query-ea-missing",   STATUS_NO_EAS_ON_FILE }, 0u,    1024u, false },
    };

    constexpr std::string_view EA_NAME = "MINIMON";

    struct FILE_FULL_EA_INFORMATION {
        ULONG NextEntryOffset;
        UCHAR Flags;
        UCHAR EaNameLength;
        USHORT EaValueLength;
        CHAR EaName[1u];
    };

    std::vector<std::byte> MakeEaBuffer(uint16_t valueSize) {
        constexpr size_t NAME_OFFSET = offsetof(FILE_FULL_EA_INFORMATION, EaName);
        FILE_FULL_EA_INFORMATION entry{};
        entry.EaNameLength = static_cast<UCHAR>(EA_NAME.size());
        entry.EaValueLength = static_cast<USHORT>(valueSize);

        std::vector<std::byte> buffer(NAME_OFFSET + EA_NAME.size() + 1u + valueSize);
        std::memcpy(buffer.data(), &entry, NAME_OFFSET);
        std::memcpy(buffer.data() + NAME_OFFSET, EA_NAME.data(), EA_NAME.size());

        return buffer;
    }


    NTSTATUS Set(const test::Target& target, std::span<const std::byte> data) {
        std::vector<std::byte> buffer(data.begin(), data.end());
        IO_STATUS_BLOCK ioStatus{};

        return NtSetEaFile(target.Get(), &ioStatus, buffer.data(), static_cast<ULONG>(buffer.size()));
    }


    struct FILE_GET_EA_INFORMATION {
        ULONG NextEntryOffset;
        UCHAR EaNameLength;
        CHAR EaName[1u];
    };

    std::vector<std::byte> MakeEaList() {
        constexpr size_t NAME_OFFSET = offsetof(FILE_GET_EA_INFORMATION, EaName);
        FILE_GET_EA_INFORMATION entry{};
        entry.EaNameLength = static_cast<UCHAR>(EA_NAME.size());

        std::vector<std::byte> buffer(NAME_OFFSET + EA_NAME.size() + 1u);
        std::memcpy(buffer.data(), &entry, NAME_OFFSET);
        std::memcpy(buffer.data() + NAME_OFFSET, EA_NAME.data(), EA_NAME.size());

        return buffer;
    }


    NTSTATUS Query(
        const test::Target& target,
        uint32_t size,
        std::span<const std::byte> eaList
    ) {
        std::vector<std::byte> listBuffer(eaList.begin(), eaList.end());
        std::vector<std::byte> buffer(size);
        IO_STATUS_BLOCK ioStatus{};

        return NtQueryEaFile(target.Get(), &ioStatus, buffer.data(), static_cast<ULONG>(buffer.size()), FALSE, listBuffer.data(), static_cast<ULONG>(listBuffer.size()), nullptr, TRUE);
    }


    constexpr std::string_view QUERY_MAJOR = "Query EA";

    void RunQueryOperations(std::wstring_view ntDirectory, test::report::Counts& counts) {

        for (const QueryOperation& operation : QUERY_OPERATIONS) {
            test::Target target{ ntDirectory, operation.description.name };
            const NTSTATUS createStatus = target.Create(FILE_READ_EA | FILE_WRITE_EA);

            if (!NT_SUCCESS(createStatus)) {
                std::cerr << "Failed to create the target file\n";
                error::Display(RtlNtStatusToDosError(createStatus));
                test::report::Add(counts, QUERY_MAJOR, operation.description, std::nullopt);

                continue;
            }

            const std::vector<std::byte> data = MakeEaBuffer(operation.valueSize);
            const NTSTATUS setStatus = Set(target, data);

            if (!NT_SUCCESS(setStatus)) {
                std::cerr << "Failed to set the target file EA\n";
                error::Display(RtlNtStatusToDosError(setStatus));
                test::report::Add(counts, QUERY_MAJOR, operation.description, std::nullopt);

                continue;
            }

            const std::vector<std::byte> eaList = operation.listed ? MakeEaList() : std::vector<std::byte>{};
            const NTSTATUS queryStatus = Query(target, operation.size, eaList);
            test::report::Add(counts, QUERY_MAJOR, operation.description, queryStatus);
        }

        return;
    }


    struct SetOperation {
        test::report::Description description;
        uint16_t valueSize;
    };

    constexpr SetOperation SET_OPERATIONS[]{
        { { L"set-ea",           STATUS_SUCCESS }, 8u    },
        { { L"set-ea-oversized", STATUS_SUCCESS }, 2048u },
    };

    constexpr std::string_view SET_MAJOR = "Set EA";

    void RunSetOperations(std::wstring_view ntDirectory, test::report::Counts& counts) {

        for (const SetOperation& operation : SET_OPERATIONS) {
            test::Target target{ ntDirectory, operation.description.name };
            const NTSTATUS createStatus = target.Create(FILE_WRITE_EA);

            if (!NT_SUCCESS(createStatus)) {
                std::cerr << "Failed to create the target file\n";
                error::Display(RtlNtStatusToDosError(createStatus));
                test::report::Add(counts, SET_MAJOR, operation.description, std::nullopt);

                continue;
            }

            const std::vector<std::byte> data = MakeEaBuffer(operation.valueSize);
            const NTSTATUS setStatus = Set(target, data);
            test::report::Add(counts, SET_MAJOR, operation.description, setStatus);
        }

        return;
    }

}

namespace mimo {

    namespace test {

        namespace operations {

            namespace ea {

                bool Run(std::wstring_view ntDirectory, report::Counts& counts) {
                    RunQueryOperations(ntDirectory, counts);
                    RunSetOperations(ntDirectory, counts);

                    return true;
                }

            }

        }

    }

}
