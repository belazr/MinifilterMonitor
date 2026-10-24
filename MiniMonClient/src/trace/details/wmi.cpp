#include "wmi.h"

#include "..\values.h"

#include "..\..\..\..\inc\protocol.h"

#include <cstdint>
#include <format>
#include <string>

using namespace mimo;

namespace {

    std::wstring RenderParameters(uint64_t providerId, uint32_t size) {

        return std::format(L"ProviderId: 0x{:X}, BufferSize: {}", providerId, size);
    }

}

namespace mimo {

    namespace trace {

        namespace details {

            namespace wmi {

                std::wstring Render(const protocol::RecordData& data) {
                    const protocol::FltParameters& parameters = data.parameters;
                    std::wstring details = RenderParameters(parameters.wmi.providerId, parameters.wmi.bufferSize);

                    const protocol::WmiSupplement& wmiSupplement = data.supplement.wmi;

                    if (wmiSupplement.captured & protocol::WMI_CAPTURED_GUID) {
                        details += std::format(L", Guid: {}", values::RenderGuid(wmiSupplement.guid));
                    }

                    return details;
                }

            }

        }

    }

}
