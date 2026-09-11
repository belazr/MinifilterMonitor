#include "wmi.h"

#include "..\values.h"

#include "..\..\..\..\inc\protocol.h"

#include <format>
#include <string>

namespace mimo {

    namespace trace {

        namespace details {

            namespace wmi {

                std::wstring Render(const protocol::RecordData& data) {
                    const protocol::FltParameters& parameters = data.parameters;
                    std::wstring details = std::format(L"ProviderId: 0x{:X}, BufferSize: {}", parameters.wmi.providerId, parameters.wmi.bufferSize);

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
