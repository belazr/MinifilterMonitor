#include "dispatch.h"

#include "trace\capture.h"

#include "records.h"
#include "transaction.h"

#include <fltKernel.h>

namespace mimo {

    namespace dispatch {

        _Use_decl_annotations_
        FLT_PREOP_CALLBACK_STATUS PreOperationCallback(
            FLT_CALLBACK_DATA* pData,
            const FLT_RELATED_OBJECTS* pFltObjects,
            void** ppCompletionContext
        ) {
            records::Entry* const pEntry = records::CreateEntry();

            if (!pEntry) return FLT_PREOP_SUCCESS_NO_CALLBACK;

            trace::capture::PopulatePreOperationRecordData(&pEntry->record.data, pData, pFltObjects);

            if (pData->Iopb->MajorFunction == IRP_MJ_SHUTDOWN) {
                PostOperationCallback(pData, pFltObjects, pEntry, 0u);

                return FLT_PREOP_SUCCESS_NO_CALLBACK;
            }

            *ppCompletionContext = pEntry;

            return FLT_PREOP_SUCCESS_WITH_CALLBACK;
        }


        _Use_decl_annotations_
        FLT_POSTOP_CALLBACK_STATUS PostOperationCallback(
            FLT_CALLBACK_DATA* pData,
            const FLT_RELATED_OBJECTS* pFltObjects,
            void* pCompletionContext,
            FLT_POST_OPERATION_FLAGS flags
        ) {
            records::Entry* const pEntry = reinterpret_cast<records::Entry*>(pCompletionContext);

            if (FlagOn(flags, FLTFL_POST_OPERATION_DRAINING)) {
                records::DeleteEntry(pEntry);

                return FLT_POSTOP_FINISHED_PROCESSING;
            }

            ULONG transactionSequence = 0u;

            if (pFltObjects->Transaction && KeGetCurrentIrql() == PASSIVE_LEVEL) {
                transactionSequence = transaction::Enlist(pFltObjects);
            }

            trace::capture::PopulatePostOperationRecordData(&pEntry->record.data, pData, transactionSequence);
            records::Append(pEntry);

            return FLT_POSTOP_FINISHED_PROCESSING;
        }

    }

}
