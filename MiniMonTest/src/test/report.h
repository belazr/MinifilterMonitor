#pragma once

#include <Windows.h>
#include <winternl.h>

#include <cstdint>
#include <optional>
#include <string_view>

namespace mimo {

    namespace test {

        namespace report {

            struct Description {
                std::wstring_view name;
                NTSTATUS expectedStatus;
            };

            struct Counts {
                uint32_t operations = 0u;
                uint32_t notExecuted = 0u;
                uint32_t unexpected = 0u;
            };

            void Add(
                Counts& counts,
                std::string_view major,
                const Description& description,
                std::optional<NTSTATUS> status
            );

        }

    }

}
