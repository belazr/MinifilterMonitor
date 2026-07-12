#pragma once

#include <fltKernel.h>

namespace mimo {

    namespace port {

        __declspec(code_seg("INIT"))
        NTSTATUS Open(_In_ PFLT_FILTER filter);

        void Close();

        __declspec(code_seg("PAGE"))
        NTSTATUS Connect(
            _In_ PFLT_PORT pClientPort,
            _In_ void* pServerPortCookie,
            _In_reads_bytes_(sizeOfContext) void* pConnectionContext,
            _In_ ULONG sizeOfContext,
            _Flt_ConnectionCookie_Outptr_ void** ppConnectionCookie
        );

        __declspec(code_seg("PAGE"))
        void Disconnect(_In_opt_ void* pConnectionCookie);

        __declspec(code_seg("PAGE"))
        NTSTATUS Message(
            _In_ void* pConnectionCookie,
            _In_reads_bytes_opt_(inputSize) void* pInputBuffer,
            _In_ ULONG inputSize,
            _Out_writes_bytes_to_opt_(outputSize, *pBytesWritten) void* pOutputBuffer,
            _In_ ULONG outputSize,
            _Out_ ULONG* pBytesWritten
        );

    }

}
