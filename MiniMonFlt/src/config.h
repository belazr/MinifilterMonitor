#pragma once

#include <fltKernel.h>

namespace mimo {

    namespace config {

        __declspec(code_seg("INIT"))
        NTSTATUS Create(_In_ UNICODE_STRING* pRegistryPath);

        void Delete();

        ULONG GetMaxMemoryKb();

        __declspec(code_seg("PAGE"))
        ULONG GetInstanceAltitude(_In_ const FLT_RELATED_OBJECTS* pFltObjects);

    }

}
