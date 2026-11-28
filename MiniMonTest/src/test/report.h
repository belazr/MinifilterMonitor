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
            void Add(const Description& description, std::optional<NTSTATUS> status);

            void PrintSummary() const;

            bool Succeeded() const;

        private:
            uint32_t operations = 0u;
            uint32_t notExecuted = 0u;
            uint32_t unexpected = 0u;
        };

    }

}
