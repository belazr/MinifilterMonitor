#include "security.h"

#include "..\names.h"

#include "..\..\..\..\inc\protocol.h"

#include <Windows.h>
#include <sddl.h>

#include <cstdint>
#include <format>
#include <span>
#include <string>

using namespace mimo;

namespace {

    std::span<const uint8_t> ExtractPayload(const protocol::SecuritySupplement& supplement) {

        if (!(supplement.captured & protocol::SECURITY_CAPTURED_PAYLOAD)) return {};

        return { supplement.payload, supplement.capturedBytes };
    }


    std::wstring RenderDescriptor(std::span<const uint8_t> payload, uint32_t securityInformation) {

        if (payload.empty()) return {};

        const PSECURITY_DESCRIPTOR pSecurityDescriptor = const_cast<uint8_t*>(payload.data());
        wchar_t* pText = nullptr;

        if (!ConvertSecurityDescriptorToStringSecurityDescriptorW(pSecurityDescriptor, SDDL_REVISION_1, static_cast<DWORD>(securityInformation), &pText, nullptr)) return L"Descriptor: [invalid descriptor]";

        const std::wstring descriptor = std::format(L"Descriptor: {}", pText);
        LocalFree(pText);

        return descriptor;
    }

}

namespace mimo {

    namespace trace {

        namespace details {

            namespace security {

                std::wstring RenderQuery(const protocol::RecordData& data) {
                    const protocol::FltParameters& parameters = data.parameters;
                    std::wstring details = std::format(L"Information: {}, Length: {}", names::RenderSecurityInformation(parameters.querySecurity.securityInformation), parameters.querySecurity.length);

                    const std::wstring descriptor = RenderDescriptor(ExtractPayload(data.supplement.security), parameters.querySecurity.securityInformation);

                    if (!descriptor.empty()) {
                        details += L", ";
                        details += descriptor;
                    }

                    return details;
                }


                std::wstring RenderSet(const protocol::RecordData& data) {
                    const protocol::FltParameters& parameters = data.parameters;
                    std::wstring details = std::format(L"Information: {}", names::RenderSecurityInformation(parameters.setSecurity.securityInformation));

                    const std::wstring descriptor = RenderDescriptor(ExtractPayload(data.supplement.security), parameters.setSecurity.securityInformation);

                    if (!descriptor.empty()) {
                        details += L", ";
                        details += descriptor;
                    }

                    return details;
                }

            }

        }

    }

}
