#include "quota.h"

#include "..\report.h"
#include "..\target.h"

#include "..\..\error.h"

#include <Windows.h>
#undef WIN32_NO_STATUS
#include <ntstatus.h>
#include <sddl.h>
#include <winternl.h>

#include <array>
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

extern "C" NTSYSAPI NTSTATUS NTAPI NtSetQuotaInformationFile(
    HANDLE hFile,
    IO_STATUS_BLOCK* pIoStatus,
    void* pBuffer,
    ULONG bufferSize
);

using namespace mimo;

namespace {

    constexpr test::report::Description QUERY_DESCRIPTIONS[]{
        { L"query-quota", STATUS_SUCCESS },
    };

    std::optional<std::vector<std::byte>> MakeSid() {
        constexpr std::wstring_view SID_TEXT = L"S-1-5-21-3664179068-802908425-4097507119-1001";
        PSID pSid = nullptr;

        if (!ConvertStringSidToSidW(SID_TEXT.data(), &pSid)) {
            const DWORD lastError = GetLastError();
            std::cerr << "Failed to build the SID\n";
            error::Display(lastError);

            return std::nullopt;
        }

        std::vector<std::byte> sid(static_cast<size_t>(GetLengthSid(pSid)));
        std::memcpy(sid.data(), pSid, sid.size());
        LocalFree(pSid);

        return sid;
    }


    struct FILE_GET_QUOTA_INFORMATION {
        ULONG NextEntryOffset;
        ULONG SidLength;
        SID Sid;
    };

    std::optional<std::vector<std::byte>> MakeSidListBuffer() {
        constexpr size_t SID_OFFSET = offsetof(FILE_GET_QUOTA_INFORMATION, Sid);
        const std::optional<std::vector<std::byte>> sid = MakeSid();

        if (!sid.has_value()) return std::nullopt;

        FILE_GET_QUOTA_INFORMATION header{};
        header.SidLength = static_cast<ULONG>(sid->size());

        std::vector<std::byte> buffer(SID_OFFSET + sid->size());
        std::memcpy(buffer.data(), &header, SID_OFFSET);
        std::memcpy(buffer.data() + SID_OFFSET, sid->data(), sid->size());

        return buffer;
    }


    NTSTATUS Query(const test::Target& target, std::span<const std::byte> sidListData) {
        constexpr uint32_t BUFFER_SIZE = 256u;
        std::array<std::byte, BUFFER_SIZE> buffer{};
        std::vector<std::byte> sidListBuffer(sidListData.begin(), sidListData.end());
        IO_STATUS_BLOCK ioStatus{};

        return NtQueryQuotaInformationFile(target.Get(), &ioStatus, buffer.data(), static_cast<ULONG>(buffer.size()), TRUE, sidListBuffer.data(), static_cast<ULONG>(sidListBuffer.size()), nullptr, FALSE);
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


    // STATUS_ACCESS_DENIED is expected because the process runs without SeManageVolumePrivilege
    // this is enough to provoke a trace and not make persistent changes to the machine
    constexpr test::report::Description SET_DESCRIPTIONS[]{
        { L"set-quota", STATUS_ACCESS_DENIED },
    };

    struct FILE_QUOTA_INFORMATION {
        ULONG NextEntryOffset;
        ULONG SidLength;
        LARGE_INTEGER ChangeTime;
        LARGE_INTEGER QuotaUsed;
        LARGE_INTEGER QuotaThreshold;
        LARGE_INTEGER QuotaLimit;
        SID Sid;
    };

    std::optional<std::vector<std::byte>> MakeQuotaBuffer() {
        // a limit of -2 asks the file system to delete the entry of the SID, so the request cannot leave one behind
        constexpr LONGLONG DELETE_ENTRY = -2;
        constexpr size_t SID_OFFSET = offsetof(FILE_QUOTA_INFORMATION, Sid);
        constexpr size_t ALIGNMENT = alignof(FILE_QUOTA_INFORMATION);
        const std::optional<std::vector<std::byte>> sid = MakeSid();

        if (!sid.has_value()) return std::nullopt;

        FILE_QUOTA_INFORMATION header{};
        header.SidLength = static_cast<ULONG>(sid->size());
        header.QuotaLimit.QuadPart = DELETE_ENTRY;

        std::vector<std::byte> buffer((SID_OFFSET + sid->size() + ALIGNMENT - 1u) / ALIGNMENT * ALIGNMENT);
        std::memcpy(buffer.data(), &header, SID_OFFSET);
        std::memcpy(buffer.data() + SID_OFFSET, sid->data(), sid->size());

        return buffer;
    }


    NTSTATUS Set(const test::Target& target, std::span<const std::byte> quotaData) {
        std::vector<std::byte> buffer(quotaData.begin(), quotaData.end());
        IO_STATUS_BLOCK ioStatus{};

        return NtSetQuotaInformationFile(target.Get(), &ioStatus, buffer.data(), static_cast<ULONG>(buffer.size()));
    }


    constexpr std::string_view SET_MAJOR = "Set quota";

    void RunSetOperations(std::wstring_view ntDirectory, test::report::Counts& counts) {

        for (const test::report::Description& description : SET_DESCRIPTIONS) {
            test::Target target{ ntDirectory, description.name };
            const NTSTATUS createStatus = target.Create(FILE_WRITE_DATA);

            if (!NT_SUCCESS(createStatus)) {
                std::cerr << "Failed to create the target file\n";
                error::Display(RtlNtStatusToDosError(createStatus));
                test::report::Add(counts, SET_MAJOR, description, std::nullopt);

                continue;
            }

            const std::optional<std::vector<std::byte>> quotaData = MakeQuotaBuffer();

            if (!quotaData.has_value()) {
                test::report::Add(counts, SET_MAJOR, description, std::nullopt);

                continue;
            }

            const NTSTATUS setStatus = Set(target, *quotaData);
            test::report::Add(counts, SET_MAJOR, description, setStatus);
        }

        return;
    }

}

namespace mimo {

    namespace test {

        namespace operations {

            namespace quota {

                void Run(std::wstring_view ntDirectory, report::Counts& counts) {
                    RunQueryOperations(ntDirectory, counts);
                    RunSetOperations(ntDirectory, counts);

                    return;
                }

            }

        }

    }

}
