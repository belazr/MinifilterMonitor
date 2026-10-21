#pragma once

#include <cstdint>
#include <span>
#include <string>

namespace mimo {

    namespace trace {

        namespace eas {

            std::wstring RenderEasPayload(std::span<const uint8_t> payload);

        }

    }

}
