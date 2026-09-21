#pragma once

#include "..\..\..\..\inc\protocol.h"

#include <fltKernel.h>

namespace mimo {

    namespace trace {

        namespace supplement {

            namespace readwrite {

                void PopulateWrite(_Inout_ protocol::ReadWriteSupplement* pSupplement, _In_ const FLT_CALLBACK_DATA* pData);

                void PopulateRead(_Inout_ protocol::ReadWriteSupplement* pSupplement, _In_ const FLT_CALLBACK_DATA* pData);

            }

        }

    }

}
