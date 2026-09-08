#pragma once

#include "..\..\text.h"

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <span>
#include <string>

namespace mimo {

    namespace trace {

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


                template <typename T>
                bool ReadHeader(
                    std::span<const uint8_t> payload,
                    T& value,
                    size_t headerSize,
                    size_t offset = 0u
                ) {

                    if (payload.size() < offset + headerSize) return false;

                    std::memset(&value, 0, sizeof(value));
                    std::memcpy(&value, payload.data() + offset, headerSize);

                    return true;
                }


                inline std::wstring RenderName(std::span<const uint8_t> nameData, uint32_t nameSize) {
                    const size_t copySize = nameSize < nameData.size() ? nameSize : nameData.size();

                    std::wstring name(copySize / sizeof(wchar_t), L'\0');
                    std::memcpy(name.data(), nameData.data(), name.size() * sizeof(wchar_t));

                    return text::MarkTruncated(name, copySize < nameSize);
                }

            }

        }

    }

}
