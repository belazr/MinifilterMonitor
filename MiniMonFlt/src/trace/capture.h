#pragma once

#include "..\..\..\inc\protocol.h"

#include <fltKernel.h>

namespace mimo {

    namespace trace {

        namespace capture {

            void PopulatePreOperationRecordData(
                _Inout_ protocol::RecordData* pRecordData,
                _In_ FLT_CALLBACK_DATA* pData,
                _In_ const FLT_RELATED_OBJECTS* pFltObjects
            );

            void PopulatePostOperationRecordData(
                _Inout_ protocol::RecordData* pRecordData,
                _In_ const FLT_CALLBACK_DATA* pData,
                _In_ ULONG transactionSequence
            );

            void PopulateTransactionEventRecordData(
                _Inout_ protocol::RecordData* pRecordData,
                _In_ const FLT_RELATED_OBJECTS* pFltObjects,
                _In_ ULONG notificationMask,
                _In_ ULONG transactionSequence
            );

        }

    }

}
