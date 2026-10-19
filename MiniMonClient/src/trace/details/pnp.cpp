#include "pnp.h"

#include "..\kernel.h"
#include "..\names.h"
#include "..\values.h"

#include "..\..\..\..\inc\protocol.h"

#include <format>
#include <string>

using namespace mimo;

namespace {

    std::wstring RenderQueryDeviceRelations(const protocol::RecordData& data) {

        return std::format(L"Type: {}", trace::names::RenderDeviceRelationType(data.parameters.pnp.queryDeviceRelations.type));
    }


    std::wstring RenderDeviceUsageNotification(const protocol::RecordData& data) {
        const protocol::FltParameters& parameters = data.parameters;

        return std::format(L"InPath: {}, Type: {}", trace::values::RenderBoolean(parameters.pnp.usageNotification.inPath), trace::names::RenderDeviceUsageNotificationType(parameters.pnp.usageNotification.type));
    }

}

namespace mimo {

    namespace trace {

        namespace details {

            namespace pnp {

                std::wstring Render(const protocol::RecordData& data) {

                    switch (data.callbackMinorId) {

                        case kernel::IRP_MN_QUERY_DEVICE_RELATIONS:

                            return RenderQueryDeviceRelations(data);

                        case kernel::IRP_MN_DEVICE_USAGE_NOTIFICATION:

                            return RenderDeviceUsageNotification(data);

                    }

                    return {};
                }

            }

        }

    }

}
