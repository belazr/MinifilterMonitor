#pragma once

#include <fltKernel.h>

namespace mimo {

    namespace name {

        NTSTATUS FormatFileName(
            _In_ FLT_CALLBACK_DATA* pData,
            _In_ const FLT_RELATED_OBJECTS* pFltObjects,
            _Inout_ UNICODE_STRING* pName
        );

        __declspec(code_seg("PAGE"))
        NTSTATUS FormatTargetFileName(
            _In_ FLT_CALLBACK_DATA* pData,
            _In_ const FLT_RELATED_OBJECTS* pFltObjects,
            _Inout_ UNICODE_STRING* pName
        );

    }

}
