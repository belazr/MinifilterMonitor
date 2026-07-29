#pragma once

#include <span>
#include <string>
#include <string_view>

namespace mimo {

    namespace text {

        inline std::wstring_view Extract(std::span<const wchar_t> buffer) {
            const std::wstring_view view{ buffer.data(), buffer.size() };

            return view.substr(0u, view.find(L'\0'));
        }


        std::string ConvertToUtf8(std::wstring_view text);

    }

}
