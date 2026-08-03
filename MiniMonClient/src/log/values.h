#pragma once

#include <cstdint>
#include <string>

namespace mimo {

    namespace log {

        namespace values {

            std::wstring RenderByteOffset(int64_t byteOffset);

            std::wstring RenderFileTime(int64_t fileTime);

        }

    }

}
