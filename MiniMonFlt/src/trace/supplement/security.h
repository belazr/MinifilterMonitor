#pragma once

#include "..\..\..\..\inc\protocol.h"

#include <fltKernel.h>

namespace mimo {

    namespace trace {

        namespace supplement {

            namespace security {

                void PopulateSet(_Inout_ protocol::SecuritySupplement* pSupplement, _In_ const FLT_CALLBACK_DATA* pData);

                void PopulateQuery(_Inout_ protocol::SecuritySupplement* pSupplement, _In_ const FLT_CALLBACK_DATA* pData);

            }

        }

    }

}
