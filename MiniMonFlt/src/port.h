#pragma once

#include <fltKernel.h>

namespace mimo {

    namespace port {

        __declspec(code_seg("INIT"))
        NTSTATUS Open(_In_ PFLT_FILTER pFilter);

        void Close();

        __declspec(code_seg("PAGE"))
        NTSTATUS ConnectNotify(
            _In_ PFLT_PORT pNewClientPort,
            _In_ void* pServerPortCookie,
            _In_reads_bytes_(sizeOfContext) void* pConnectionContext,
            _In_ ULONG sizeOfContext,
            _Flt_ConnectionCookie_Outptr_ void** ppConnectionCookie
        );

        __declspec(code_seg("PAGE"))
        void DisconnectNotify(_In_opt_ void* pConnectionCookie);

        __declspec(code_seg("PAGE"))
        NTSTATUS MessageNotify(
            _In_ void* pConnectionCookie,
            _In_reads_bytes_opt_(inputSize) void* pInputBuffer,
            _In_ ULONG inputSize,
            _Out_writes_bytes_to_opt_(outputSize, *pBytesWritten) void* pOutputBuffer,
            _In_ ULONG outputSize,
            _Out_ ULONG* pBytesWritten
        );

    }

}
