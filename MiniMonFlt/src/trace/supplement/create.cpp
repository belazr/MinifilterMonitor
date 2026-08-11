#include "create.h"

#include "..\ecp.h"

#include "..\..\..\..\inc\protocol.h"

#include <fltKernel.h>

using namespace mimo;

static_assert(protocol::CREATE_SID_BYTES == SECURITY_MAX_SID_SIZE, "protocol::CREATE_SID_BYTES mirror drift");

namespace {

    __declspec(code_seg("PAGE"))
    void PopulateImpersonatedSid(_Inout_ protocol::CreateSupplement* pSupplement, _In_ PACCESS_TOKEN pClientToken) {
        PAGED_CODE();

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

                __declspec(code_seg("PAGE"))
                _Use_decl_annotations_
                void Populate(protocol::CreateSupplement* pSupplement, FLT_CALLBACK_DATA* pData) {
                    PAGED_CODE();

                    UNICODE_STRING ecpText{};
                    RtlInitEmptyUnicodeString(&ecpText, pSupplement->ecpText, sizeof(pSupplement->ecpText));

                    if (ecp::FormatEcps(pData, &ecpText) == STATUS_BUFFER_OVERFLOW) {
                        pSupplement->captured |= protocol::CREATE_TRUNCATED_ECP_TEXT;
                    }

                    const IO_SECURITY_CONTEXT* const pSecurityContext = pData->Iopb->Parameters.Create.SecurityContext;

                    if (!pSecurityContext) return;

                    pSupplement->desiredAccess = pSecurityContext->DesiredAccess;
                    pSupplement->captured |= protocol::CREATE_CAPTURED_DESIRED_ACCESS;

                    if (!pSecurityContext->AccessState) return;

                    PACCESS_TOKEN const pClientToken = pSecurityContext->AccessState->SubjectSecurityContext.ClientToken;

                    if (!pClientToken) return;

                    PopulateImpersonatedSid(pSupplement, pClientToken);

                    return;
                }

            }

        }

    }

}
