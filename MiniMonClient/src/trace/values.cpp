#include "values.h"

#include "kernel.h"

#include <Windows.h>
#include <sddl.h>

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <format>
#include <map>
#include <optional>
#include <span>
#include <string>
#include <utility>

namespace {

    std::optional<SYSTEMTIME> MakeLocalTime(int64_t time) {

        if (time < 0) return std::nullopt;

        const uint64_t ticks = static_cast<uint64_t>(time);

        FILETIME utcFileTime{};
        utcFileTime.dwLowDateTime = static_cast<DWORD>(ticks);
        utcFileTime.dwHighDateTime = static_cast<DWORD>(ticks >> 32);

        FILETIME localFileTime{};

        if (!FileTimeToLocalFileTime(&utcFileTime, &localFileTime)) return std::nullopt;

        SYSTEMTIME localTime{};

        if (!FileTimeToSystemTime(&localFileTime, &localTime)) return std::nullopt;

        return localTime;
    }


    constexpr uint32_t ACCOUNT_NAME_WCHAR_COUNT = 256u;

    std::map<std::wstring, std::wstring> accountNames;

}

namespace mimo {

    namespace trace {

        namespace values {

            std::wstring RenderBoolean(bool value) {

                return value ? L"True" : L"False";
            }


            std::wstring RenderByteOffset(int64_t byteOffset) {

                if (byteOffset == kernel::FILE_WRITE_TO_END_OF_FILE) return L"EOF";

                if (byteOffset == kernel::FILE_USE_FILE_POINTER_POSITION) return L"Current";

                return std::to_wstring(byteOffset);
            }


            std::wstring RenderObjectId(uint64_t objectId) {

                if (objectId == 0u) return L"";

                return std::format(L"{:016X}", objectId);
            }


            std::wstring RenderTopLevelIrp(uint64_t topLevelIrp) {

                switch (topLevelIrp) {
                    case kernel::FSRTL_FSP_TOP_LEVEL_IRP:               return L"FSP";
                    case kernel::FSRTL_CACHE_TOP_LEVEL_IRP:             return L"CACHE";
                    case kernel::FSRTL_MOD_WRITE_TOP_LEVEL_IRP:         return L"MOD_WRITE";
                    case kernel::FSRTL_FAST_IO_TOP_LEVEL_IRP:           return L"FAST_IO";
                    case kernel::FSRTL_NETWORK1_TOP_LEVEL_IRP:          return L"NETWORK1";
                    case kernel::FSRTL_NETWORK2_TOP_LEVEL_IRP:          return L"NETWORK2";
                    case kernel::FSRTL_ASYNC_CACHED_READ_TOP_LEVEL_IRP: return L"ASYNC_CACHED_READ";
                    case kernel::FSRTL_VOLSNAP_TOP_LEVEL_IRP:           return L"VOLSNAP";
                }

                return RenderObjectId(topLevelIrp);
            }


            std::wstring RenderFileId(std::span<const uint8_t, 16u> fileId) {
                uint64_t low;
                uint64_t high;

                std::memcpy(&low, fileId.data(), sizeof(low));
                std::memcpy(&high, fileId.data() + sizeof(low), sizeof(high));

                if (!high) return std::format(L"0x{:X}", low);

                return std::format(L"0x{:X}{:016X}", high, low);
            }


            std::wstring RenderGuid(std::span<const uint8_t, 16u> guid) {
                uint32_t data1 = 0u;
                std::memcpy(&data1, guid.data(), sizeof(data1));

                uint16_t data2 = 0u;
                std::memcpy(&data2, guid.data() + 4u, sizeof(data2));

                uint16_t data3 = 0u;
                std::memcpy(&data3, guid.data() + 6u, sizeof(data3));

                return std::format(L"{:08X}-{:04X}-{:04X}-{:02X}{:02X}-{:02X}{:02X}{:02X}{:02X}{:02X}{:02X}", data1, data2, data3, guid[8u], guid[9u], guid[10u], guid[11u], guid[12u], guid[13u], guid[14u], guid[15u]);
            }


            std::wstring RenderOperationTime(int64_t operationTime) {
                constexpr uint64_t TICKS_PER_SECOND = 10000000u;
                const std::optional<SYSTEMTIME> localTime = MakeLocalTime(operationTime);

                if (!localTime.has_value()) return L"[invalid time]";

                const uint64_t ticks = static_cast<uint64_t>(operationTime);
                const uint32_t subSecond = static_cast<uint32_t>(ticks % TICKS_PER_SECOND);

                return std::format(L"{:02}:{:02}:{:02}.{:07}", localTime->wHour, localTime->wMinute, localTime->wSecond, subSecond);
            }


            std::wstring RenderTime(int64_t time) {
                const std::optional<SYSTEMTIME> localTime = MakeLocalTime(time);

                if (!localTime.has_value()) return L"[invalid time]";

                return std::format(L"{:04}-{:02}-{:02} {:02}:{:02}:{:02}", localTime->wYear, localTime->wMonth, localTime->wDay, localTime->wHour, localTime->wMinute, localTime->wSecond);
            }


            std::wstring RenderSid(std::span<const uint8_t> sidData) {

                if (sidData.size() < offsetof(SID, SubAuthority)) return L"[invalid sid]";

                const PSID pSid = const_cast<uint8_t*>(sidData.data());

                if (!IsValidSid(pSid) || GetSidLengthRequired(*GetSidSubAuthorityCount(pSid)) > sidData.size()) return L"[invalid sid]";

                wchar_t* pSidText = nullptr;

                if (!ConvertSidToStringSidW(pSid, &pSidText)) return L"[invalid sid]";

                std::wstring sidText = pSidText;
                LocalFree(pSidText);

                const auto [it, inserted] = accountNames.try_emplace(std::move(sidText));

                if (!inserted) return it->second;

                wchar_t name[ACCOUNT_NAME_WCHAR_COUNT]{};
                wchar_t domain[ACCOUNT_NAME_WCHAR_COUNT]{};
                DWORD nameWcharCount = static_cast<DWORD>(ACCOUNT_NAME_WCHAR_COUNT);
                DWORD domainWcharCount = static_cast<DWORD>(ACCOUNT_NAME_WCHAR_COUNT);
                SID_NAME_USE use{};

                if (LookupAccountSidW(nullptr, pSid, name, &nameWcharCount, domain, &domainWcharCount, &use)) {
                    it->second = *domain ? std::format(L"{}\\{}", domain, name) : name;
                }
                else {
                    it->second = it->first;
                }

                return it->second;
            }

        }

    }

}
