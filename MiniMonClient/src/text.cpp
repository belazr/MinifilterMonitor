#include "text.h"

#include <Windows.h>

namespace mimo {

    namespace text {

        std::string ConvertToUtf8(std::wstring_view text) {

            if (text.empty()) return {};

            const int utf8Size = WideCharToMultiByte(CP_UTF8, 0u, text.data(), static_cast<int>(text.size()), nullptr, 0, nullptr, nullptr);

            if (utf8Size <= 0) return {};

            std::string utf8(static_cast<size_t>(utf8Size), '\0');
            WideCharToMultiByte(CP_UTF8, 0u, text.data(), static_cast<int>(text.size()), utf8.data(), utf8Size, nullptr, nullptr);

            return utf8;
        }

    }

}
