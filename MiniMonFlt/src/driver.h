#pragma once

#include <fltKernel.h>

namespace mimo {

    namespace driver {

        inline constexpr ULONG ALTITUDE_WCHAR_COUNT = 16u;

        struct InstanceContext {
            WCHAR altitude[ALTITUDE_WCHAR_COUNT];
            bool altitudeTruncated;
        };

        inline constexpr ULONG POOL_TAG = 'oMiM';

        extern PFLT_FILTER pFilter;

    }

}
