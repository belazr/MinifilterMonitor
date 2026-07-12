#pragma once

#include <fltKernel.h>

namespace mimo {

    namespace driver {

        struct InstanceContext {
            ULONG altitude;
        };

        inline constexpr ULONG MEM_TAG = 'oMiM';

        extern PFLT_FILTER Filter;

    }

}
