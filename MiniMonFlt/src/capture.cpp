#include "capture.h"

#include "driver.h"
#include "ecp.h"
#include "name.h"
#include "stack.h"

#include "..\..\inc\protocol.h"

#include <fltKernel.h>

using namespace mimo;

#define MIRROR_ASSERT(mirrorMember, fltMember) static_assert( \
    offsetof(protocol::FltParameters, mirrorMember) == offsetof(FLT_PARAMETERS, fltMember) \
    && RTL_FIELD_SIZE(protocol::FltParameters, mirrorMember) == RTL_FIELD_SIZE(FLT_PARAMETERS, fltMember), \
    "protocol::FltParameters mirror drift: " #mirrorMember)

static_assert(sizeof(protocol::FltParameters) == sizeof(FLT_PARAMETERS), "protocol::FltParameters mirror drift");

MIRROR_ASSERT(create.securityContext, Create.SecurityContext);
MIRROR_ASSERT(create.options,         Create.Options);
MIRROR_ASSERT(create.fileAttributes,  Create.FileAttributes);
MIRROR_ASSERT(create.shareAccess,     Create.ShareAccess);
MIRROR_ASSERT(create.eaLength,        Create.EaLength);
MIRROR_ASSERT(create.eaBuffer,        Create.EaBuffer);
MIRROR_ASSERT(create.allocationSize,  Create.AllocationSize);

MIRROR_ASSERT(readWrite.length,     Read.Length);
MIRROR_ASSERT(readWrite.key,        Read.Key);
MIRROR_ASSERT(readWrite.byteOffset, Read.ByteOffset);
MIRROR_ASSERT(readWrite.buffer,     Read.ReadBuffer);
MIRROR_ASSERT(readWrite.mdlAddress, Read.MdlAddress);

static_assert(
    offsetof(FLT_PARAMETERS, Write.Length) == offsetof(FLT_PARAMETERS, Read.Length)
    && offsetof(FLT_PARAMETERS, Write.Key) == offsetof(FLT_PARAMETERS, Read.Key)
    && offsetof(FLT_PARAMETERS, Write.ByteOffset) == offsetof(FLT_PARAMETERS, Read.ByteOffset)
    && offsetof(FLT_PARAMETERS, Write.WriteBuffer) == offsetof(FLT_PARAMETERS, Read.ReadBuffer)
    && offsetof(FLT_PARAMETERS, Write.MdlAddress) == offsetof(FLT_PARAMETERS, Read.MdlAddress),
    "FLT_PARAMETERS Read and Write diverge: split protocol::FltParameters readWrite"
);

MIRROR_ASSERT(queryFileInformation.length,               QueryFileInformation.Length);
MIRROR_ASSERT(queryFileInformation.fileInformationClass, QueryFileInformation.FileInformationClass);
MIRROR_ASSERT(queryFileInformation.infoBuffer,           QueryFileInformation.InfoBuffer);

MIRROR_ASSERT(setFileInformation.length,                SetFileInformation.Length);
MIRROR_ASSERT(setFileInformation.fileInformationClass,  SetFileInformation.FileInformationClass);
MIRROR_ASSERT(setFileInformation.parentOfTarget,        SetFileInformation.ParentOfTarget);
MIRROR_ASSERT(setFileInformation.flags.replaceIfExists, SetFileInformation.ReplaceIfExists);
MIRROR_ASSERT(setFileInformation.flags.advanceOnly,     SetFileInformation.AdvanceOnly);
MIRROR_ASSERT(setFileInformation.clusterCount,          SetFileInformation.ClusterCount);
MIRROR_ASSERT(setFileInformation.deleteHandle,          SetFileInformation.DeleteHandle);
MIRROR_ASSERT(setFileInformation.infoBuffer,            SetFileInformation.InfoBuffer);

static_assert(protocol::SID_BYTES == SECURITY_MAX_SID_SIZE, "protocol::SID_BYTES mirror drift");

namespace {

    void PopulateOriginRecordData(_Inout_ protocol::RecordData* pRecordData, _In_ const FLT_RELATED_OBJECTS* pFltObjects) {
        DEVICE_OBJECT* pDevObj = nullptr;
        const NTSTATUS status = FltGetDeviceObject(pFltObjects->Volume, &pDevObj);

        if (NT_SUCCESS(status)) {
            ObDereferenceObject(pDevObj);
        }
        else {
            pDevObj = nullptr;
        }

        pRecordData->deviceObject = reinterpret_cast<protocol::ObjectId>(pDevObj);
        pRecordData->fileObject   = reinterpret_cast<protocol::ObjectId>(pFltObjects->FileObject);
        pRecordData->transaction  = reinterpret_cast<protocol::ObjectId>(pFltObjects->Transaction);
        pRecordData->processId    = reinterpret_cast<protocol::ObjectId>(PsGetCurrentProcessId());
        pRecordData->threadId     = reinterpret_cast<protocol::ObjectId>(PsGetCurrentThreadId());

        driver::InstanceContext* pInstanceContext = nullptr;

        if (pFltObjects->Instance && NT_SUCCESS(FltGetInstanceContext(pFltObjects->Instance, reinterpret_cast<PFLT_CONTEXT*>(&pInstanceContext)))) {
            pRecordData->altitude = pInstanceContext->altitude;
            FltReleaseContext(pInstanceContext);
        }

        ULONG stackFrameCount = 0u;
        stack::CaptureStackTrace(pRecordData->stackTrace, protocol::STACK_TRACE_FRAMES, &stackFrameCount);
        pRecordData->stackFrameCount = stackFrameCount;

        LARGE_INTEGER originatingTime{};
        KeQuerySystemTimePrecise(&originatingTime);
        pRecordData->originatingTime = originatingTime.QuadPart;

        return;
    }


    __declspec(code_seg("PAGE"))
    void PopulateImpersonatedSid(_Inout_ protocol::CreateSupplement* pSupplement, _In_ PACCESS_TOKEN pClientToken) {
        PAGED_CODE();

        TOKEN_USER* pTokenUser = nullptr;

        if (!NT_SUCCESS(SeQueryInformationToken(pClientToken, TokenUser, reinterpret_cast<void**>(&pTokenUser)))) return;

        ULONG sidSize;

        if (!RtlValidSid(pTokenUser->User.Sid)) goto done;

        sidSize = RtlLengthSid(pTokenUser->User.Sid);

        if (sidSize > sizeof(pSupplement->impersonatedSid)) goto done;

        RtlCopyMemory(pSupplement->impersonatedSid, pTokenUser->User.Sid, sidSize);
        pSupplement->captured |= protocol::CREATE_CAPTURED_IMPERSONATED_SID;

    done:

        ExFreePool(pTokenUser);

        return;
    }


    __declspec(code_seg("PAGE"))
    void PopulateCreateSupplement(_Inout_ protocol::RecordData* pRecordData, _In_ FLT_CALLBACK_DATA* pData) {
        PAGED_CODE();

        protocol::CreateSupplement* const pSupplement = &pRecordData->supplement.create;

        UNICODE_STRING ecpText{};
        RtlInitEmptyUnicodeString(&ecpText, pSupplement->ecpText, sizeof(pSupplement->ecpText));
        ecp::FormatEcps(pData, &ecpText);

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


    void PopulateInfoSupplement(
        _Inout_ protocol::RecordData* pRecordData,
        _In_reads_bytes_(size) const void* pBuffer,
        _In_ ULONG size
    ) {
        protocol::InfoSupplement* const pSupplement = &pRecordData->supplement.info;
        const ULONG copySize = size < protocol::INFO_PAYLOAD_BYTES ? size : protocol::INFO_PAYLOAD_BYTES;

        __try {
            RtlCopyMemory(pSupplement->payload, pBuffer, copySize);
        }
        __except (EXCEPTION_EXECUTE_HANDLER) {

            return;
        }

        pSupplement->capturedBytes = copySize;
        pSupplement->captured |= protocol::INFO_CAPTURED_PAYLOAD;

        return;
    }

}

namespace mimo {

    namespace capture {

        _Use_decl_annotations_
        void PopulatePreOperationRecordData(
            protocol::RecordData* pRecordData,
            FLT_CALLBACK_DATA* pData,
            const FLT_RELATED_OBJECTS* pFltObjects
        ) {
            PopulateOriginRecordData(pRecordData, pFltObjects);

            pRecordData->callbackMajorId = pData->Iopb->MajorFunction;
            pRecordData->callbackMinorId = pData->Iopb->MinorFunction;
            pRecordData->operationFlags  = pData->Iopb->OperationFlags;
            pRecordData->irpFlags        = pData->Iopb->IrpFlags;
            pRecordData->flags           = pData->Flags;
            pRecordData->operationId     = reinterpret_cast<protocol::ObjectId>(pData);
            pRecordData->topLevelIrp     = reinterpret_cast<protocol::ObjectId>(IoGetTopLevelIrp());

            RtlCopyMemory(&pRecordData->parameters, &pData->Iopb->Parameters, sizeof(pRecordData->parameters));

            UNICODE_STRING fileName{};
            RtlInitEmptyUnicodeString(&fileName, pRecordData->name, sizeof(pRecordData->name));
            name::ResolveFileName(pData, pFltObjects, &fileName);

            switch (pData->Iopb->MajorFunction) {

                case IRP_MJ_CREATE:

                    if (KeGetCurrentIrql() == PASSIVE_LEVEL) {
                        PopulateCreateSupplement(pRecordData, pData);
                    }

                    break;

                case IRP_MJ_SET_INFORMATION:

                    if (KeGetCurrentIrql() < DISPATCH_LEVEL && pData->Iopb->Parameters.SetFileInformation.InfoBuffer) {
                        PopulateInfoSupplement(pRecordData, pData->Iopb->Parameters.SetFileInformation.InfoBuffer, pData->Iopb->Parameters.SetFileInformation.Length);
                    }

                    break;

            }

            return;
        }


        _Use_decl_annotations_
        void PopulatePostOperationRecordData(
            protocol::RecordData* pRecordData,
            const FLT_CALLBACK_DATA* pData,
            ULONG transactionSequence
        ) {
            pRecordData->status = pData->IoStatus.Status;
            pRecordData->information = pData->IoStatus.Information;
            pRecordData->transactionSequence = transactionSequence;

            if (pData->TagData) {
                pRecordData->reparseTag = pData->TagData->FileTag;
            }

            switch (pData->Iopb->MajorFunction) {

                case IRP_MJ_QUERY_INFORMATION:

                    if ((NT_SUCCESS(pData->IoStatus.Status) || pData->IoStatus.Status == STATUS_BUFFER_OVERFLOW) && pData->Iopb->Parameters.QueryFileInformation.InfoBuffer && KeGetCurrentIrql() < DISPATCH_LEVEL) {
                        const ULONG length = pData->Iopb->Parameters.QueryFileInformation.Length;
                        const ULONG_PTR information = pData->IoStatus.Information;
                        const ULONG size = information < length ? static_cast<ULONG>(information) : length;

                        PopulateInfoSupplement(pRecordData, pData->Iopb->Parameters.QueryFileInformation.InfoBuffer, size);
                    }

                    break;

            }

            LARGE_INTEGER completionTime{};
            KeQuerySystemTimePrecise(&completionTime);
            pRecordData->completionTime = completionTime.QuadPart;

            return;
        }


        _Use_decl_annotations_
        void PopulateTransactionEventRecordData(
            protocol::RecordData* pRecordData,
            const FLT_RELATED_OBJECTS* pFltObjects,
            ULONG notificationMask,
            ULONG transactionSequence
        ) {
            PopulateOriginRecordData(pRecordData, pFltObjects);

            pRecordData->transactionNotify = notificationMask;
            pRecordData->transactionSequence = transactionSequence;

            return;
        }

    }

}
