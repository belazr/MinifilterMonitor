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


        inline std::wstring MarkTruncated(std::wstring_view value, bool truncated) {
            constexpr std::wstring_view TRUNCATION_MARK = L"...";

            std::wstring result{ value };

            if (truncated) {
                result += TRUNCATION_MARK;
            }

            return result;
        }


        std::string ConvertToUtf8(std::wstring_view text);

    }

}
