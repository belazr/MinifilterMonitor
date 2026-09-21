#pragma once

#include "..\..\..\..\inc\protocol.h"

#include <fltKernel.h>

namespace mimo {

    namespace trace {

        namespace supplement {

            namespace directory {

                void PopulateFileName(_Inout_ protocol::QueryDirectorySupplement* pSupplement, _In_ const FLT_CALLBACK_DATA* pData);

                void PopulateQuery(_Inout_ protocol::QueryDirectorySupplement* pSupplement, _In_ const FLT_CALLBACK_DATA* pData);

                void PopulateNotify(_Inout_ protocol::NotifyDirectorySupplement* pSupplement, _In_ const FLT_CALLBACK_DATA* pData);

            }

        }

    }

}
