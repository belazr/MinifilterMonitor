#include "capture.h"

#include "driver.h"
#include "stack.h"

#include "..\..\inc\protocol.h"

#include <fltKernel.h>
#include <ntstrsafe.h>

using namespace mimo;

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
            pRecordData->irpFlags        = pData->Iopb->IrpFlags;
            pRecordData->flags           = pData->Flags;
            pRecordData->operationId     = reinterpret_cast<protocol::ObjectId>(pData);
            pRecordData->topLevelIrp     = reinterpret_cast<protocol::ObjectId>(IoGetTopLevelIrp());

            pRecordData->arg1 = reinterpret_cast<ULONGLONG>(pData->Iopb->Parameters.Others.Argument1);
            pRecordData->arg2 = reinterpret_cast<ULONGLONG>(pData->Iopb->Parameters.Others.Argument2);
            pRecordData->arg3 = reinterpret_cast<ULONGLONG>(pData->Iopb->Parameters.Others.Argument3);
            pRecordData->arg4 = reinterpret_cast<ULONGLONG>(pData->Iopb->Parameters.Others.Argument4);
            pRecordData->arg5 = reinterpret_cast<ULONGLONG>(pData->Iopb->Parameters.Others.Argument5);
            pRecordData->arg6 = pData->Iopb->Parameters.Others.Argument6.QuadPart;

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
