#include "fastiocheck.h"

#include "..\values.h"

#include "..\..\..\..\inc\protocol.h"

#include <cstdint>
#include <format>
#include <string>

using namespace mimo;

namespace {

    std::wstring RenderParameters(bool checkForReadOperation, int64_t fileOffset, uint32_t size) {

        return std::format(L"Operation: {}, Offset: {}, Length: {}", checkForReadOperation ? L"Read" : L"Write", trace::values::RenderByteOffset(fileOffset), size);
    }

}

namespace mimo {

    namespace trace {

        namespace details {

            namespace fastiocheck {

                std::wstring Render(const protocol::RecordData& data) {
                    const protocol::FltParameters& parameters = data.parameters;
                    std::wstring details = RenderParameters(parameters.fastIoCheckIfPossible.checkForReadOperation, parameters.fastIoCheckIfPossible.fileOffset, parameters.fastIoCheckIfPossible.length);

                    if (parameters.fastIoCheckIfPossible.lockKey) {
                        details += std::format(L", Key: 0x{:X}", parameters.fastIoCheckIfPossible.lockKey);
                    }

                    return details;
                }

            }

        }

    }

}
