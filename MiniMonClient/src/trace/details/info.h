#pragma once

#include "..\..\..\..\inc\protocol.h"

#include <string>

namespace mimo {

    namespace trace {

        namespace details {

            namespace info {

                std::wstring RenderQuery(const protocol::RecordData& data);

                std::wstring RenderSet(const protocol::RecordData& data);

            }

        }

    }

}
