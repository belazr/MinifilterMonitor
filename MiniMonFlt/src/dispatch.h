#pragma once

#include <fltKernel.h>

namespace mimo {

    namespace dispatch {

        FLT_PREOP_CALLBACK_STATUS PreOperationCallback(
            _Inout_ FLT_CALLBACK_DATA* pData,
            _In_ const FLT_RELATED_OBJECTS* pFltObjects,
            _Flt_CompletionContext_Outptr_ void** ppCompletionContext
        );

        FLT_POSTOP_CALLBACK_STATUS PostOperationCallback(
            _Inout_ FLT_CALLBACK_DATA* pData,
            _In_ const FLT_RELATED_OBJECTS* pFltObjects,
            _In_ void* pCompletionContext,
            _In_ FLT_POST_OPERATION_FLAGS flags
        );

    }

}
