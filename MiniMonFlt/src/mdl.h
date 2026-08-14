#pragma once

#include <fltKernel.h>

namespace mimo {

    namespace mdl {

        __declspec(code_seg("PAGE"))
        const void* Map(
            _In_opt_ MDL* pMdl,
            _In_opt_ const void* pExpectedAddress,
            _Inout_ ULONG* pBufferSize
        );

    }

}
