#pragma once

#include "..\..\..\inc\protocol.h"

#include <string>

namespace mimo {

    namespace trace {

        namespace format {

            std::wstring GetHeader();

            std::wstring Render(const protocol::Record& record);

        }

    }

}
