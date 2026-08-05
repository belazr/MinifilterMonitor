#pragma once

#include "..\..\..\inc\protocol.h"

#include <fltKernel.h>

namespace mimo {

    namespace trace {

        namespace supplement {

            void PopulatePreOperation(
                _Inout_ protocol::Supplement* pSupplement,
                _In_ FLT_CALLBACK_DATA* pData,
                _In_ const FLT_RELATED_OBJECTS* pFltObjects
            );

            void PopulatePostOperation(_Inout_ protocol::Supplement* pSupplement, _In_ const FLT_CALLBACK_DATA* pData);

        }

    }

}
