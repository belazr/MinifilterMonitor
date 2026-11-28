#include "operations.h"

#include "operations\create.h"
#include "operations\deviceio.h"
#include "operations\directory.h"
#include "operations\ea.h"
#include "operations\filesystem.h"
#include "operations\flush.h"
#include "operations\info.h"
#include "operations\lock.h"
#include "operations\quota.h"
#include "operations\readwrite.h"
#include "operations\section.h"
#include "operations\security.h"
#include "operations\volume.h"

#include "report.h"

#include <string_view>

namespace mimo {

    namespace test {

        namespace operations {

            bool Run(std::wstring_view ntDirectory) {
                Report report;

                if (!create::Run(ntDirectory, report)) return false;

                readwrite::Run(ntDirectory, report);
                info::Run(ntDirectory, report);
                ea::Run(ntDirectory, report);
                flush::Run(ntDirectory, report);
                volume::Run(ntDirectory, report);
                directory::Run(ntDirectory, report);
                filesystem::Run(ntDirectory, report);
                deviceio::Run(ntDirectory, report);
                lock::Run(ntDirectory, report);
                security::Run(ntDirectory, report);
                quota::Run(ntDirectory, report);
                section::Run(ntDirectory, report);
                report.PrintSummary();

                return report.Succeeded();
            }

        }

    }

}
