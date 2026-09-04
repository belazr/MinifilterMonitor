#pragma once

#include "..\..\..\..\inc\protocol.h"

#include <string>

namespace mimo {

    namespace trace {

        namespace details {

            namespace fastiocheck {

                std::wstring Render(const protocol::RecordData& data);

            }

        }

    }

}
