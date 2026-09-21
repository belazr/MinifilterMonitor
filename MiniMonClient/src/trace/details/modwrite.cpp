#include "modwrite.h"

#include "..\..\..\..\inc\protocol.h"

#include <format>
#include <string>

namespace mimo {

    namespace trace {

        namespace details {

            namespace modwrite {

                std::wstring RenderAcquire(const protocol::RecordData& data) {
                    const protocol::ModWriteSupplement& modWriteSupplement = data.supplement.modWrite;
                    std::wstring details;

                    if (modWriteSupplement.captured & protocol::MOD_WRITE_CAPTURED_ENDING_OFFSET) {
                        details += std::format(L"EndingOffset: {}, ", modWriteSupplement.endingOffset);
                    }

                    if (modWriteSupplement.captured & protocol::MOD_WRITE_CAPTURED_RESOURCE) {
                        details += std::format(L"Resource: 0x{:X}, ", modWriteSupplement.resource);
                    }

                    if (!details.empty()) {
                        details.resize(details.size() - 2u);
                    }

                    return details;
                }


                std::wstring RenderRelease(const protocol::RecordData& data) {
                    const uint64_t resource = data.parameters.releaseForModifiedPageWriter.resourceToRelease;

                    if (!resource) return {};

                    return std::format(L"Resource: 0x{:X}", resource);
                }

            }

        }

    }

}
