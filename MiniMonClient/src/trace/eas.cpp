#include "eas.h"

#include "kernel.h"
#include "names.h"
#include "payload.h"

#include "..\text.h"

#include <cstddef>
#include <cstdint>
#include <format>
#include <span>
#include <string>

namespace mimo {

    namespace trace {

        namespace eas {

            std::wstring RenderEasPayload(std::span<const uint8_t> payload) {

                if (payload.empty()) return {};

                std::wstring result;
                size_t offset = 0u;
                uint32_t index = 1u;
                bool terminated = false;

                while (true) {
                    constexpr size_t NAME_OFFSET = offsetof(kernel::FILE_FULL_EA_INFORMATION, EaName);
                    kernel::FILE_FULL_EA_INFORMATION entry{};

                    if (!payload::ReadHeader(payload, entry, NAME_OFFSET, offset)) break;

                    result += std::format(L"{}: EaName: {}, EaValueLength: {}", index, payload::RenderAsciiName(payload.subspan(offset + NAME_OFFSET), entry.EaNameLength), entry.EaValueLength);
                    index++;

                    if (entry.EaValueLength) {
                        result += std::format(L", EaValue: {}", payload::RenderBytes(payload, offset + NAME_OFFSET + entry.EaNameLength + 1u, entry.EaValueLength));
                    }

                    if (entry.Flags) {
                        result += std::format(L", Flags: {}", names::RenderEaFlags(entry.Flags));
                    }

                    result += L", ";

                    if (!entry.NextEntryOffset) {
                        terminated = true;

                        break;
                    }

                    if (!payload::Advance(payload, offset, entry.NextEntryOffset)) break;

                }

                if (terminated) {
                    result.resize(result.size() - 2u);
                }

                return text::MarkTruncated(result, !terminated);
            }

        }

    }

}
