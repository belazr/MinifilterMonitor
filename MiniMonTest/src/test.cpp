#include "test.h"

#include "text.h"

#include <cstdint>
#include <format>
#include <iostream>
#include <string>
#include <string_view>

namespace mimo {

    namespace test {

        void Report(
            Counts& counts,
            std::string_view major,
            const Description& description,
            const Result& result
        ) {
            const std::string line = std::format("{} {}: status 0x{:08X}, expected 0x{:08X}\n", major, text::ConvertToUtf8(description.name), static_cast<uint32_t>(result.status), static_cast<uint32_t>(description.expectedStatus));
            counts.operations++;

            if (result.status == description.expectedStatus) {
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
