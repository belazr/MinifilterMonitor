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

#include <format>
#include <iostream>
#include <string_view>

namespace mimo {

    namespace test {

        namespace operations {

            bool Run(std::wstring_view ntDirectory) {
                report::Counts counts{};

                if (!create::Run(ntDirectory, counts)) return false;

                readwrite::Run(ntDirectory, counts);
                info::Run(ntDirectory, counts);
                ea::Run(ntDirectory, counts);
                flush::Run(ntDirectory, counts);
                volume::Run(ntDirectory, counts);
                directory::Run(ntDirectory, counts);
                filesystem::Run(ntDirectory, counts);
                deviceio::Run(ntDirectory, counts);
                lock::Run(ntDirectory, counts);
                security::Run(ntDirectory, counts);
                quota::Run(ntDirectory, counts);
                section::Run(ntDirectory, counts);
                std::cout << std::format("Operations: {}, not executed: {}, unexpected: {}\n", counts.operations, counts.notExecuted, counts.unexpected);

                return !counts.notExecuted && !counts.unexpected;
            }

        }

    }

}
