#pragma once

#include <fltKernel.h>

namespace mimo {

    namespace memory {

        bool IsRawBufferReadable(
            _In_ const FLT_CALLBACK_DATA* pData,
            _In_opt_ const void* pRawBuffer,
            _In_ ULONG size,
            _In_ bool probed = true
        );

        const void* GetReadableBuffer(
            _In_ const FLT_CALLBACK_DATA* pData,
            _In_opt_ MDL* pMdl,
            _In_opt_ const void* pRawBuffer,
            _Inout_ ULONG* pBufferSize,
            _In_ bool probed = true
        );

        bool CopyReadableBuffer(
            _Out_writes_bytes_(size) void* pBuffer,
            _In_reads_bytes_(size) const void* pReadableBuffer,
            _In_ ULONG size
        );

    }

}
