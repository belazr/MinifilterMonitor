#pragma once

#include "..\..\..\..\inc\protocol.h"

#include <string>

namespace mimo {

    namespace trace {

        namespace details {

            namespace readwrite {

                std::wstring RenderCopy(const protocol::RecordData& data);

                std::wstring RenderMdl(const protocol::RecordData& data);

                std::wstring RenderMdlReadComplete(const protocol::RecordData& data);

                std::wstring RenderMdlWriteComplete(const protocol::RecordData& data);

            }

        }

    }

}
