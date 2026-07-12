#include <initguid.h>

#include "ecp.h"

#include "driver.h"

#include <fltKernel.h>
#include <ntstrsafe.h>
#include <wsk.h>

using namespace mimo;

namespace {

    constexpr ULONG ENDPOINT_TEXT_CHARS = 64u;

    struct EcpWriter {
        WCHAR* pCursor;
        size_t bytesLeft;
    };

    __declspec(code_seg("PAGE"))
    _Success_(return)
    bool FormatEndpoint(_In_ PSOCKADDR_STORAGE_NFS pAddr, _Out_writes_z_(ENDPOINT_TEXT_CHARS) CHAR* pText) {
        PAGED_CODE();

        ULONG textChars = ENDPOINT_TEXT_CHARS;
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
    void AppendGuid(_Inout_ EcpWriter* pWriter, _In_ PCWSTR pLabel, _In_ const GUID& guid) {
        PAGED_CODE();

        UNICODE_STRING text{};

        if (NT_SUCCESS(RtlStringFromGUID(guid, &text))) {
            RtlStringCbPrintfExW(pWriter->pCursor, pWriter->bytesLeft, &pWriter->pCursor, &pWriter->bytesLeft, 0u, L"%s=%wZ ", pLabel, &text);
            RtlFreeUnicodeString(&text);
        }
        else {
            RtlStringCbPrintfExW(pWriter->pCursor, pWriter->bytesLeft, &pWriter->pCursor, &pWriter->bytesLeft, 0u, L"%s=<unprintable> ", pLabel);
        }

        return;
    }


    __declspec(code_seg("PAGE"))
    void AppendOplockKey(_Inout_ EcpWriter* pWriter, _In_ const OPLOCK_KEY_ECP_CONTEXT* pContext) {
        PAGED_CODE();

        RtlStringCbPrintfExW(pWriter->pCursor, pWriter->bytesLeft, &pWriter->pCursor, &pWriter->bytesLeft, 0u, L"OPLOCK: ");

        AppendGuid(pWriter, L"key", pContext->OplockKey);

        return;
    }


    __declspec(code_seg("PAGE"))
    void AppendDualOplockKey(_Inout_ EcpWriter* pWriter, _In_ const DUAL_OPLOCK_KEY_ECP_CONTEXT* pContext) {
        PAGED_CODE();

        RtlStringCbPrintfExW(pWriter->pCursor, pWriter->bytesLeft, &pWriter->pCursor, &pWriter->bytesLeft, 0u, L"DUAL-OPLOCK: ");

        if (pContext->ParentOplockKeySet) {
            AppendGuid(pWriter, L"parent", pContext->ParentOplockKey);
        }

        if (pContext->TargetOplockKeySet) {
            AppendGuid(pWriter, L"target", pContext->TargetOplockKey);
        }

        return;
    }


    __declspec(code_seg("PAGE"))
    void AppendNfsOpen(_Inout_ EcpWriter* pWriter, _In_ const NFS_OPEN_ECP_CONTEXT* pContext) {
        PAGED_CODE();

        CHAR endpoint[ENDPOINT_TEXT_CHARS]{};

        RtlStringCbPrintfExW(pWriter->pCursor, pWriter->bytesLeft, &pWriter->pCursor, &pWriter->bytesLeft, 0u, L"NFS: ");

        if (pContext->ExportAlias) {
            RtlStringCbPrintfExW(pWriter->pCursor, pWriter->bytesLeft, &pWriter->pCursor, &pWriter->bytesLeft, 0u, L"share=%wZ ", pContext->ExportAlias);
        }

        if (pContext->ClientSocketAddress && FormatEndpoint(pContext->ClientSocketAddress, endpoint)) {
            RtlStringCbPrintfExW(pWriter->pCursor, pWriter->bytesLeft, &pWriter->pCursor, &pWriter->bytesLeft, 0u, L"client=%S ", endpoint);
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

        RtlStringCbPrintfExW(pWriter->pCursor, pWriter->bytesLeft, &pWriter->pCursor, &pWriter->bytesLeft, 0u, L"state=");

        if (!count) {
            RtlStringCbPrintfExW(pWriter->pCursor, pWriter->bytesLeft, &pWriter->pCursor, &pWriter->bytesLeft, 0u, L"none");
        }

        for (ULONG i = 0u; i < count; i++) {
            RtlStringCbPrintfExW(pWriter->pCursor, pWriter->bytesLeft, &pWriter->pCursor, &pWriter->bytesLeft, 0u, i ? L"|%s" : L"%s", states[i]);
        }

        RtlStringCbPrintfExW(pWriter->pCursor, pWriter->bytesLeft, &pWriter->pCursor, &pWriter->bytesLeft, 0u, L" ");

        return;
    }


    __declspec(code_seg("PAGE"))
    void AppendSrvOpen(_Inout_ EcpWriter* pWriter, _In_ const SRV_OPEN_ECP_CONTEXT* pContext) {
        PAGED_CODE();

        CHAR endpoint[ENDPOINT_TEXT_CHARS]{};

        RtlStringCbPrintfExW(pWriter->pCursor, pWriter->bytesLeft, &pWriter->pCursor, &pWriter->bytesLeft, 0u, L"SRV: ");

        if (pContext->ShareName) {
            RtlStringCbPrintfExW(pWriter->pCursor, pWriter->bytesLeft, &pWriter->pCursor, &pWriter->bytesLeft, 0u, L"share=%wZ ", pContext->ShareName);
        }

        if (pContext->SocketAddress && FormatEndpoint(pContext->SocketAddress, endpoint)) {
            RtlStringCbPrintfExW(pWriter->pCursor, pWriter->bytesLeft, &pWriter->pCursor, &pWriter->bytesLeft, 0u, L"client=%S ", endpoint);
        }

        AppendOplockStates(pWriter, pContext);

        return;
    }


    __declspec(code_seg("PAGE"))
    bool AppendEcp(_Inout_ EcpWriter* pWriter, _In_ const GUID& guid, _In_ void* pContext) {
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
            RtlStringCbPrintfExW(pWriter->pCursor, pWriter->bytesLeft, &pWriter->pCursor, &pWriter->bytesLeft, 0u, L"PREFETCH ");
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
        void FormatEcps(FLT_CALLBACK_DATA* pData, UNICODE_STRING* pEcpString) {
            PAGED_CODE();

            ECP_LIST* pEcpList = nullptr;

            if (!NT_SUCCESS(FltGetEcpListFromCallbackData(driver::Filter, pData, &pEcpList)) || !pEcpList) return;

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
                RtlStringCbPrintfExW(writer.pCursor, writer.bytesLeft, &writer.pCursor, &writer.bytesLeft, 0u, L"unknown=%lu ", total - recognized);
            }

            pEcpString->Length = static_cast<USHORT>(pEcpString->MaximumLength - writer.bytesLeft);

            if (pEcpString->Length >= sizeof(WCHAR) && pEcpString->Buffer[pEcpString->Length / sizeof(WCHAR) - 1u] == L' ') {
                pEcpString->Length -= sizeof(WCHAR);
                pEcpString->Buffer[pEcpString->Length / sizeof(WCHAR)] = L'\0';
            }

            return;
        }

    }

}
