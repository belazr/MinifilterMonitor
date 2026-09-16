#pragma once

#include "..\..\..\..\inc\protocol.h"

#include <fltKernel.h>

namespace mimo {

    namespace trace {

        namespace supplement {

            namespace directory {

                __declspec(code_seg("PAGE"))
                void PopulateFileName(_Inout_ protocol::QueryDirectorySupplement* pSupplement, _In_ const FLT_CALLBACK_DATA* pData);

                __declspec(code_seg("PAGE"))
                void PopulateQuery(_Inout_ protocol::QueryDirectorySupplement* pSupplement, _In_ const FLT_CALLBACK_DATA* pData);

                __declspec(code_seg("PAGE"))
                void PopulateNotify(_Inout_ protocol::NotifyDirectorySupplement* pSupplement, _In_ const FLT_CALLBACK_DATA* pData);

            }

        }

    }

}
