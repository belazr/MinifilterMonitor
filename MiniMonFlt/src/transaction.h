#pragma once

#include <fltKernel.h>

namespace mimo {

    namespace transaction {

        struct Context {
            ULONG sequence;
        };

        __declspec(code_seg("PAGE"))
        ULONG Enlist(_In_ const FLT_RELATED_OBJECTS* pFltObjects);

        __declspec(code_seg("PAGE"))
        NTSTATUS HandleNotification(
            _In_ const FLT_RELATED_OBJECTS* pFltObjects,
            _In_ PFLT_CONTEXT pContext,
            _In_ ULONG notificationMask
        );

    }

}
