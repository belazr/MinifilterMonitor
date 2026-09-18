#include "flush.h"

#include "..\names.h"

#include "..\..\..\..\inc\protocol.h"

#include <string>

namespace mimo {

    namespace trace {

        namespace details {

            namespace flush {

                std::wstring Render(const protocol::RecordData& data) {

                    return names::RenderFlushFlags(data.operationFlags);
                }

            }

        }

    }

}
