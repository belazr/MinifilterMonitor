#include <initguid.h>

#include "create.h"

#include "..\..\driver.h"

#include "..\..\..\..\inc\protocol.h"

#include <fltKernel.h>
#include <ntstrsafe.h>
#include <wsk.h>

#include <stdarg.h>

using namespace mimo;

static_assert(protocol::CREATE_SID_SIZE == SECURITY_MAX_SID_SIZE, "protocol::CREATE_SID_SIZE mirror drift");

namespace {

    struct EcpWriter {
        WCHAR* pCursor;
        size_t bytesLeft;
        bool truncated;
    };

    __declspec(code_seg("PAGE"))
    void AppendText(_Inout_ EcpWriter* pWriter, _In_z_ _Printf_format_string_ const WCHAR* pFormat, ...) {
        PAGED_CODE();

        va_list args{};
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
        _In_z_ const WCHAR* pLabel,
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
    bool FormatEndpoint(_In_ SOCKADDR_STORAGE* pAddress, _Out_writes_z_(INET6_ADDRSTRLEN) CHAR* pText) {
        PAGED_CODE();

        ULONG charCount = INET6_ADDRSTRLEN;
        NTSTATUS status = STATUS_INVALID_PARAMETER;

        if (pAddress->ss_family == AF_INET) {
            const SOCKADDR_IN* const pIpv4Address = reinterpret_cast<const SOCKADDR_IN*>(pAddress);
            status = RtlIpv4AddressToStringEx(&pIpv4Address->sin_addr, pIpv4Address->sin_port, pText, &charCount);
        }
        else if (pAddress->ss_family == AF_INET6) {
            const SOCKADDR_IN6* const pIpv6Address = reinterpret_cast<const SOCKADDR_IN6*>(pAddress);
            status = RtlIpv6AddressToStringEx(&pIpv6Address->sin6_addr, 0u, pIpv6Address->sin6_port, pText, &charCount);
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

        constexpr ULONG STATE_COUNT = 3u;
        const WCHAR* states[STATE_COUNT]{};
        ULONG count = 0u;

        if (pContext->OplockBlockState) {
            states[count++] = L"block";
        }

        if (pContext->OplockAppState) {
            states[count++] = L"app";
        }

        if (pContext->OplockFinalState) {
            states[count++] = L"final";
        }

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


    __declspec(code_seg("PAGE"))
    NTSTATUS FormatEcps(_In_ FLT_CALLBACK_DATA* pData, _Inout_ UNICODE_STRING* pEcpString) {
        PAGED_CODE();

        ECP_LIST* pEcpList = nullptr;

        if (!NT_SUCCESS(FltGetEcpListFromCallbackData(driver::pFilter, pData, &pEcpList)) || !pEcpList) return STATUS_SUCCESS;

        EcpWriter writer{ pEcpString->Buffer, pEcpString->MaximumLength };
        ULONG total = 0u;
        ULONG recognized = 0u;
        void* pContext = nullptr;
        GUID guid{};
        ULONG contextSize = 0u;

        while (NT_SUCCESS(FltGetNextExtraCreateParameter(driver::pFilter, pEcpList, pContext, &guid, &pContext, &contextSize))) {
            total++;

            if (FltIsEcpFromUserMode(driver::pFilter, pContext)) continue;

            if (AppendEcp(&writer, guid, pContext)) {
                recognized++;
            }

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


    void PopulateImpersonatedSid(_Inout_ protocol::CreateSupplement* pSupplement, _In_ PACCESS_TOKEN pClientToken) {
        TOKEN_USER* pTokenUser = nullptr;
        ULONG sidSize = 0u;

        if (!NT_SUCCESS(SeQueryInformationToken(pClientToken, TokenUser, reinterpret_cast<void**>(&pTokenUser)))) goto done;

        if (!RtlValidSid(pTokenUser->User.Sid)) goto done;

        sidSize = RtlLengthSid(pTokenUser->User.Sid);

        if (sidSize > sizeof(pSupplement->impersonatedSid)) goto done;

        RtlCopyMemory(pSupplement->impersonatedSid, pTokenUser->User.Sid, sidSize);
        pSupplement->captured |= protocol::CREATE_CAPTURED_IMPERSONATED_SID;

    done:

        if (pTokenUser) {
            ExFreePool(pTokenUser);
        }

        return;
    }

}

namespace mimo {

    namespace trace {

        namespace supplement {

            namespace create {

                _Use_decl_annotations_
                void Populate(protocol::CreateSupplement* pSupplement, FLT_CALLBACK_DATA* pData) {

                    if (KeGetCurrentIrql() != PASSIVE_LEVEL) return;

                    UNICODE_STRING ecpText{};
                    RtlInitEmptyUnicodeString(&ecpText, pSupplement->ecpText, static_cast<USHORT>(sizeof(pSupplement->ecpText)));

                    if (FormatEcps(pData, &ecpText) == STATUS_BUFFER_OVERFLOW) {
                        pSupplement->captured |= protocol::CREATE_TRUNCATED_ECP_TEXT;
                    }

                    const void* const pEaBuffer = pData->Iopb->Parameters.Create.EaBuffer;
                    const ULONG bufferSize = pData->Iopb->Parameters.Create.EaLength;

                    if (pEaBuffer && bufferSize) {
                        const ULONG copySize = bufferSize < sizeof(pSupplement->eaBuffer) ? bufferSize : sizeof(pSupplement->eaBuffer);

                        RtlCopyMemory(pSupplement->eaBuffer, pEaBuffer, copySize);

                        if (copySize < bufferSize) {
                            pSupplement->captured |= protocol::CREATE_TRUNCATED_EA_BUFFER;
                        }

                        pSupplement->capturedEaBufferSize = static_cast<uint32_t>(copySize);
                        pSupplement->captured |= protocol::CREATE_CAPTURED_EA_BUFFER;
                    }

                    const IO_SECURITY_CONTEXT* const pSecurityContext = pData->Iopb->Parameters.Create.SecurityContext;

                    if (!pSecurityContext) return;

                    pSupplement->desiredAccess = static_cast<uint32_t>(pSecurityContext->DesiredAccess);
                    pSupplement->captured |= protocol::CREATE_CAPTURED_DESIRED_ACCESS;

                    if (!pSecurityContext->AccessState) return;

                    const PACCESS_TOKEN pClientToken = pSecurityContext->AccessState->SubjectSecurityContext.ClientToken;

                    if (!pClientToken) return;

                    PopulateImpersonatedSid(pSupplement, pClientToken);

                    return;
                }

            }

        }

    }

}
