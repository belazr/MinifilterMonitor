#pragma once

#include "..\report.h"

#include <string_view>

namespace mimo {

    namespace test {

        namespace operations {

            namespace volume {

                void Run(std::wstring_view ntDirectory, Report& report);

            }

        }

    }

}
