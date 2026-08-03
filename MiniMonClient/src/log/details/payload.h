#pragma once

#include "..\..\text.h"

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <span>
#include <string>

namespace mimo {

    namespace log {

        namespace details {

            namespace payload {

                template <typename T>
                bool ReadValue(
                    std::span<const uint8_t> payload,
                    T& value,
                    size_t offset = 0u
                ) {

                    if (payload.size() < offset + sizeof(T)) return false;

                    std::memcpy(&value, payload.data() + offset, sizeof(T));

                    return true;
                }


                inline std::wstring RenderFileName(std::span<const uint8_t> nameData, uint32_t fileNameLength) {
                    const size_t nameBytes = fileNameLength < nameData.size() ? fileNameLength : nameData.size();

                    std::wstring name(nameBytes / sizeof(wchar_t), L'\0');
                    std::memcpy(name.data(), nameData.data(), name.size() * sizeof(wchar_t));

                    return text::MarkTruncated(name, nameBytes < fileNameLength);
                }

            }

        }

    }

}
