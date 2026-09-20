#include "pnp.h"

#include "..\kernel.h"
#include "..\names.h"
#include "..\values.h"

#include "..\..\..\..\inc\protocol.h"

#include <format>
#include <string>

namespace mimo {

    namespace trace {

        namespace details {

            namespace pnp {

                std::wstring Render(const protocol::RecordData& data) {
                    const protocol::FltParameters& parameters = data.parameters;

                    switch (data.callbackMinorId) {

                        case kernel::IRP_MN_QUERY_DEVICE_RELATIONS:

                            return std::format(L"Type: {}", names::RenderDeviceRelationType(parameters.pnp.queryDeviceRelations.type));

                        case kernel::IRP_MN_DEVICE_USAGE_NOTIFICATION:

                            return std::format(L"InPath: {}, Type: {}", values::RenderBoolean(parameters.pnp.usageNotification.inPath), names::RenderDeviceUsageNotificationType(parameters.pnp.usageNotification.type));

                    }

                    return {};
                }

            }

        }

    }

}
