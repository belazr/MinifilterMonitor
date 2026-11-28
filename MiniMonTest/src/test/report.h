#pragma once

#include "..\test.h"

#include <Windows.h>
#include <winternl.h>

#include <cstdint>
#include <optional>

namespace mimo {

    namespace test {

        class Report final {
        public:
            void Count(NTSTATUS expectedStatus, std::optional<NTSTATUS> returnedStatus);

            void Print(const Description& description, std::optional<NTSTATUS> returnedStatus);

            void PrintSummary() const;

            bool Succeeded() const;

        private:
            uint64_t operations = 0u;
            uint64_t notExecuted = 0u;
            uint64_t unexpected = 0u;
        };

    }

}
