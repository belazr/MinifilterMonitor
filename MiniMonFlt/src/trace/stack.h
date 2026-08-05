#pragma once

#include "..\..\..\inc\protocol.h"

#include <fltKernel.h>

namespace mimo {

    namespace trace {

        namespace stack {

            void CaptureStackTrace(
                _Out_writes_to_(capacity, *pCount) protocol::StackFrame* pFrames,
                _In_ ULONG capacity,
                _Out_ ULONG* pCount
            );

        }

    }

}
