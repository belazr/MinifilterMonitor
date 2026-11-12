#include "error.h"

#include "text.h"

#include <Windows.h>

#include <array>
#include <cstdint>
#include <format>
#include <iostream>
#include <string>
#include <string_view>

namespace mimo {

    namespace error {

        void Display(DWORD code) {
            constexpr uint32_t MESSAGE_WCHAR_COUNT = 1024u;
            std::array<wchar_t, MESSAGE_WCHAR_COUNT> buffer{};
            const DWORD wcharCount = FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, code, 0u, buffer.data(), static_cast<DWORD>(buffer.size()), nullptr);
            const std::wstring_view wideMessage = buffer.data();
            const std::string message = wcharCount ? text::ConvertToUtf8(wideMessage.substr(0u, wideMessage.find_last_not_of(L"\r\n") + 1u)) : "failed to translate error code";
            std::cerr << std::format("Error: {}, {}\n", code, message);

            return;
        }

    }

}
