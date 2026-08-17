#include "details.h"

#include "details\create.h"
#include "details\deviceio.h"
#include "details\directory.h"
#include "details\filesystem.h"
#include "details\info.h"
#include "details\readwrite.h"
#include "details\security.h"
#include "details\volume.h"

#include "kernel.h"

#include "..\..\..\inc\protocol.h"

#include <string>

namespace mimo {

    namespace trace {

        namespace details {

            std::wstring Render(const protocol::RecordData& data) {

                if (data.transactionNotify) return {};

                switch (data.callbackMajorId) {

                    case kernel::IRP_MJ_CREATE:

                        return create::Render(data);

                    case kernel::IRP_MJ_READ:
                    case kernel::IRP_MJ_WRITE:

                        return readwrite::Render(data);

                    case kernel::IRP_MJ_QUERY_INFORMATION:

                        return info::RenderQuery(data);

                    case kernel::IRP_MJ_SET_INFORMATION:

                        return info::RenderSet(data);

                    case kernel::IRP_MJ_QUERY_VOLUME_INFORMATION:
                    case kernel::IRP_MJ_SET_VOLUME_INFORMATION:

                        return volume::Render(data);

                    case kernel::IRP_MJ_DIRECTORY_CONTROL:

                        return directory::Render(data);

                    case kernel::IRP_MJ_FILE_SYSTEM_CONTROL:

                        return filesystem::Render(data);

                    case kernel::IRP_MJ_DEVICE_CONTROL:
                    case kernel::IRP_MJ_INTERNAL_DEVICE_CONTROL:

                        return deviceio::Render(data);

                    case kernel::IRP_MJ_QUERY_SECURITY:

                        return security::RenderQuery(data);

                    case kernel::IRP_MJ_SET_SECURITY:

                        return security::RenderSet(data);

                }

                return {};
            }

        }

    }

}
