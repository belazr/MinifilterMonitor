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

    std::wstring RenderQueryParameters(uint32_t information, uint32_t size) {

        return std::format(L"Information: {}, Length: {}", trace::names::RenderSecurityInformation(information), size);
    }


    std::span<const uint8_t> ExtractPayload(const protocol::SecuritySupplement& supplement) {

        if (!(supplement.captured & protocol::SECURITY_CAPTURED_PAYLOAD)) return {};

        return { supplement.payload, supplement.capturedSize };
    }


    std::wstring RenderDescriptorPayload(std::span<const uint8_t> payload, uint32_t information) {

        if (payload.empty()) return {};

        const PSECURITY_DESCRIPTOR pSecurityDescriptor = const_cast<uint8_t*>(payload.data());
        wchar_t* pText = nullptr;

        if (!ConvertSecurityDescriptorToStringSecurityDescriptorW(pSecurityDescriptor, SDDL_REVISION_1, static_cast<DWORD>(information), &pText, nullptr)) return L"Descriptor: [invalid descriptor]";

        const std::wstring descriptor = *pText != L'\0' ? std::format(L"Descriptor: {}", pText) : L"Descriptor: [no matching components]";
        LocalFree(pText);

        return descriptor;
    }


    std::wstring RenderSetParameters(uint32_t information) {

        return std::format(L"Information: {}", trace::names::RenderSecurityInformation(information));
    }

}

namespace mimo {

    namespace trace {

        namespace details {

            namespace security {

                std::wstring RenderQuery(const protocol::RecordData& data) {
                    const protocol::FltParameters& parameters = data.parameters;
                    std::wstring details = RenderQueryParameters(parameters.querySecurity.securityInformation, parameters.querySecurity.length);

                    const std::wstring payloadText = RenderDescriptorPayload(ExtractPayload(data.supplement.security), parameters.querySecurity.securityInformation);

                    if (!payloadText.empty()) {
                        details += L", ";
                        details += payloadText;
                    }

                    return details;
                }


                std::wstring RenderSet(const protocol::RecordData& data) {
                    const protocol::FltParameters& parameters = data.parameters;
                    std::wstring details = RenderSetParameters(parameters.setSecurity.securityInformation);

                    const std::wstring payloadText = RenderDescriptorPayload(ExtractPayload(data.supplement.security), parameters.setSecurity.securityInformation);

                    if (!payloadText.empty()) {
                        details += L", ";
                        details += payloadText;
                    }

                    return details;
                }

            }

        }

    }

}
