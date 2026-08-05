#pragma once

#include "..\..\..\inc\protocol.h"

#include <string>

namespace mimo {

    namespace trace {

        namespace details {

            std::wstring Render(const protocol::RecordData& data);

        }

    }

}
