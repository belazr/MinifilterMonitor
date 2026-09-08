#pragma once

#include "..\..\..\..\inc\protocol.h"

#include <fltKernel.h>

namespace mimo {

    namespace trace {

        namespace supplement {

            namespace quota {

                __declspec(code_seg("PAGE"))
                void PopulateList(_Inout_ protocol::QueryQuotaSupplement* pSupplement, _In_ const FLT_CALLBACK_DATA* pData);

                __declspec(code_seg("PAGE"))
                void PopulateSet(_Inout_ protocol::SetQuotaSupplement* pSupplement, _In_ const FLT_CALLBACK_DATA* pData);

                __declspec(code_seg("PAGE"))
                void PopulateQuery(_Inout_ protocol::QueryQuotaSupplement* pSupplement, _In_ const FLT_CALLBACK_DATA* pData);

            }

        }

    }

}
