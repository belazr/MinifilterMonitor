#pragma once

#include "..\..\..\inc\protocol.h"

#include <fltKernel.h>

namespace mimo {

    namespace trace {

        namespace modules {

            __declspec(code_seg("INIT"))
            void Create();

            __declspec(code_seg("INIT"))
            NTSTATUS Init();

            void Delete();

            void Resolve(
                _In_reads_(count) const void* const* ppAddresses,
                _In_ ULONG count,
                _Out_writes_(count) protocol::StackFrame* pFrames
            );

        }

    }

}
