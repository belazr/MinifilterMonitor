#include "capture.h"

#include "name.h"
#include "stack.h"
#include "supplement.h"

#include "..\driver.h"

#include "..\..\..\inc\protocol.h"

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

MIRROR_ASSERT(fileSystemControl.outputBufferLength, FileSystemControl.Neither.OutputBufferLength);
MIRROR_ASSERT(fileSystemControl.inputBufferLength,  FileSystemControl.Neither.InputBufferLength);
MIRROR_ASSERT(fileSystemControl.fsControlCode,      FileSystemControl.Neither.FsControlCode);
MIRROR_ASSERT(fileSystemControl.inputBuffer,        FileSystemControl.Neither.InputBuffer);
MIRROR_ASSERT(fileSystemControl.outputBuffer,       FileSystemControl.Neither.OutputBuffer);
MIRROR_ASSERT(fileSystemControl.outputMdlAddress,   FileSystemControl.Neither.OutputMdlAddress);

static_assert(
    offsetof(FLT_PARAMETERS, FileSystemControl.Common.OutputBufferLength) == offsetof(FLT_PARAMETERS, FileSystemControl.Neither.OutputBufferLength)
    && offsetof(FLT_PARAMETERS, FileSystemControl.Common.InputBufferLength) == offsetof(FLT_PARAMETERS, FileSystemControl.Neither.InputBufferLength)
    && offsetof(FLT_PARAMETERS, FileSystemControl.Common.FsControlCode) == offsetof(FLT_PARAMETERS, FileSystemControl.Neither.FsControlCode),
    "FLT_PARAMETERS FileSystemControl.Common and FileSystemControl.Neither diverge: split protocol::FltParameters fileSystemControl"
);

static_assert(
    offsetof(FLT_PARAMETERS, FileSystemControl.Buffered.OutputBufferLength) == offsetof(FLT_PARAMETERS, FileSystemControl.Neither.OutputBufferLength)
    && offsetof(FLT_PARAMETERS, FileSystemControl.Buffered.InputBufferLength) == offsetof(FLT_PARAMETERS, FileSystemControl.Neither.InputBufferLength)
    && offsetof(FLT_PARAMETERS, FileSystemControl.Buffered.FsControlCode) == offsetof(FLT_PARAMETERS, FileSystemControl.Neither.FsControlCode)
    && offsetof(FLT_PARAMETERS, FileSystemControl.Buffered.SystemBuffer) == offsetof(FLT_PARAMETERS, FileSystemControl.Neither.InputBuffer),
    "FLT_PARAMETERS FileSystemControl.Buffered and FileSystemControl.Neither diverge: split protocol::FltParameters fileSystemControl"
);

static_assert(
    offsetof(FLT_PARAMETERS, FileSystemControl.Direct.OutputBufferLength) == offsetof(FLT_PARAMETERS, FileSystemControl.Neither.OutputBufferLength)
    && offsetof(FLT_PARAMETERS, FileSystemControl.Direct.InputBufferLength) == offsetof(FLT_PARAMETERS, FileSystemControl.Neither.InputBufferLength)
    && offsetof(FLT_PARAMETERS, FileSystemControl.Direct.FsControlCode) == offsetof(FLT_PARAMETERS, FileSystemControl.Neither.FsControlCode)
    && offsetof(FLT_PARAMETERS, FileSystemControl.Direct.InputSystemBuffer) == offsetof(FLT_PARAMETERS, FileSystemControl.Neither.InputBuffer)
    && offsetof(FLT_PARAMETERS, FileSystemControl.Direct.OutputBuffer) == offsetof(FLT_PARAMETERS, FileSystemControl.Neither.OutputBuffer)
    && offsetof(FLT_PARAMETERS, FileSystemControl.Direct.OutputMdlAddress) == offsetof(FLT_PARAMETERS, FileSystemControl.Neither.OutputMdlAddress),
    "FLT_PARAMETERS FileSystemControl.Direct and FileSystemControl.Neither diverge: split protocol::FltParameters fileSystemControl"
);

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
            pRecordData->altitude = static_cast<uint32_t>(pInstanceContext->altitude);
            FltReleaseContext(pInstanceContext);
        }

        ULONG stackFrameCount = 0u;
        trace::stack::CaptureStackTrace(pRecordData->stackTrace, static_cast<ULONG>(protocol::STACK_TRACE_FRAMES), &stackFrameCount);
        pRecordData->stackFrameCount = static_cast<uint32_t>(stackFrameCount);

        LARGE_INTEGER originatingTime{};
        KeQuerySystemTimePrecise(&originatingTime);
        pRecordData->originatingTime = originatingTime.QuadPart;

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
                pRecordData->irpFlags        = static_cast<uint32_t>(pData->Iopb->IrpFlags);
                pRecordData->flags           = static_cast<uint32_t>(pData->Flags);
                pRecordData->operationId     = reinterpret_cast<protocol::ObjectId>(pData);
                pRecordData->topLevelIrp     = reinterpret_cast<protocol::ObjectId>(IoGetTopLevelIrp());

                RtlCopyMemory(&pRecordData->parameters, &pData->Iopb->Parameters, sizeof(pRecordData->parameters));

                UNICODE_STRING fileName{};
                RtlInitEmptyUnicodeString(&fileName, pRecordData->name, static_cast<USHORT>(sizeof(pRecordData->name)));

                if (name::FormatFileName(pData, pFltObjects, &fileName) == STATUS_BUFFER_OVERFLOW) {
                    pRecordData->truncated |= protocol::TRUNCATED_NAME;
                }

                supplement::PopulatePreOperation(&pRecordData->supplement, pData, pFltObjects);

                return;
            }


            _Use_decl_annotations_
            void PopulatePostOperationRecordData(
                protocol::RecordData* pRecordData,
                const FLT_CALLBACK_DATA* pData,
                ULONG transactionSequence
            ) {
                pRecordData->status = static_cast<int32_t>(pData->IoStatus.Status);
                pRecordData->information = pData->IoStatus.Information;
                pRecordData->transactionSequence = static_cast<uint32_t>(transactionSequence);

                if (pData->TagData) {
                    pRecordData->reparseTag = static_cast<uint32_t>(pData->TagData->FileTag);
                }

                supplement::PopulatePostOperation(&pRecordData->supplement, pData);

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

                pRecordData->transactionNotify = static_cast<uint32_t>(notificationMask);
                pRecordData->transactionSequence = static_cast<uint32_t>(transactionSequence);

                return;
            }

        }

    }

}
