#pragma once

#include "..\..\..\..\inc\protocol.h"

#include <fltKernel.h>

namespace mimo {

    namespace trace {

        namespace supplement {

            namespace volume {

                __declspec(code_seg("PAGE"))
                void PopulateSet(_Inout_ protocol::VolumeInfoSupplement* pSupplement, _In_ const FLT_CALLBACK_DATA* pData);

                __declspec(code_seg("PAGE"))
                void PopulateQuery(_Inout_ protocol::VolumeInfoSupplement* pSupplement, _In_ const FLT_CALLBACK_DATA* pData);

            }

        }

    }

}
