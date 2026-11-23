#include "quota.h"

#include "..\report.h"
#include "..\target.h"

#include "..\..\error.h"

#include <Windows.h>
#undef WIN32_NO_STATUS
#include <ntstatus.h>
#include <sddl.h>
#include <winternl.h>

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <optional>
#include <span>
#include <string_view>
#include <vector>

extern "C" NTSYSAPI NTSTATUS NTAPI NtQueryQuotaInformationFile(
    HANDLE hFile,
    IO_STATUS_BLOCK* pIoStatus,
    void* pBuffer,
    ULONG bufferSize,
    BOOLEAN returnSingleEntry,
    void* pSidList,
    ULONG sidListSize,
    PSID pStartSid,
    BOOLEAN restartScan
);

using namespace mimo;

namespace {

    constexpr test::report::Description QUERY_DESCRIPTIONS[]{
        { L"query-quota", STATUS_SUCCESS },
    };

    struct FILE_GET_QUOTA_INFORMATION {
        ULONG NextEntryOffset;
        ULONG SidLength;
        SID Sid;
    };

    std::optional<std::vector<std::byte>> MakeSidListBuffer() {
        constexpr std::wstring_view SID_TEXT = L"S-1-5-21-1-2-3-1001";
        constexpr size_t SID_OFFSET = offsetof(FILE_GET_QUOTA_INFORMATION, Sid);
        PSID pSid = nullptr;

        if (!ConvertStringSidToSidW(SID_TEXT.data(), &pSid)) {
            const DWORD lastError = GetLastError();
            std::cerr << "Failed to build the SID\n";
            error::Display(lastError);

            return std::nullopt;
        }

        const uint32_t sidSize = static_cast<uint32_t>(GetLengthSid(pSid));
        FILE_GET_QUOTA_INFORMATION header{};
        header.SidLength = static_cast<ULONG>(sidSize);

        std::vector<std::byte> buffer(SID_OFFSET + sidSize);
        std::memcpy(buffer.data(), &header, SID_OFFSET);
        std::memcpy(buffer.data() + SID_OFFSET, pSid, sidSize);
        LocalFree(pSid);

        return buffer;
    }


    NTSTATUS Query(const test::Target& target, std::span<const std::byte> sidListData) {
        constexpr size_t BUFFER_SIZE = 256u;
        std::vector<std::byte> buffer(BUFFER_SIZE);
        std::vector<std::byte> sidList(sidListData.begin(), sidListData.end());
        IO_STATUS_BLOCK ioStatus{};

        return NtQueryQuotaInformationFile(target.Get(), &ioStatus, buffer.data(), static_cast<ULONG>(buffer.size()), TRUE, sidList.data(), static_cast<ULONG>(sidList.size()), nullptr, FALSE);
    }


    constexpr std::string_view QUERY_MAJOR = "Query quota";

    void RunQueryOperations(std::wstring_view ntDirectory, test::report::Counts& counts) {

        for (const test::report::Description& description : QUERY_DESCRIPTIONS) {
            test::Target target{ ntDirectory, description.name };
            const NTSTATUS createStatus = target.Create(FILE_GENERIC_READ);

            if (!NT_SUCCESS(createStatus)) {
                std::cerr << "Failed to create the target file\n";
                error::Display(RtlNtStatusToDosError(createStatus));
                test::report::Add(counts, QUERY_MAJOR, description, std::nullopt);

                continue;
            }

            const std::optional<std::vector<std::byte>> sidListData = MakeSidListBuffer();

            if (!sidListData.has_value()) {
                test::report::Add(counts, QUERY_MAJOR, description, std::nullopt);

                continue;
            }

            const NTSTATUS queryStatus = Query(target, *sidListData);
            test::report::Add(counts, QUERY_MAJOR, description, queryStatus);
        }

        return;
    }

}

namespace mimo {

    namespace test {

        namespace operations {

            namespace quota {

                bool Run(std::wstring_view ntDirectory, report::Counts& counts) {
                    RunQueryOperations(ntDirectory, counts);

                    return true;
                }

            }

        }

    }

}
