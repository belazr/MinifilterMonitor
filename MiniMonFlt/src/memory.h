#pragma once

#include <fltKernel.h>

namespace mimo {

    namespace memory {

        const void* MapMdl(
            _In_opt_ MDL* pMdl,
            _In_opt_ const void* pExpectedAddress,
            _Inout_ ULONG* pBufferSize
        );

        bool IsRawBufferReadable(
            _In_ const FLT_CALLBACK_DATA* pData,
            _In_opt_ const void* pRawBuffer,
            _In_ ULONG bufferSize
        );

        const void* GetReadableBuffer(
            _In_ const FLT_CALLBACK_DATA* pData,
            _In_opt_ MDL* pMdl,
            _In_opt_ const void* pRawBuffer,
            _Inout_ ULONG* pBufferSize
        );

    }

}
