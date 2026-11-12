#include "test.h"

#include "text.h"

#include <cstdint>
#include <format>
#include <iostream>
#include <optional>
#include <string>
#include <string_view>

namespace mimo {

    namespace test {

        void Report(
            Counts& counts,
            std::string_view major,
            const Description& description,
            std::optional<NTSTATUS> status
        ) {
            const std::string name = text::ConvertToUtf8(description.name);
            counts.operations++;

            if (!status.has_value()) {
                std::cerr << std::format("{} {}: not executed, expected 0x{:08X}\n", major, name, static_cast<uint32_t>(description.expectedStatus));
                counts.notExecuted++;

                return;
            }

            const std::string line = std::format("{} {}: status 0x{:08X}, expected 0x{:08X}\n", major, name, static_cast<uint32_t>(*status), static_cast<uint32_t>(description.expectedStatus));

            if (*status == description.expectedStatus) {
                std::cout << line;
            }
            else {
                std::cerr << line;
                counts.unexpected++;
            }

            return;
        }

    }

}
