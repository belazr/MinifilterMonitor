#pragma once

#include "..\..\inc\protocol.h"

#include <fltKernel.h>

namespace mimo {

    namespace records {

        struct Entry {
            LIST_ENTRY list;
            protocol::Record record;
        };

        __declspec(code_seg("INIT"))
        void Create();

        Entry* CreateEntry();

        void DeleteEntry(_In_ Entry* pEntry);

        void Append(_Inout_ Entry* pEntry);

        NTSTATUS Drain(
            _Out_writes_bytes_to_(size, *pBytesWritten) UCHAR* pBuffer,
            _In_ ULONG size,
            _Out_ ULONG* pBytesWritten
        );

        void Delete();

    }

}
