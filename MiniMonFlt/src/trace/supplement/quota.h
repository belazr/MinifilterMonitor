#pragma once

#include "..\..\..\..\inc\protocol.h"

#include <fltKernel.h>

namespace mimo {

    namespace trace {

        namespace supplement {

            namespace quota {

                void PopulateSidList(_Inout_ protocol::QueryQuotaSupplement* pSupplement, _In_ const FLT_CALLBACK_DATA* pData);

                void PopulateSet(_Inout_ protocol::SetQuotaSupplement* pSupplement, _In_ const FLT_CALLBACK_DATA* pData);

                void PopulateQuery(_Inout_ protocol::QueryQuotaSupplement* pSupplement, _In_ const FLT_CALLBACK_DATA* pData);

            }

        }

    }

}
