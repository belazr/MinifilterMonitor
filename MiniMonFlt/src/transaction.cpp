#include "transaction.h"

#include "capture.h"
#include "records.h"

#include <fltKernel.h>

using namespace mimo;

namespace {

    volatile LONG TransactionSequence;

}

namespace mimo {

    namespace transaction {

        __declspec(code_seg("PAGE"))
        _Use_decl_annotations_
        ULONG Enlist(const FLT_RELATED_OBJECTS* pFltObjects) {
            PAGED_CODE();

            Context* pContext = nullptr;
            NTSTATUS status = STATUS_SUCCESS;
            Context* pOldContext = nullptr;
            ULONG sequence = 0u;

            status = FltGetTransactionContext(pFltObjects->Instance, pFltObjects->Transaction, reinterpret_cast<PFLT_CONTEXT*>(&pContext));

            if (!NT_SUCCESS(status)) {

                if (status != STATUS_NOT_FOUND) goto done;

                status = FltAllocateContext(pFltObjects->Filter, FLT_TRANSACTION_CONTEXT, sizeof(Context), PagedPool, reinterpret_cast<PFLT_CONTEXT*>(&pContext));

                if (!NT_SUCCESS(status)) goto done;

                RtlZeroMemory(pContext, sizeof(Context));
                pContext->sequence = static_cast<ULONG>(InterlockedIncrement(&TransactionSequence));

                status = FltSetTransactionContext(pFltObjects->Instance, pFltObjects->Transaction, FLT_SET_CONTEXT_KEEP_IF_EXISTS, pContext, reinterpret_cast<PFLT_CONTEXT*>(&pOldContext));

                if (!NT_SUCCESS(status)) {
                    FltReleaseContext(pContext);
                    pContext = nullptr;

                    if (status != STATUS_FLT_CONTEXT_ALREADY_DEFINED) goto done;

                    FLT_ASSERT(pOldContext);

                    pContext = pOldContext;
                    pOldContext = nullptr;
                }

            }

            sequence = pContext->sequence;
            status = FltEnlistInTransaction(pFltObjects->Instance, pFltObjects->Transaction, pContext, FLT_MAX_TRANSACTION_NOTIFICATIONS);
            FLT_ASSERT(NT_SUCCESS(status) || status == STATUS_FLT_ALREADY_ENLISTED);

        done:

            if (pOldContext) {
                FltReleaseContext(pOldContext);
            }

            if (pContext) {
                FltReleaseContext(pContext);
            }

            return sequence;
        }


        __declspec(code_seg("PAGE"))
        _Use_decl_annotations_
        NTSTATUS HandleNotification(const FLT_RELATED_OBJECTS* pFltObjects, PFLT_CONTEXT pContext, ULONG notificationMask) {
            PAGED_CODE();

            const Context* const pCtx = reinterpret_cast<Context*>(pContext);
            records::Entry* const pEntry = records::CreateEntry();

            if (!pEntry) return STATUS_SUCCESS;

            capture::PopulateTransactionEventRecordData(&pEntry->record.data, pFltObjects, notificationMask, pCtx->sequence);
            records::Append(pEntry);

            return STATUS_SUCCESS;
        }

    }

}
