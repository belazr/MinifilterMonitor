#include <initguid.h>

#include "ecp.h"

#include "driver.h"

#include <fltKernel.h>
#include <ntstrsafe.h>
#include <wsk.h>

#include <stdarg.h>

using namespace mimo;

namespace {

    struct EcpWriter {
        WCHAR* pCursor;
        size_t bytesLeft;
        bool truncated;
    };

    __declspec(code_seg("PAGE"))
    void AppendText(_Inout_ EcpWriter* pWriter, _In_ _Printf_format_string_ PCWSTR pFormat, ...) {
        PAGED_CODE();

        va_list args;
        va_start(args, pFormat);

        const NTSTATUS status = RtlStringCbVPrintfExW(pWriter->pCursor, pWriter->bytesLeft, &pWriter->pCursor, &pWriter->bytesLeft, 0u, pFormat, args);

        va_end(args);

        if (status == STATUS_BUFFER_OVERFLOW) {
            pWriter->truncated = true;
        }

        return;
    }


    __declspec(code_seg("PAGE"))
    void AppendGuid(
        _Inout_ EcpWriter* pWriter,
        _In_ PCWSTR pLabel,
        _In_ const GUID& guid
    ) {
        PAGED_CODE();

        UNICODE_STRING text{};

        if (NT_SUCCESS(RtlStringFromGUID(guid, &text))) {
            AppendText(pWriter, L"%s=%wZ ", pLabel, &text);
            RtlFreeUnicodeString(&text);
        }
        else {
            AppendText(pWriter, L"%s=<unprintable> ", pLabel);
        }

        return;
    }


    __declspec(code_seg("PAGE"))
    void AppendOplockKey(_Inout_ EcpWriter* pWriter, _In_ const OPLOCK_KEY_ECP_CONTEXT* pContext) {
        PAGED_CODE();

        AppendText(pWriter, L"OPLOCK: ");
        AppendGuid(pWriter, L"key", pContext->OplockKey);

        return;
    }


    __declspec(code_seg("PAGE"))
    void AppendDualOplockKey(_Inout_ EcpWriter* pWriter, _In_ const DUAL_OPLOCK_KEY_ECP_CONTEXT* pContext) {
        PAGED_CODE();

        AppendText(pWriter, L"DUAL-OPLOCK: ");

        if (pContext->ParentOplockKeySet) {
            AppendGuid(pWriter, L"parent", pContext->ParentOplockKey);
        }

        if (pContext->TargetOplockKeySet) {
            AppendGuid(pWriter, L"target", pContext->TargetOplockKey);
        }

        return;
    }


    __declspec(code_seg("PAGE"))
    _Success_(return)
    bool FormatEndpoint(_In_ PSOCKADDR_STORAGE_NFS pAddr, _Out_writes_z_(INET6_ADDRSTRLEN) CHAR* pText) {
        PAGED_CODE();

        ULONG textChars = INET6_ADDRSTRLEN;
        NTSTATUS status = STATUS_INVALID_PARAMETER;

        if (pAddr->ss_family == AF_INET) {
            const SOCKADDR_IN* const pIpv4 = reinterpret_cast<const SOCKADDR_IN*>(pAddr);
            status = RtlIpv4AddressToStringEx(&pIpv4->sin_addr, pIpv4->sin_port, pText, &textChars);
        }
        else if (pAddr->ss_family == AF_INET6) {
            const SOCKADDR_IN6* const pIpv6 = reinterpret_cast<const SOCKADDR_IN6*>(pAddr);
            status = RtlIpv6AddressToStringEx(&pIpv6->sin6_addr, 0u, pIpv6->sin6_port, pText, &textChars);
        }

        return NT_SUCCESS(status);
    }


    __declspec(code_seg("PAGE"))
    void AppendNfsOpen(_Inout_ EcpWriter* pWriter, _In_ const NFS_OPEN_ECP_CONTEXT* pContext) {
        PAGED_CODE();

        CHAR endpoint[INET6_ADDRSTRLEN]{};

        AppendText(pWriter, L"NFS: ");

        if (pContext->ExportAlias) {
            AppendText(pWriter, L"share=%wZ ", pContext->ExportAlias);
        }

        if (pContext->ClientSocketAddress && FormatEndpoint(pContext->ClientSocketAddress, endpoint)) {
            AppendText(pWriter, L"client=%S ", endpoint);
        }

        return;
    }


    __declspec(code_seg("PAGE"))
    void AppendOplockStates(_Inout_ EcpWriter* pWriter, _In_ const SRV_OPEN_ECP_CONTEXT* pContext) {
        PAGED_CODE();

        PCWSTR states[3u]{};
        ULONG count = 0u;

        if (pContext->OplockBlockState) states[count++] = L"block";

        if (pContext->OplockAppState) states[count++] = L"app";

        if (pContext->OplockFinalState) states[count++] = L"final";

        AppendText(pWriter, L"state=");

        if (!count) {
            AppendText(pWriter, L"none");
        }

        for (ULONG i = 0u; i < count; i++) {
            AppendText(pWriter, i ? L"|%s" : L"%s", states[i]);
        }

        AppendText(pWriter, L" ");

        return;
    }


    __declspec(code_seg("PAGE"))
    void AppendSrvOpen(_Inout_ EcpWriter* pWriter, _In_ const SRV_OPEN_ECP_CONTEXT* pContext) {
        PAGED_CODE();

        CHAR endpoint[INET6_ADDRSTRLEN]{};

        AppendText(pWriter, L"SRV: ");

        if (pContext->ShareName) {
            AppendText(pWriter, L"share=%wZ ", pContext->ShareName);
        }

        if (pContext->SocketAddress && FormatEndpoint(pContext->SocketAddress, endpoint)) {
            AppendText(pWriter, L"client=%S ", endpoint);
        }

        AppendOplockStates(pWriter, pContext);

        return;
    }


    __declspec(code_seg("PAGE"))
    bool AppendEcp(
        _Inout_ EcpWriter* pWriter,
        _In_ const GUID& guid,
        _In_ void* pContext
    ) {
        PAGED_CODE();

        if (IsEqualGUID(GUID_ECP_OPLOCK_KEY, guid)) {
            AppendOplockKey(pWriter, reinterpret_cast<const OPLOCK_KEY_ECP_CONTEXT*>(pContext));
        }
        else if (IsEqualGUID(GUID_ECP_DUAL_OPLOCK_KEY, guid)) {
            AppendDualOplockKey(pWriter, reinterpret_cast<const DUAL_OPLOCK_KEY_ECP_CONTEXT*>(pContext));
        }
        else if (IsEqualGUID(GUID_ECP_NFS_OPEN, guid)) {
            AppendNfsOpen(pWriter, reinterpret_cast<const NFS_OPEN_ECP_CONTEXT*>(pContext));
        }
        else if (IsEqualGUID(GUID_ECP_SRV_OPEN, guid)) {
            AppendSrvOpen(pWriter, reinterpret_cast<const SRV_OPEN_ECP_CONTEXT*>(pContext));
        }
        else if (IsEqualGUID(GUID_ECP_PREFETCH_OPEN, guid)) {
            AppendText(pWriter, L"PREFETCH ");
        }
        else {

            return false;
        }

        return true;
    }

}

namespace mimo {

    namespace ecp {

        __declspec(code_seg("PAGE"))
        _Use_decl_annotations_
        NTSTATUS FormatEcps(FLT_CALLBACK_DATA* pData, UNICODE_STRING* pEcpString) {
            PAGED_CODE();

            ECP_LIST* pEcpList = nullptr;

            if (!NT_SUCCESS(FltGetEcpListFromCallbackData(driver::Filter, pData, &pEcpList)) || !pEcpList) return STATUS_SUCCESS;

            EcpWriter writer{ pEcpString->Buffer, pEcpString->MaximumLength };
            ULONG total = 0u;
            ULONG recognized = 0u;
            void* pContext = nullptr;
            GUID guid{};
            ULONG contextSize = 0u;

            while (NT_SUCCESS(FltGetNextExtraCreateParameter(driver::Filter, pEcpList, pContext, &guid, &pContext, &contextSize))) {
                total++;

                if (FltIsEcpFromUserMode(driver::Filter, pContext)) continue;

                if (AppendEcp(&writer, guid, pContext)) recognized++;

            }

            if (recognized < total) {
                AppendText(&writer, L"unknown=%lu ", total - recognized);
            }

            pEcpString->Length = static_cast<USHORT>(pEcpString->MaximumLength - writer.bytesLeft);

            if (pEcpString->Length >= sizeof(WCHAR) && pEcpString->Buffer[pEcpString->Length / sizeof(WCHAR) - 1u] == L' ') {
                pEcpString->Length -= sizeof(WCHAR);
                pEcpString->Buffer[pEcpString->Length / sizeof(WCHAR)] = L'\0';
            }

            return writer.truncated ? STATUS_BUFFER_OVERFLOW : STATUS_SUCCESS;
        }

    }

}
