#include "create.h"

#include "..\eas.h"
#include "..\kernel.h"
#include "..\names.h"
#include "..\values.h"

#include "..\..\text.h"

#include "..\..\..\..\inc\protocol.h"

#include <cstdint>
#include <format>
#include <span>
#include <string>
#include <string_view>

using namespace mimo;

namespace {

    std::wstring RenderParameters(uint32_t options, uint16_t fileAttributes, uint16_t shareAccess, int64_t allocationSize) {
        const uint32_t disposition = options >> 24;
        const uint32_t createOptions = options & 0x00FFFFFFu;
        std::wstring result = std::format(L"Disposition: {}", trace::names::RenderCreateDisposition(disposition));

        if (createOptions) {
            result += std::format(L", Options: {}", trace::names::RenderCreateOptions(createOptions));
        }

        result += std::format(L", Attributes: {}, ShareMode: {}", trace::names::RenderFileAttributes(fileAttributes), trace::names::RenderShareAccess(shareAccess));

        if (disposition == trace::kernel::FILE_OPEN) {
            result += L", AllocationSize: n/a";
        }
        else {
            result += std::format(L", AllocationSize: {}", allocationSize);
        }

        return result;
    }


    std::wstring RenderEcpText(const protocol::CreateSupplement& supplement) {
        const std::wstring_view ecpText = text::Extract(supplement.ecpText);
        const bool truncated = supplement.captured & protocol::CREATE_TRUNCATED_ECP_TEXT;

        if (ecpText.empty() && !truncated) return {};

        return text::MarkTruncated(ecpText, truncated);
    }


    std::span<const uint8_t> ExtractEaBuffer(const protocol::CreateSupplement& supplement) {

        if (!(supplement.captured & protocol::CREATE_CAPTURED_EA_BUFFER)) return {};

        return { supplement.eaBuffer, supplement.capturedEaBufferSize };
    }

}

namespace mimo {

    namespace trace {

        namespace details {

            namespace create {

                std::wstring Render(const protocol::RecordData& data) {
                    const protocol::FltParameters& parameters = data.parameters;
                    const protocol::CreateSupplement& createSupplement = data.supplement.create;
                    std::wstring details;

                    if (createSupplement.captured & protocol::CREATE_CAPTURED_DESIRED_ACCESS) {
                        details += std::format(L"Desired Access: {}, ", names::RenderDesiredAccess(createSupplement.desiredAccess));
                    }

                    details += RenderParameters(parameters.create.options, parameters.create.fileAttributes, parameters.create.shareAccess, parameters.create.allocationSize);

                    if (data.operationFlags) {
                        details += L", ";
                        details += names::RenderCreateFlags(data.operationFlags);
                    }

                    if (createSupplement.captured & protocol::CREATE_CAPTURED_IMPERSONATED_SID) {
                        details += std::format(L", Impersonating: {}", values::RenderSid(createSupplement.impersonatedSid));
                    }

                    if (data.status == 0 || data.information == kernel::FILE_EXISTS || data.information == kernel::FILE_DOES_NOT_EXIST) {
                        const std::wstring openResultText = names::RenderOpenResult(data.information);

                        if (!openResultText.empty()) {
                            details += std::format(L", OpenResult: {}", openResultText);
                        }

                    }

                    const std::wstring ecpText = RenderEcpText(createSupplement);

                    if (!ecpText.empty()) {
                        details += L", ";
                        details += ecpText;
                    }

                    if (parameters.create.eaLength) {
                        details += std::format(L", EaLength: {}", parameters.create.eaLength);
                    }

                    const std::wstring eaBufferText = trace::eas::RenderEasPayload(ExtractEaBuffer(createSupplement));

                    if (!eaBufferText.empty()) {
                        details += L", ";
                        details += eaBufferText;
                    }

                    return details;
                }

            }

        }

    }

}
