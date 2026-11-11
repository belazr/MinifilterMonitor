#pragma once

#include "..\test.h"

#include <string_view>

namespace mimo {

    namespace operations {

        namespace create {

            void Run(std::wstring_view ntDirectory, test::Counts& counts);

        }

    }

}
