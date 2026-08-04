#pragma once

#include "..\..\..\..\inc\protocol.h"

#include <string>

namespace mimo {

    namespace log {

        namespace details {

            namespace volume {

                std::wstring Render(const protocol::RecordData& data);

            }

        }

    }

}
