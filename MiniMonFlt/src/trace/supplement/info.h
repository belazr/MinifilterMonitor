#pragma once

#include "..\..\..\..\inc\protocol.h"

#include <fltKernel.h>

namespace mimo {

    namespace trace {

        namespace supplement {

            namespace info {

                __declspec(code_seg("PAGE"))
                void PopulateSet(
                    _Inout_ protocol::SetInfoSupplement* pSupplement,
                    _In_ FLT_CALLBACK_DATA* pData,
                    _In_ const FLT_RELATED_OBJECTS* pFltObjects
                );

                __declspec(code_seg("PAGE"))
                void PopulateQuery(_Inout_ protocol::QueryInfoSupplement* pSupplement, _In_ const FLT_CALLBACK_DATA* pData);

            }

        }

    }

}
