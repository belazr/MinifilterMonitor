#include "capture.h"

#include "driver.h"
#include "stack.h"

#include "..\..\inc\protocol.h"

#include <fltKernel.h>
#include <ntstrsafe.h>

using namespace mimo;

static_assert(sizeof(protocol::FltParameters) == sizeof(FLT_PARAMETERS), "protocol::FltParameters mirror drift");

static_assert(
    offsetof(protocol::FltParameters, readWrite.length) == offsetof(FLT_PARAMETERS, Read.Length)
    && RTL_FIELD_SIZE(protocol::FltParameters, readWrite.length) == RTL_FIELD_SIZE(FLT_PARAMETERS, Read.Length),
    "protocol::FltParameters mirror drift: readWrite.length"
);

static_assert(
    offsetof(protocol::FltParameters, readWrite.key) == offsetof(FLT_PARAMETERS, Read.Key)
    && RTL_FIELD_SIZE(protocol::FltParameters, readWrite.key) == RTL_FIELD_SIZE(FLT_PARAMETERS, Read.Key),
    "protocol::FltParameters mirror drift: readWrite.key"
);

static_assert(
    offsetof(protocol::FltParameters, readWrite.byteOffset) == offsetof(FLT_PARAMETERS, Read.ByteOffset)
    && RTL_FIELD_SIZE(protocol::FltParameters, readWrite.byteOffset) == RTL_FIELD_SIZE(FLT_PARAMETERS, Read.ByteOffset),
    "protocol::FltParameters mirror drift: readWrite.byteOffset"
);

static_assert(
    offsetof(protocol::FltParameters, readWrite.buffer) == offsetof(FLT_PARAMETERS, Read.ReadBuffer)
    && RTL_FIELD_SIZE(protocol::FltParameters, readWrite.buffer) == RTL_FIELD_SIZE(FLT_PARAMETERS, Read.ReadBuffer),
    "protocol::FltParameters mirror drift: readWrite.buffer"
);

static_assert(
    offsetof(protocol::FltParameters, readWrite.mdlAddress) == offsetof(FLT_PARAMETERS, Read.MdlAddress)
    && RTL_FIELD_SIZE(protocol::FltParameters, readWrite.mdlAddress) == RTL_FIELD_SIZE(FLT_PARAMETERS, Read.MdlAddress),
    "protocol::FltParameters mirror drift: readWrite.mdlAddress"
);

static_assert(
    offsetof(FLT_PARAMETERS, Write.Length) == offsetof(FLT_PARAMETERS, Read.Length)
    && offsetof(FLT_PARAMETERS, Write.Key) == offsetof(FLT_PARAMETERS, Read.Key)
    && offsetof(FLT_PARAMETERS, Write.ByteOffset) == offsetof(FLT_PARAMETERS, Read.ByteOffset)
    && offsetof(FLT_PARAMETERS, Write.WriteBuffer) == offsetof(FLT_PARAMETERS, Read.ReadBuffer)
    && offsetof(FLT_PARAMETERS, Write.MdlAddress) == offsetof(FLT_PARAMETERS, Read.MdlAddress),
    "protocol::FltParameters mirror drift: readWrite covers Read and Write"
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

}

namespace mimo {

    namespace capture {

        _Use_decl_annotations_
        void PopulatePreOperationRecordData(protocol::RecordData* pRecordData, const FLT_CALLBACK_DATA* pData, const FLT_RELATED_OBJECTS* pFltObjects, const UNICODE_STRING* pName, const UNICODE_STRING* pEcpData) {
            PopulateOriginRecordData(pRecordData, pFltObjects);

            pRecordData->callbackMajorId = pData->Iopb->MajorFunction;
            pRecordData->callbackMinorId = pData->Iopb->MinorFunction;
            pRecordData->operationFlags  = pData->Iopb->OperationFlags;
            pRecordData->irpFlags        = pData->Iopb->IrpFlags;
            pRecordData->flags           = pData->Flags;
            pRecordData->operationId     = reinterpret_cast<protocol::ObjectId>(pData);
            pRecordData->topLevelIrp     = reinterpret_cast<protocol::ObjectId>(IoGetTopLevelIrp());

            RtlCopyMemory(&pRecordData->parameters, &pData->Iopb->Parameters, sizeof(pRecordData->parameters));

            FLT_ASSERT(pName);
            FLT_ASSERT(pEcpData);

            if (pEcpData->Length) {
                RtlStringCbPrintfW(pRecordData->name, sizeof(pRecordData->name), L"%wZ %wZ", pName, pEcpData);
            }
            else {
                RtlStringCbPrintfW(pRecordData->name, sizeof(pRecordData->name), L"%wZ", pName);
            }

            return;
        }


        _Use_decl_annotations_
        void PopulatePostOperationRecordData(protocol::RecordData* pRecordData, const FLT_CALLBACK_DATA* pData, ULONG transactionSequence) {
            pRecordData->status = pData->IoStatus.Status;
            pRecordData->information = pData->IoStatus.Information;
            pRecordData->transactionSequence = transactionSequence;

            if (pData->TagData) {
                pRecordData->reparseTag = pData->TagData->FileTag;
            }

            LARGE_INTEGER completionTime{};
            KeQuerySystemTimePrecise(&completionTime);
            pRecordData->completionTime = completionTime.QuadPart;

            return;
        }


        _Use_decl_annotations_
        void PopulateTransactionEventRecordData(protocol::RecordData* pRecordData, const FLT_RELATED_OBJECTS* pFltObjects, ULONG notificationMask, ULONG transactionSequence) {
            PopulateOriginRecordData(pRecordData, pFltObjects);

            pRecordData->transactionNotify = notificationMask;
            pRecordData->transactionSequence = transactionSequence;

            return;
        }

    }

}
