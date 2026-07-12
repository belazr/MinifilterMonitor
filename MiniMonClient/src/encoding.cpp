#include "encoding.h"

#include <Windows.h>

namespace mimo {

    namespace encoding {

        std::string ConvertToUtf8(std::wstring_view text) {

            if (text.empty()) return {};

            const int length = WideCharToMultiByte(CP_UTF8, 0u, text.data(), static_cast<int>(text.size()), nullptr, 0, nullptr, nullptr);

            if (length <= 0) return {};

            std::string utf8(static_cast<size_t>(length), '\0');
            WideCharToMultiByte(CP_UTF8, 0u, text.data(), static_cast<int>(text.size()), utf8.data(), length, nullptr, nullptr);

            return utf8;
        }

    }

}
