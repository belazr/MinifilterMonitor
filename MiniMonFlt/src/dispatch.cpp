#include "dispatch.h"

#include "capture.h"
#include "ecp.h"
#include "records.h"
#include "transaction.h"

#include "..\..\inc\protocol.h"

#include <fltKernel.h>
#include <ntstrsafe.h>

using namespace mimo;

namespace {

    void ResolveFileName(_In_ FLT_CALLBACK_DATA* pData, _In_ const FLT_RELATED_OBJECTS* pFltObjects, _Inout_ UNICODE_STRING* pName) {
        FLT_FILE_NAME_INFORMATION* pNormalizedInfo = nullptr;
        FLT_FILE_NAME_INFORMATION* pOpenedInfo = nullptr;
        NTSTATUS normalizedStatus = STATUS_UNSUCCESSFUL;
        NTSTATUS openedStatus = STATUS_UNSUCCESSFUL;

        if (!pFltObjects->FileObject) {
            RtlUnicodeStringCopyString(pName, L"[no file name]");

            goto done;
        }

        normalizedStatus = FltGetFileNameInformation(pData, FLT_FILE_NAME_NORMALIZED | FLT_FILE_NAME_QUERY_ALWAYS_ALLOW_CACHE_LOOKUP, &pNormalizedInfo);

        if (NT_SUCCESS(normalizedStatus)) {

        #if DBG

            FLT_ASSERT(NT_SUCCESS(FltParseFileNameInformation(pNormalizedInfo)));

        #endif // DBG

            RtlUnicodeStringPrintf(pName, L"%wZ", &pNormalizedInfo->Name);

            goto done;
        }

        openedStatus = FltGetFileNameInformation(pData, FLT_FILE_NAME_OPENED | FLT_FILE_NAME_QUERY_ALWAYS_ALLOW_CACHE_LOOKUP, &pOpenedInfo);

        if (NT_SUCCESS(openedStatus)) {
            RtlUnicodeStringPrintf(pName, L"%wZ [opened: normalize 0x%08X]", &pOpenedInfo->Name, normalizedStatus);

            goto done;
        }

        RtlUnicodeStringPrintf(pName, L"[name unavailable: normalize 0x%08X, opened 0x%08X]", normalizedStatus, openedStatus);

    done:

        if (pNormalizedInfo) {
            FltReleaseFileNameInformation(pNormalizedInfo);
        }

        if (pOpenedInfo) {
            FltReleaseFileNameInformation(pOpenedInfo);
        }

        return;
    }

}

namespace mimo {

    namespace dispatch {

        #pragma warning(suppress: 6262)
        _Use_decl_annotations_
        FLT_PREOP_CALLBACK_STATUS PreOperationCallback(FLT_CALLBACK_DATA* pData, const FLT_RELATED_OBJECTS* pFltObjects, void** ppCompletionContext) {
            records::Entry* const pEntry = records::CreateEntry();

            if (!pEntry) return FLT_PREOP_SUCCESS_NO_CALLBACK;

            WCHAR nameBuffer[protocol::NAME_WCHARS]{};
            UNICODE_STRING fileName{};
            RtlInitEmptyUnicodeString(&fileName, nameBuffer, sizeof(nameBuffer));
            ResolveFileName(pData, pFltObjects, &fileName);

            WCHAR ecpDataBuffer[protocol::NAME_WCHARS]{};
            UNICODE_STRING ecpData{};
            RtlInitEmptyUnicodeString(&ecpData, ecpDataBuffer, sizeof(ecpDataBuffer));

            if (pData->Iopb->MajorFunction == IRP_MJ_CREATE) {
                ecp::FormatEcps(pData, &ecpData);
            }

            capture::PopulatePreOperationRecordData(&pEntry->record.data, pData, pFltObjects, &fileName, &ecpData);

            if (pData->Iopb->MajorFunction == IRP_MJ_SHUTDOWN) {
                PostOperationCallback(pData, pFltObjects, pEntry, 0u);

                return FLT_PREOP_SUCCESS_NO_CALLBACK;
            }

            *ppCompletionContext = pEntry;

            return FLT_PREOP_SUCCESS_WITH_CALLBACK;
        }


        _Use_decl_annotations_
        FLT_POSTOP_CALLBACK_STATUS PostOperationCallback(FLT_CALLBACK_DATA* pData, const FLT_RELATED_OBJECTS* pFltObjects, void* pCompletionContext, FLT_POST_OPERATION_FLAGS flags) {
            records::Entry* const pEntry = reinterpret_cast<records::Entry*>(pCompletionContext);

            if (FlagOn(flags, FLTFL_POST_OPERATION_DRAINING)) {
                records::DeleteEntry(pEntry);

                return FLT_POSTOP_FINISHED_PROCESSING;
            }

            ULONG transactionSequence = 0u;

            if (pFltObjects->Transaction && KeGetCurrentIrql() == PASSIVE_LEVEL) {
                transactionSequence = transaction::Enlist(pFltObjects);
            }

            capture::PopulatePostOperationRecordData(&pEntry->record.data, pData, transactionSequence);
            records::Append(pEntry);

            return FLT_POSTOP_FINISHED_PROCESSING;
        }

    }

}
