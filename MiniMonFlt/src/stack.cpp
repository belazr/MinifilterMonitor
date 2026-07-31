#include "stack.h"

#include "modules.h"

#include "..\..\inc\protocol.h"

#include <fltKernel.h>

using namespace mimo;

namespace {

    constexpr ULONG STACK_TRACE_SKIP_FRAMES = 3u;

}

namespace mimo {

    namespace stack {

        _Use_decl_annotations_
        void CaptureStackTrace(
            protocol::StackFrame* pFrames,
            ULONG capacity,
            ULONG* pCount
        ) {
            *pCount = 0u;
            const ULONG limit = capacity < protocol::STACK_TRACE_FRAMES ? capacity : protocol::STACK_TRACE_FRAMES;

            if (limit == 0u) return;

            void* addresses[protocol::STACK_TRACE_FRAMES]{};
            const USHORT captured = RtlCaptureStackBackTrace(STACK_TRACE_SKIP_FRAMES, limit, addresses, nullptr);

            modules::Resolve(addresses, captured, pFrames);

            *pCount = captured;

            return;
        }

    }

}
