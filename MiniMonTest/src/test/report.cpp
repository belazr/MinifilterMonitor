#include "report.h"

#include "..\test.h"
#include "..\text.h"

#include <Windows.h>
#include <winternl.h>

#include <cstdint>
#include <format>
#include <iostream>
#include <optional>
#include <string>

namespace mimo {

    namespace test {

        void Report::Add(const Description& description, std::optional<NTSTATUS> status) {
            const std::string name = text::ConvertToUtf8(description.name);
            this->operations++;

            if (!status.has_value()) {
                std::cerr << std::format("{} {}: not executed, expected 0x{:08X}\n", description.major, name, static_cast<uint32_t>(description.expectedStatus));
                this->notExecuted++;

                return;
            }

            const std::string line = std::format("{} {}: status 0x{:08X}, expected 0x{:08X}\n", description.major, name, static_cast<uint32_t>(*status), static_cast<uint32_t>(description.expectedStatus));

            if (*status == description.expectedStatus) {
                std::cout << line;
            }
            else {
                std::cerr << line;
                this->unexpected++;
            }

            return;
        }


        void Report::PrintSummary() const {
            std::cout << std::format("Operations: {}, not executed: {}, unexpected: {}\n", this->operations, this->notExecuted, this->unexpected);

            return;
        }


        bool Report::Succeeded() const {

            return !this->notExecuted && !this->unexpected;
        }

    }

}
