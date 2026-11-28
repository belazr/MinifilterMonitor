#pragma once

#include "..\report.h"

#include <string_view>

namespace mimo {

    namespace test {

        namespace operations {

            namespace deviceio {

                void Run(std::wstring_view ntDirectory, report::Counts& counts);

            }

        }

    }

}
