#pragma once

#include <cstdint>
#include <span>
#include <string>

namespace mimo {

    namespace trace {

        namespace values {

            std::wstring RenderBoolean(bool value);

            std::wstring RenderByteOffset(int64_t offset);

            std::wstring RenderPartitionNumber(uint32_t number);

            std::wstring RenderObjectId(uint64_t id);

            std::wstring RenderTopLevelIrp(uint64_t irp);

            std::wstring RenderFileId(uint64_t id);

            std::wstring RenderFileId(std::span<const uint8_t, 16u> id);

            std::wstring RenderGuid(std::span<const uint8_t, 16u> guid);

            std::wstring RenderOperationTime(int64_t time);

            std::wstring RenderTime(int64_t time);

            std::wstring RenderSid(std::span<const uint8_t> sidData);

            std::wstring RenderBytes(std::span<const uint8_t> bytes, bool truncated);

        }

    }

}
