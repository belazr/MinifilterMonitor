#pragma once

#include <Windows.h>
#include <winternl.h>

#include <cstdint>
#include <string_view>

namespace mimo {

    namespace test {

        struct Description {
            std::wstring_view name;
            NTSTATUS expectedStatus;
        };

        struct Result {
            NTSTATUS status;
        };

        struct Counts {
            uint32_t operations = 0u;
            uint32_t unexpected = 0u;
        };

        void Report(
            Counts& counts,
            std::string_view major,
            const Description& description,
            const Result& result
        );

    }

}
