#include "fastiocheck.h"

#include "..\..\..\..\inc\protocol.h"

#include <format>
#include <string>

namespace mimo {

    namespace trace {

        namespace details {

            namespace fastiocheck {

                std::wstring Render(const protocol::RecordData& data) {
                    const protocol::FltParameters& parameters = data.parameters;
                    std::wstring details = std::format(L"Operation: {}, Offset: {}, Length: {}", parameters.fastIoCheckIfPossible.checkForReadOperation ? L"Read" : L"Write", parameters.fastIoCheckIfPossible.fileOffset, parameters.fastIoCheckIfPossible.length);

                    if (parameters.fastIoCheckIfPossible.lockKey) {
                        details += std::format(L", Key: 0x{:X}", parameters.fastIoCheckIfPossible.lockKey);
                    }

                    return details;
                }

            }

        }

    }

}
