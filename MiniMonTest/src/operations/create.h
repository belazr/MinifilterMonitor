#pragma once

#include "..\test.h"

#include <string_view>

namespace mimo {

    namespace operations {

        namespace create {

            bool Run(std::wstring_view ntDirectory, test::Counts& counts);

        }

    }

}
