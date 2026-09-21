#pragma once

#include "..\..\..\..\inc\protocol.h"

#include <fltKernel.h>

namespace mimo {

    namespace trace {

        namespace supplement {

            namespace filesystem {

                void PopulateInput(_Inout_ protocol::FsControlSupplement* pSupplement, _In_ const FLT_CALLBACK_DATA* pData);

                void PopulateOutput(_Inout_ protocol::FsControlSupplement* pSupplement, _In_ const FLT_CALLBACK_DATA* pData);

            }

        }

    }

}
