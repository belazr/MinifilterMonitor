#pragma once

#include <string>
#include <string_view>

namespace mimo {

    namespace text {

        std::string ConvertToUtf8(std::wstring_view text);

    }

}
