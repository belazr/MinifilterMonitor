#include "filter.h"

#include "handle.h"
#include "text.h"

#include "..\..\inc\protocol.h"

#include <fltUser.h>

#include <array>
#include <format>
#include <iostream>
#include <string>
#include <vector>

using namespace mimo;

namespace {

    constexpr wchar_t MINIMON_NAME[] = L"MiniMonFlt";

    HRESULT EnableLoadDriverPrivilege() {
        NullHandle token{};

        if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, token.Put())) {

            return HRESULT_FROM_WIN32(GetLastError());
        }

        LUID luid{};

        if (!LookupPrivilegeValueW(nullptr, SE_LOAD_DRIVER_NAME, &luid)) {

            return HRESULT_FROM_WIN32(GetLastError());
        }

        TOKEN_PRIVILEGES privileges{};
        privileges.PrivilegeCount = 1u;
        privileges.Privileges[0].Luid = luid;
        privileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

        if (!AdjustTokenPrivileges(token.Get(), FALSE, &privileges, static_cast<DWORD>(sizeof(privileges)), nullptr, nullptr)) {

            return HRESULT_FROM_WIN32(GetLastError());
        }

        // AdjustTokenPrivileges returns TRUE even when it enabled nothing
        // with a single privilege, ERROR_NOT_ALL_ASSIGNED means that privilege isn't held (e.g. not elevated)
        if (GetLastError() == ERROR_NOT_ALL_ASSIGNED) {

            return HRESULT_FROM_WIN32(ERROR_NOT_ALL_ASSIGNED);
        }

        return S_OK;
    }


    HRESULT Load() {
        const HRESULT hPrivRes = EnableLoadDriverPrivilege();

        if (FAILED(hPrivRes)) return hPrivRes;

        const HRESULT hLoadRes = FilterLoad(MINIMON_NAME);

        // treat an already-loaded driver as success
        if (hLoadRes == HRESULT_FROM_WIN32(ERROR_ALREADY_EXISTS)) return S_OK;

        if (hLoadRes == HRESULT_FROM_WIN32(ERROR_SERVICE_ALREADY_RUNNING)) return S_OK;

        return hLoadRes;
    }


    std::vector<std::wstring> EnumerateSubKeys(const std::wstring& path) {
        std::vector<std::wstring> names;
        RegKeyHandle key{};

        if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, path.c_str(), 0u, KEY_READ, reinterpret_cast<HKEY*>(key.Put())) != ERROR_SUCCESS) return names;

        for (DWORD i = 0u; ; i++) {
            std::array<wchar_t, 256u> buffer{};
            DWORD len = static_cast<DWORD>(buffer.size());
            const LSTATUS status = RegEnumKeyExW(static_cast<HKEY>(key.Get()), i, buffer.data(), &len, nullptr, nullptr, nullptr, nullptr);

            if (status == ERROR_MORE_DATA) continue;

            if (status != ERROR_SUCCESS) break;

            names.emplace_back(buffer.data(), len);
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

}

namespace mimo {

    namespace filter {

        HRESULT Attach(const std::wstring& volumeName) {
            const HRESULT hLoadRes = Load();

            if (FAILED(hLoadRes)) return hLoadRes;

            return FilterAttach(MINIMON_NAME, volumeName.c_str(), nullptr, 0u, nullptr);
        }


        HRESULT AttachAll(const std::wstring& volumeName) {
            const HRESULT hLoadRes = Load();

            if (FAILED(hLoadRes)) return hLoadRes;

            const std::vector<std::wstring> names = GetInstalledInstanceNames();

            if (names.empty()) return HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);

            HRESULT hLastRes = S_OK;

            for (const std::wstring& name : names) {
                const HRESULT hRes = FilterAttach(MINIMON_NAME, volumeName.c_str(), name.c_str(), 0u, nullptr);

                if (SUCCEEDED(hRes)) continue;

                if (hRes == ERROR_FLT_INSTANCE_NAME_COLLISION) continue;

                if (hRes == ERROR_FLT_INSTANCE_ALTITUDE_COLLISION) continue;

                std::cerr << std::format("  Failed to attach instance: {} 0x{:08X}\n", text::ConvertToUtf8(name), static_cast<ULONG>(hRes));
                hLastRes = hRes;
            }

            return hLastRes;
        }


        HRESULT Unload() {
            const HRESULT hPrivRes = EnableLoadDriverPrivilege();

            if (FAILED(hPrivRes)) return hPrivRes;

            const HRESULT hUnloadRes = FilterUnload(MINIMON_NAME);

            if (hUnloadRes == ERROR_FLT_FILTER_NOT_FOUND) return S_OK;

            return hUnloadRes;
        }


        HRESULT Connect(InvHandle& port) {

            return FilterConnectCommunicationPort(PORT_NAME, 0u, nullptr, 0u, nullptr, port.Put());
        }


        HRESULT GetRecords(const InvHandle& port, AlignedBuffer& records) {
            protocol::CommandMessage cmdMsg{};
            cmdMsg.command = protocol::Command::GetRecords;
            DWORD bytesRet = 0u;

            const HRESULT hRes = FilterSendMessage(port.Get(), &cmdMsg, sizeof(cmdMsg), records.Data(), records.Size(), &bytesRet);

            // empty list on an idle volume: STATUS_NO_MORE_ENTRIES surfaces as ERROR_NO_MORE_ITEMS
            // report zero records and keep polling
            if (hRes == HRESULT_FROM_WIN32(ERROR_NO_MORE_ITEMS)) {
                records.Resize(0u);

                return S_OK;
            }

            if (SUCCEEDED(hRes)) {
                records.Resize(bytesRet);
            }

            return hRes;
        }

    }

}
