#pragma once

#include <fltKernel.h>

namespace mimo {

    namespace ecp {

        __declspec(code_seg("PAGE"))
        void FormatEcps(_In_ FLT_CALLBACK_DATA* pData, _Inout_ UNICODE_STRING* pEcpString);

    }

}
