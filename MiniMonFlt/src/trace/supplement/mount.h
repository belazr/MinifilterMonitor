#pragma once

#include "..\..\..\..\inc\protocol.h"

#include <fltKernel.h>

namespace mimo {

    namespace trace {

        namespace supplement {

            namespace mount {

                __declspec(code_seg("PAGE"))
                void Populate(_Inout_ protocol::MountSupplement* pSupplement, _In_ const FLT_RELATED_OBJECTS* pFltObjects);

            }

        }

    }

}
