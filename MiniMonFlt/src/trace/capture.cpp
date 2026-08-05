#include "capture.h"

#include "..\driver.h"
#include "ecp.h"
#include "name.h"
#include "stack.h"

#include "..\..\..\inc\protocol.h"

#include <fltKernel.h>
#include <ntstrsafe.h>

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

MIRROR_ASSERT(volumeInformation.length,             QueryVolumeInformation.Length);
MIRROR_ASSERT(volumeInformation.fsInformationClass, QueryVolumeInformation.FsInformationClass);
MIRROR_ASSERT(volumeInformation.volumeBuffer,       QueryVolumeInformation.VolumeBuffer);

static_assert(
    offsetof(FLT_PARAMETERS, SetVolumeInformation.Length) == offsetof(FLT_PARAMETERS, QueryVolumeInformation.Length)
    && offsetof(FLT_PARAMETERS, SetVolumeInformation.FsInformationClass) == offsetof(FLT_PARAMETERS, QueryVolumeInformation.FsInformationClass)
    && offsetof(FLT_PARAMETERS, SetVolumeInformation.VolumeBuffer) == offsetof(FLT_PARAMETERS, QueryVolumeInformation.VolumeBuffer),
    "FLT_PARAMETERS QueryVolumeInformation and SetVolumeInformation diverge: split protocol::FltParameters volumeInformation"
);

MIRROR_ASSERT(queryDirectory.length,               DirectoryControl.QueryDirectory.Length);
MIRROR_ASSERT(queryDirectory.fileName,             DirectoryControl.QueryDirectory.FileName);
MIRROR_ASSERT(queryDirectory.fileInformationClass, DirectoryControl.QueryDirectory.FileInformationClass);
MIRROR_ASSERT(queryDirectory.fileIndex,            DirectoryControl.QueryDirectory.FileIndex);
MIRROR_ASSERT(queryDirectory.directoryBuffer,      DirectoryControl.QueryDirectory.DirectoryBuffer);
MIRROR_ASSERT(queryDirectory.mdlAddress,           DirectoryControl.QueryDirectory.MdlAddress);

MIRROR_ASSERT(notifyDirectory.length,                          DirectoryControl.NotifyDirectoryEx.Length);
MIRROR_ASSERT(notifyDirectory.completionFilter,                DirectoryControl.NotifyDirectoryEx.CompletionFilter);
MIRROR_ASSERT(notifyDirectory.directoryNotifyInformationClass, DirectoryControl.NotifyDirectoryEx.DirectoryNotifyInformationClass);
MIRROR_ASSERT(notifyDirectory.directoryBuffer,                 DirectoryControl.NotifyDirectoryEx.DirectoryBuffer);
MIRROR_ASSERT(notifyDirectory.mdlAddress,                      DirectoryControl.NotifyDirectoryEx.MdlAddress);

static_assert(
    offsetof(FLT_PARAMETERS, DirectoryControl.NotifyDirectory.Length) == offsetof(FLT_PARAMETERS, DirectoryControl.NotifyDirectoryEx.Length)
    && offsetof(FLT_PARAMETERS, DirectoryControl.NotifyDirectory.CompletionFilter) == offsetof(FLT_PARAMETERS, DirectoryControl.NotifyDirectoryEx.CompletionFilter)
    && offsetof(FLT_PARAMETERS, DirectoryControl.NotifyDirectory.DirectoryBuffer) == offsetof(FLT_PARAMETERS, DirectoryControl.NotifyDirectoryEx.DirectoryBuffer)
    && offsetof(FLT_PARAMETERS, DirectoryControl.NotifyDirectory.MdlAddress) == offsetof(FLT_PARAMETERS, DirectoryControl.NotifyDirectoryEx.MdlAddress),
    "FLT_PARAMETERS NotifyDirectory and NotifyDirectoryEx diverge: split protocol::FltParameters notifyDirectory"
);

static_assert(protocol::CREATE_SID_BYTES == SECURITY_MAX_SID_SIZE, "protocol::CREATE_SID_BYTES mirror drift");

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
        trace::stack::CaptureStackTrace(pRecordData->stackTrace, protocol::STACK_TRACE_FRAMES, &stackFrameCount);
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

        if (!NT_SUCCESS(SeQueryInformationToken(pClientToken, TokenUser, reinterpret_cast<void**>(&pTokenUser)))) goto done;

        ULONG sidSize;

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


    __declspec(code_seg("PAGE"))
    void PopulateCreateSupplement(_Inout_ protocol::CreateSupplement* pSupplement, _In_ FLT_CALLBACK_DATA* pData) {
        PAGED_CODE();

        UNICODE_STRING ecpText{};
        RtlInitEmptyUnicodeString(&ecpText, pSupplement->ecpText, sizeof(pSupplement->ecpText));

        if (trace::ecp::FormatEcps(pData, &ecpText) == STATUS_BUFFER_OVERFLOW) {
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


    __declspec(code_seg("PAGE"))
    void PopulateTargetName(
        _Inout_ protocol::SetInfoSupplement* pSupplement,
        _In_ FLT_CALLBACK_DATA* pData,
        _In_ const FLT_RELATED_OBJECTS* pFltObjects
    ) {
        PAGED_CODE();

        UNICODE_STRING targetName{};
        RtlInitEmptyUnicodeString(&targetName, pSupplement->targetName, sizeof(pSupplement->targetName));

        const NTSTATUS status = trace::name::FormatTargetFileName(pData, pFltObjects, &targetName);

        if (!NT_SUCCESS(status) && status != STATUS_BUFFER_OVERFLOW) return;

        if (status == STATUS_BUFFER_OVERFLOW) {
            pSupplement->captured |= protocol::SET_INFO_TRUNCATED_TARGET_NAME;
        }

        pSupplement->captured |= protocol::SET_INFO_CAPTURED_TARGET_NAME;

        return;
    }


    __declspec(code_seg("PAGE"))
    void PopulateSetInfoSupplement(
        _Inout_ protocol::SetInfoSupplement* pSupplement,
        _In_ FLT_CALLBACK_DATA* pData,
        _In_ const FLT_RELATED_OBJECTS* pFltObjects
    ) {
        PAGED_CODE();

        const ULONG size = pData->Iopb->Parameters.SetFileInformation.Length;
        const ULONG copySize = size < protocol::SET_INFO_PAYLOAD_BYTES ? size : protocol::SET_INFO_PAYLOAD_BYTES;

        __try {
            RtlCopyMemory(pSupplement->payload, pData->Iopb->Parameters.SetFileInformation.InfoBuffer, copySize);
        }
        __except (EXCEPTION_EXECUTE_HANDLER) {

            return;
        }

        pSupplement->capturedBytes = copySize;
        pSupplement->captured |= protocol::SET_INFO_CAPTURED_PAYLOAD;

        switch (pData->Iopb->Parameters.SetFileInformation.FileInformationClass) {

            case FileRenameInformation:
            case FileLinkInformation:
            case FileRenameInformationBypassAccessCheck:
            case FileLinkInformationBypassAccessCheck:
            case FileRenameInformationEx:
            case FileRenameInformationExBypassAccessCheck:
            case FileLinkInformationEx:
            case FileLinkInformationExBypassAccessCheck:
                PopulateTargetName(pSupplement, pData, pFltObjects);

                break;

            default:

                break;

        }

        return;
    }


    __declspec(code_seg("PAGE"))
    void PopulateSetVolumeInfoSupplement(_Inout_ protocol::VolumeInfoSupplement* pSupplement, _In_ const FLT_CALLBACK_DATA* pData) {
        PAGED_CODE();

        const ULONG size = pData->Iopb->Parameters.SetVolumeInformation.Length;
        const ULONG copySize = size < protocol::VOLUME_INFO_PAYLOAD_BYTES ? size : protocol::VOLUME_INFO_PAYLOAD_BYTES;

        __try {
            RtlCopyMemory(pSupplement->payload, pData->Iopb->Parameters.SetVolumeInformation.VolumeBuffer, copySize);
        }
        __except (EXCEPTION_EXECUTE_HANDLER) {

            return;
        }

        pSupplement->capturedBytes = copySize;
        pSupplement->captured |= protocol::VOLUME_INFO_CAPTURED_PAYLOAD;

        return;
    }


    __declspec(code_seg("PAGE"))
    void PopulateQueryDirectoryFileName(_Inout_ protocol::QueryDirectorySupplement* pSupplement, _In_ const FLT_CALLBACK_DATA* pData) {
        PAGED_CODE();

        UNICODE_STRING fileName{};
        RtlInitEmptyUnicodeString(&fileName, pSupplement->fileName, sizeof(pSupplement->fileName));

        NTSTATUS status = STATUS_UNSUCCESSFUL;

        __try {
            status = RtlUnicodeStringCopy(&fileName, pData->Iopb->Parameters.DirectoryControl.QueryDirectory.FileName);
        }
        __except (EXCEPTION_EXECUTE_HANDLER) {

            return;
        }

        if (!NT_SUCCESS(status) && status != STATUS_BUFFER_OVERFLOW) return;

        if (status == STATUS_BUFFER_OVERFLOW) {
            pSupplement->captured |= protocol::QUERY_DIRECTORY_TRUNCATED_FILE_NAME;
        }

        pSupplement->captured |= protocol::QUERY_DIRECTORY_CAPTURED_FILE_NAME;

        return;
    }


    __declspec(code_seg("PAGE"))
    void PopulateQueryInfoSupplement(_Inout_ protocol::QueryInfoSupplement* pSupplement, _In_ const FLT_CALLBACK_DATA* pData) {
        PAGED_CODE();

        const ULONG bufferSize = pData->Iopb->Parameters.QueryFileInformation.Length;
        const ULONG_PTR writtenSize = pData->IoStatus.Information;
        const ULONG dataSize = writtenSize < bufferSize ? static_cast<ULONG>(writtenSize) : bufferSize;
        const ULONG copySize = dataSize < protocol::QUERY_INFO_PAYLOAD_BYTES ? dataSize : protocol::QUERY_INFO_PAYLOAD_BYTES;

        __try {
            RtlCopyMemory(pSupplement->payload, pData->Iopb->Parameters.QueryFileInformation.InfoBuffer, copySize);
        }
        __except (EXCEPTION_EXECUTE_HANDLER) {

            return;
        }

        pSupplement->capturedBytes = copySize;
        pSupplement->captured |= protocol::QUERY_INFO_CAPTURED_PAYLOAD;

        return;
    }


    __declspec(code_seg("PAGE"))
    void PopulateQueryVolumeInfoSupplement(_Inout_ protocol::VolumeInfoSupplement* pSupplement, _In_ const FLT_CALLBACK_DATA* pData) {
        PAGED_CODE();

        const ULONG bufferSize = pData->Iopb->Parameters.QueryVolumeInformation.Length;
        const ULONG_PTR writtenSize = pData->IoStatus.Information;
        const ULONG dataSize = writtenSize < bufferSize ? static_cast<ULONG>(writtenSize) : bufferSize;
        const ULONG copySize = dataSize < protocol::VOLUME_INFO_PAYLOAD_BYTES ? dataSize : protocol::VOLUME_INFO_PAYLOAD_BYTES;

        __try {
            RtlCopyMemory(pSupplement->payload, pData->Iopb->Parameters.QueryVolumeInformation.VolumeBuffer, copySize);
        }
        __except (EXCEPTION_EXECUTE_HANDLER) {

            return;
        }

        pSupplement->capturedBytes = copySize;
        pSupplement->captured |= protocol::VOLUME_INFO_CAPTURED_PAYLOAD;

        return;
    }


    __declspec(code_seg("PAGE"))
    void PopulateQueryDirectoryPayload(_Inout_ protocol::QueryDirectorySupplement* pSupplement, _In_ const FLT_CALLBACK_DATA* pData) {
        PAGED_CODE();

        const ULONG bufferSize = pData->Iopb->Parameters.DirectoryControl.QueryDirectory.Length;
        const ULONG_PTR writtenSize = pData->IoStatus.Information;
        const ULONG dataSize = writtenSize < bufferSize ? static_cast<ULONG>(writtenSize) : bufferSize;
        const ULONG copySize = dataSize < protocol::QUERY_DIRECTORY_PAYLOAD_BYTES ? dataSize : protocol::QUERY_DIRECTORY_PAYLOAD_BYTES;

        __try {
            RtlCopyMemory(pSupplement->payload, pData->Iopb->Parameters.DirectoryControl.QueryDirectory.DirectoryBuffer, copySize);
        }
        __except (EXCEPTION_EXECUTE_HANDLER) {

            return;
        }

        pSupplement->capturedBytes = copySize;
        pSupplement->captured |= protocol::QUERY_DIRECTORY_CAPTURED_PAYLOAD;

        return;
    }

}

namespace mimo {

    namespace trace {

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

                if (name::FormatFileName(pData, pFltObjects, &fileName) == STATUS_BUFFER_OVERFLOW) {
                    pRecordData->truncated |= protocol::TRUNCATED_NAME;
                }

                switch (pData->Iopb->MajorFunction) {

                    case IRP_MJ_CREATE:

                        if (KeGetCurrentIrql() == PASSIVE_LEVEL) {
                            PopulateCreateSupplement(&pRecordData->supplement.create, pData);
                        }

                        break;

                    case IRP_MJ_SET_INFORMATION:

                        if (KeGetCurrentIrql() < DISPATCH_LEVEL && pData->Iopb->Parameters.SetFileInformation.InfoBuffer) {
                            PopulateSetInfoSupplement(&pRecordData->supplement.setInfo, pData, pFltObjects);
                        }

                        break;

                    case IRP_MJ_SET_VOLUME_INFORMATION:

                        if (KeGetCurrentIrql() < DISPATCH_LEVEL && pData->Iopb->Parameters.SetVolumeInformation.VolumeBuffer) {
                            PopulateSetVolumeInfoSupplement(&pRecordData->supplement.volumeInfo, pData);
                        }

                        break;

                    case IRP_MJ_DIRECTORY_CONTROL:

                        if (pData->Iopb->MinorFunction == IRP_MN_QUERY_DIRECTORY && KeGetCurrentIrql() < DISPATCH_LEVEL && pData->Iopb->Parameters.DirectoryControl.QueryDirectory.FileName) {
                            PopulateQueryDirectoryFileName(&pRecordData->supplement.queryDirectory, pData);
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

                        if ((NT_SUCCESS(pData->IoStatus.Status) || pData->IoStatus.Status == STATUS_BUFFER_OVERFLOW) && KeGetCurrentIrql() < DISPATCH_LEVEL && pData->Iopb->Parameters.QueryFileInformation.InfoBuffer) {
                            PopulateQueryInfoSupplement(&pRecordData->supplement.queryInfo, pData);
                        }

                        break;

                    case IRP_MJ_QUERY_VOLUME_INFORMATION:

                        if ((NT_SUCCESS(pData->IoStatus.Status) || pData->IoStatus.Status == STATUS_BUFFER_OVERFLOW) && KeGetCurrentIrql() < DISPATCH_LEVEL && pData->Iopb->Parameters.QueryVolumeInformation.VolumeBuffer) {
                            PopulateQueryVolumeInfoSupplement(&pRecordData->supplement.volumeInfo, pData);
                        }

                        break;

                    case IRP_MJ_DIRECTORY_CONTROL:

                        if (pData->Iopb->MinorFunction == IRP_MN_QUERY_DIRECTORY && (NT_SUCCESS(pData->IoStatus.Status) || pData->IoStatus.Status == STATUS_BUFFER_OVERFLOW) && KeGetCurrentIrql() < DISPATCH_LEVEL && pData->Iopb->Parameters.DirectoryControl.QueryDirectory.DirectoryBuffer) {
                            PopulateQueryDirectoryPayload(&pRecordData->supplement.queryDirectory, pData);
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

}
