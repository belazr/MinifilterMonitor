#include "modwrite.h"

#include "..\..\..\..\inc\protocol.h"

#include <format>
#include <string>

namespace mimo {

    namespace trace {

        namespace details {

            namespace modwrite {

                std::wstring Render(const protocol::RecordData& data) {
                    const protocol::ModWriteSupplement& modWriteSupplement = data.supplement.modWrite;

                    if (!(modWriteSupplement.captured & protocol::MOD_WRITE_CAPTURED_ENDING_OFFSET)) return {};

                    return std::format(L"EndingOffset: {}", modWriteSupplement.endingOffset);
                }

            }

        }

    }

}
