#pragma once

#include "..\..\..\..\inc\protocol.h"

#include <string>

namespace mimo {

    namespace trace {

        namespace details {

            namespace pnp {

                std::wstring Render(const protocol::RecordData& data);

            }

        }

    }

}
