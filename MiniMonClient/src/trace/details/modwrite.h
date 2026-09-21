#pragma once

#include "..\..\..\..\inc\protocol.h"

#include <string>

namespace mimo {

    namespace trace {

        namespace details {

            namespace modwrite {

                std::wstring RenderAcquire(const protocol::RecordData& data);

                std::wstring RenderRelease(const protocol::RecordData& data);

            }

        }

    }

}
