#pragma once

#include <fltKernel.h>

namespace mimo {

    namespace memory {

        __declspec(code_seg("PAGE"))
        const void* MapMdl(
            _In_opt_ MDL* pMdl,
            _In_opt_ const void* pExpectedAddress,
            _Inout_ ULONG* pBufferSize
        );

        __declspec(code_seg("PAGE"))
        bool IsRawBufferReadable(
            _In_ const FLT_CALLBACK_DATA* pData,
            _In_ const void* pRawBuffer,
            _In_ ULONG bufferSize
        );

    }

}
