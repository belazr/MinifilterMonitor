#pragma once

#include <cstdint>
#include <span>
#include <string>

namespace mimo {

    namespace trace {

        namespace values {

            std::wstring RenderBoolean(bool value);

            std::wstring RenderByteOffset(int64_t byteOffset);

            std::wstring RenderObjectId(uint64_t objectId);

            std::wstring RenderTopLevelIrp(uint64_t topLevelIrp);

            std::wstring RenderFileId(uint64_t fileId);

            std::wstring RenderFileId(std::span<const uint8_t, 16u> fileId);

            std::wstring RenderGuid(std::span<const uint8_t, 16u> guid);

            std::wstring RenderOperationTime(int64_t operationTime);

            std::wstring RenderTime(int64_t time);

            std::wstring RenderSid(std::span<const uint8_t> sidData);

        }

    }

}
