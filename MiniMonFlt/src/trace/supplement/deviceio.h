#pragma once

#include "..\..\..\..\inc\protocol.h"

#include <fltKernel.h>

namespace mimo {

    namespace trace {

        namespace supplement {

            namespace deviceio {

                void PopulateInput(_Inout_ protocol::DeviceIoControlSupplement* pSupplement, _In_ const FLT_CALLBACK_DATA* pData);

                void PopulateOutput(_Inout_ protocol::DeviceIoControlSupplement* pSupplement, _In_ const FLT_CALLBACK_DATA* pData);

            }

        }

    }

}
