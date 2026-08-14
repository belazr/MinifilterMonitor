#pragma once

#include <fltKernel.h>

namespace mimo {

    namespace config {

        __declspec(code_seg("INIT"))
        void Create(_In_ DRIVER_OBJECT* pDriverObject);

        ULONG GetMaxMemoryKb();

    }

}
