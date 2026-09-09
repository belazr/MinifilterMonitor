#include "values.h"

#include "kernel.h"

#include <Windows.h>
#include <sddl.h>

#include <cstddef>
#include <cstdint>
#include <format>
#include <map>
#include <span>
#include <string>
#include <utility>

namespace {

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


            std::wstring RenderTopLevelIrp(uint64_t topLevelIrp) {

                if (topLevelIrp == 0u) return L"";

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

                return std::format(L"{:0{}X}", topLevelIrp, static_cast<int>(sizeof(topLevelIrp) * 2u));
            }


            std::wstring RenderFileTime(int64_t fileTime) {

                if (fileTime < 0) return std::to_wstring(fileTime);

                const uint64_t ticks = static_cast<uint64_t>(fileTime);

                FILETIME systemFileTime{};
                systemFileTime.dwLowDateTime = static_cast<DWORD>(ticks);
                systemFileTime.dwHighDateTime = static_cast<DWORD>(ticks >> 32);

                FILETIME localFileTime{};

                if (!FileTimeToLocalFileTime(&systemFileTime, &localFileTime)) return L"TIME ERROR";

                SYSTEMTIME localTime{};

                if (!FileTimeToSystemTime(&localFileTime, &localTime)) return L"TIME ERROR";

                return std::format(L"{:04}-{:02}-{:02} {:02}:{:02}:{:02}", localTime.wYear, localTime.wMonth, localTime.wDay, localTime.wHour, localTime.wMinute, localTime.wSecond);
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
