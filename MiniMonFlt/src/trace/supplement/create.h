#pragma once

#include "..\..\..\..\inc\protocol.h"

#include <fltKernel.h>

namespace mimo {

    namespace trace {

        namespace supplement {

            namespace create {

                __declspec(code_seg("PAGE"))
                void Populate(_Inout_ protocol::CreateSupplement* pSupplement, _In_ FLT_CALLBACK_DATA* pData);

            }

        }

    }

}
