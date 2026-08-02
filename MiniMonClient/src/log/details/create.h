#pragma once

#include "..\..\..\..\inc\protocol.h"

#include <string>

namespace mimo {

    namespace log {

        namespace details {

            namespace create {

                std::wstring Render(const protocol::RecordData& data);

            }

        }

    }

}
