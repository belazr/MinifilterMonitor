#pragma once

#include "buffer.h"

#include "..\..\inc\protocol.h"

#include <cstddef>
#include <optional>
#include <span>

namespace mimo {

    namespace records {

        static_assert(alignof(protocol::Record) <= alignof(std::max_align_t), "AlignedBuffer storage cannot satisfy protocol::Record alignment");

        inline std::optional<std::span<const protocol::Record>> Parse(const AlignedBuffer& buffer) {
            constexpr uint32_t RECORD_BYTES = static_cast<uint32_t>(sizeof(protocol::Record));

            if (buffer.Size() % RECORD_BYTES != 0u) return std::nullopt;

            const protocol::Record* const pRecords = reinterpret_cast<const protocol::Record*>(buffer.Data());

            return std::span<const protocol::Record>{ pRecords, buffer.Size() / RECORD_BYTES };
        }

    }

}
