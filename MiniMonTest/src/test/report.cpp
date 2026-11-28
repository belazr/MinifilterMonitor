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

        void Report::Count(NTSTATUS expectedStatus, std::optional<NTSTATUS> returnedStatus) {
            this->operations++;

            if (!returnedStatus.has_value()) {
                this->notExecuted++;

                return;
            }

            if (*returnedStatus != expectedStatus) {
                this->unexpected++;
            }

            return;
        }


        void Report::Print(const Description& description, std::optional<NTSTATUS> returnedStatus) {
            this->Count(description.expectedStatus, returnedStatus);
            const std::string name = text::ConvertToUtf8(description.name);

            if (!returnedStatus.has_value()) {
                std::cout << std::format("{} {}: not executed, expected 0x{:08X}\n", description.major, name, static_cast<uint32_t>(description.expectedStatus));

                return;
            }

            std::cout << std::format("{} {}: status 0x{:08X}, expected 0x{:08X}\n", description.major, name, static_cast<uint32_t>(*returnedStatus), static_cast<uint32_t>(description.expectedStatus));

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
