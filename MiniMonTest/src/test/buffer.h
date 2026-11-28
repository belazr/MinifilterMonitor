#pragma once

#include <cstddef>
#include <cstring>
#include <vector>

namespace mimo {

    namespace test {

        template <typename T>
        std::vector<std::byte> MakeBuffer(const T& payload) {
            std::vector<std::byte> buffer(sizeof(payload));
            std::memcpy(buffer.data(), &payload, sizeof(payload));

            return buffer;
        }

    }

}
