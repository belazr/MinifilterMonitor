#pragma once

#include "..\..\..\..\inc\protocol.h"

#include <fltKernel.h>

namespace mimo {

    namespace trace {

        namespace supplement {

            namespace lock {

                __declspec(code_seg("PAGE"))
                void Populate(_Inout_ protocol::LockControlSupplement* pSupplement, _In_ const FLT_CALLBACK_DATA* pData);

            }

        }

    }

}
