#pragma once

#include "..\..\..\inc\protocol.h"

#include <string>

namespace mimo {

    namespace log {

        namespace format {

            std::wstring GetHeader();

            std::wstring Render(const protocol::Record& record);

        }

    }

}
