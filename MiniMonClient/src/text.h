#pragma once

#include <cstdint>
#include <span>
#include <string>
#include <string_view>

namespace mimo {

    namespace text {

        constexpr std::wstring_view Extract(std::span<const wchar_t> buffer) {
            const std::wstring_view view{ buffer.data(), buffer.size() };

            return view.substr(0u, view.find(L'\0'));
        }


        constexpr std::wstring ConvertFromAscii(std::string_view text) {
            constexpr std::wstring_view HEX_DIGITS = L"0123456789ABCDEF";

            std::wstring result;

            for (const char character : text) {
                const uint8_t byte = static_cast<uint8_t>(character);

                if (byte < 0x80u) {
                    result.push_back(static_cast<wchar_t>(byte));
                }
                else {
                    result += L"\\x";
                    result.push_back(HEX_DIGITS[byte >> 4]);
                    result.push_back(HEX_DIGITS[byte & 0x0Fu]);
                }

            }

            return result;
        }


        constexpr std::wstring MarkTruncated(std::wstring_view value, bool truncated) {
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
