#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

namespace mimo {

    class AlignedBuffer final {
        using Block = std::max_align_t;
        std::vector<Block> buffer;

    public:
        explicit AlignedBuffer(uint32_t size) : buffer((size + sizeof(Block) - 1u) / sizeof(Block)) {}


        uint8_t* Data() {

            return reinterpret_cast<uint8_t*>(this->buffer.data());
        }


        const uint8_t* Data() const {

            return reinterpret_cast<const uint8_t*>(this->buffer.data());
        }


        uint32_t Size() const {

            return static_cast<uint32_t>(this->buffer.size() * sizeof(Block));
        }


        void Resize(uint32_t size) {
            this->buffer.resize((size + sizeof(Block) - 1u) / sizeof(Block));

            return;
        }

    };

}
