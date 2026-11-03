#include "filter.h"

#include "handle.h"
#include "text.h"

#include "..\..\inc\protocol.h"

#include <Windows.h>
#include <fltUser.h>

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <format>
#include <iostream>
#include <string>
#include <vector>

using namespace mimo;

namespace {

    constexpr wchar_t MINIMON_NAME[] = L"MiniMonFlt";

    HRESULT EnableLoadDriverPrivilege() {
        NullHandle token;

        if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, token.Put())) return HRESULT_FROM_WIN32(GetLastError());

        LUID luid{};

        if (!LookupPrivilegeValueW(nullptr, SE_LOAD_DRIVER_NAME, &luid)) return HRESULT_FROM_WIN32(GetLastError());

        TOKEN_PRIVILEGES privileges{};
        privileges.PrivilegeCount = 1u;
        privileges.Privileges[0u].Luid = luid;
        privileges.Privileges[0u].Attributes = SE_PRIVILEGE_ENABLED;

        if (!AdjustTokenPrivileges(token.Get(), FALSE, &privileges, static_cast<DWORD>(sizeof(privileges)), nullptr, nullptr)) return HRESULT_FROM_WIN32(GetLastError());

        // AdjustTokenPrivileges returns TRUE even when it enabled nothing
        // with a single privilege, ERROR_NOT_ALL_ASSIGNED means that privilege isn't held (e.g. not elevated)
        if (GetLastError() == ERROR_NOT_ALL_ASSIGNED) return HRESULT_FROM_WIN32(ERROR_NOT_ALL_ASSIGNED);

        return S_OK;
    }


    std::vector<std::wstring> EnumerateSubKeys(const std::wstring& path) {
        std::vector<std::wstring> names;
        RegistryKeyHandle key;

        if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, path.c_str(), 0u, KEY_READ, key.Put()) != ERROR_SUCCESS) return names;

        for (DWORD i = 0u; ; i++) {
            constexpr uint32_t KEY_NAME_WCHAR_COUNT = 256u;
            std::array<wchar_t, KEY_NAME_WCHAR_COUNT> buffer{};
            DWORD wcharCount = static_cast<DWORD>(buffer.size());
            const LSTATUS status = RegEnumKeyExW(key.Get(), i, buffer.data(), &wcharCount, nullptr, nullptr, nullptr, nullptr);

            if (status == ERROR_MORE_DATA) continue;

            if (status != ERROR_SUCCESS) break;

            names.emplace_back(buffer.data(), static_cast<size_t>(wcharCount));
        }

        return names;
    }


    std::vector<std::wstring> GetInstalledInstanceNames() {
        const std::wstring servicePath = std::format(L"SYSTEM\\CurrentControlSet\\Services\\{}", MINIMON_NAME);

        // modern installs register the instances under Parameters\Instances, downlevel ones under Instances
        std::vector<std::wstring> names = EnumerateSubKeys(servicePath + L"\\Parameters\\Instances");

        if (names.empty()) {
            names = EnumerateSubKeys(servicePath + L"\\Instances");
        }

        return names;
    }


    HRESULT AttachInstances(const std::wstring& volumeName, const std::vector<std::wstring>& names) {
        HRESULT hLastRes = S_OK;

        for (const std::wstring& name : names) {
            const HRESULT hRes = FilterAttach(MINIMON_NAME, volumeName.c_str(), name.c_str(), 0u, nullptr);

            if (SUCCEEDED(hRes)) continue;

            if (hRes == ERROR_FLT_INSTANCE_NAME_COLLISION) continue;

            std::cerr << std::format("Failed to attach instance {} to {}: 0x{:08X}\n", text::ConvertToUtf8(name), text::ConvertToUtf8(volumeName), static_cast<uint32_t>(hRes));
            hLastRes = hRes;
        }

        return hLastRes;
    }


    constexpr bool IsTracedFileSystem(FLT_FILESYSTEM_TYPE type) {

        return type != FLT_FSTYPE_RAW && type != FLT_FSTYPE_NPFS && type != FLT_FSTYPE_MSFS;
    }


    using VolumeFindHandle = Handle<FilterVolumeFindClose, HANDLE, INVALID_HANDLE_VALUE>;

}

namespace mimo {

    namespace filter {

        HRESULT Load() {
            const HRESULT hPrivilegeRes = EnableLoadDriverPrivilege();

            if (FAILED(hPrivilegeRes)) return hPrivilegeRes;

            const HRESULT hLoadRes = FilterLoad(MINIMON_NAME);

            // treat an already-loaded driver as success
            if (hLoadRes == HRESULT_FROM_WIN32(ERROR_ALREADY_EXISTS)) return S_OK;

            if (hLoadRes == HRESULT_FROM_WIN32(ERROR_SERVICE_ALREADY_RUNNING)) return S_OK;

            return hLoadRes;
        }


        HRESULT Attach(const std::wstring& volumeName) {
            const std::vector<std::wstring> names = GetInstalledInstanceNames();

            if (names.empty()) return HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);

            return AttachInstances(volumeName, names);
        }


        HRESULT AttachAll() {
            constexpr DWORD ENTRY_SIZE = 1024u;
            constexpr size_t NAME_OFFSET = offsetof(FILTER_VOLUME_STANDARD_INFORMATION, FilterVolumeName);
            const std::vector<std::wstring> names = GetInstalledInstanceNames();

            if (names.empty()) return HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);

            std::array<uint8_t, ENTRY_SIZE> entry{};
            DWORD entrySize = 0u;
            VolumeFindHandle volumeFind;
            HRESULT hFindRes = FilterVolumeFindFirst(FilterVolumeStandardInformation, entry.data(), ENTRY_SIZE, &entrySize, volumeFind.Put());
            uint32_t attachedVolumeCount = 0u;

            while (SUCCEEDED(hFindRes)) {
                FILTER_VOLUME_STANDARD_INFORMATION header{};
                std::memcpy(&header, entry.data(), sizeof(header));

                if (IsTracedFileSystem(header.FileSystemType) && !(header.Flags & FLTFL_VSI_DETACHED_VOLUME)) {
                    std::wstring volumeName(header.FilterVolumeNameLength / sizeof(wchar_t), L'\0');
                    std::memcpy(volumeName.data(), entry.data() + NAME_OFFSET, volumeName.size() * sizeof(wchar_t));

                    if (SUCCEEDED(AttachInstances(volumeName, names))) {
                        std::cout << std::format("Attached to {}\n", text::ConvertToUtf8(volumeName));
                        attachedVolumeCount++;
                    }

                }

                hFindRes = FilterVolumeFindNext(volumeFind.Get(), FilterVolumeStandardInformation, entry.data(), ENTRY_SIZE, &entrySize);
            }

            if (hFindRes != HRESULT_FROM_WIN32(ERROR_NO_MORE_ITEMS)) return hFindRes;

            if (!attachedVolumeCount) return ERROR_FLT_VOLUME_NOT_FOUND;

            return S_OK;
        }


        HRESULT Unload() {
            const HRESULT hPrivilegeRes = EnableLoadDriverPrivilege();

            if (FAILED(hPrivilegeRes)) return hPrivilegeRes;

            const HRESULT hUnloadRes = FilterUnload(MINIMON_NAME);

            if (hUnloadRes == ERROR_FLT_FILTER_NOT_FOUND) return S_OK;

            return hUnloadRes;
        }


        HRESULT Connect(InvalidHandle& port) {

            return FilterConnectCommunicationPort(PORT_NAME, 0u, nullptr, 0u, nullptr, port.Put());
        }


        HRESULT GetVersion(const InvalidHandle& port, protocol::Version& version) {
            protocol::CommandMessage commandMessage{};
            commandMessage.command = protocol::Command::GetVersion;
            DWORD bytesReturned = 0u;

            return FilterSendMessage(port.Get(), &commandMessage, static_cast<DWORD>(sizeof(commandMessage)), &version, static_cast<DWORD>(sizeof(version)), &bytesReturned);
        }


        HRESULT GetRecords(const InvalidHandle& port, std::vector<protocol::Record>& records) {
            protocol::CommandMessage commandMessage{};
            commandMessage.command = protocol::Command::GetRecords;
            DWORD bytesReturned = 0u;
            const HRESULT hRes = FilterSendMessage(port.Get(), &commandMessage, static_cast<DWORD>(sizeof(commandMessage)), records.data(), static_cast<DWORD>(records.size() * sizeof(protocol::Record)), &bytesReturned);

            // empty list on an idle volume: STATUS_NO_MORE_ENTRIES surfaces as ERROR_NO_MORE_ITEMS
            // report zero records and keep polling
            if (hRes == HRESULT_FROM_WIN32(ERROR_NO_MORE_ITEMS)) {
                records.clear();

                return S_OK;
            }

            if (FAILED(hRes)) return hRes;

            if (bytesReturned % sizeof(protocol::Record) != 0u) return HRESULT_FROM_WIN32(ERROR_INVALID_DATA);

            records.resize(bytesReturned / sizeof(protocol::Record));

            return S_OK;
        }

    }

}
