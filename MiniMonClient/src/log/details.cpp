#include "details.h"

#include "..\kernel.h"
#include "details\create.h"
#include "details\directory.h"
#include "details\info.h"
#include "details\readwrite.h"

#include "..\..\..\inc\protocol.h"

#include <string>

namespace mimo {

    namespace log {

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

                    case kernel::IRP_MJ_DIRECTORY_CONTROL:

                        return directory::Render(data);

                }

                return {};
            }

        }

    }

}
