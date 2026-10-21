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

                    const uint32_t disposition = parameters.create.options >> 24;
                    const uint32_t createOptions = parameters.create.options & 0x00FFFFFFu;

                    details += std::format(L"Disposition: {}", names::RenderCreateDisposition(disposition));

                    if (createOptions) {
                        details += std::format(L", Options: {}", names::RenderCreateOptions(createOptions));
                    }

                    details += std::format(L", Attributes: {}, ShareMode: {}", names::RenderFileAttributes(parameters.create.fileAttributes), names::RenderShareAccess(parameters.create.shareAccess));

                    if (disposition == kernel::FILE_OPEN) {
                        details += L", AllocationSize: n/a";
                    }
                    else {
                        details += std::format(L", AllocationSize: {}", parameters.create.allocationSize);
                    }

                    if (data.operationFlags) {
                        details += L", ";
                        details += names::RenderCreateFlags(data.operationFlags);
                    }

                    if (createSupplement.captured & protocol::CREATE_CAPTURED_IMPERSONATED_SID) {
                        details += std::format(L", Impersonating: {}", values::RenderSid(createSupplement.impersonatedSid));
                    }

                    if (data.status == 0 || data.information == kernel::FILE_EXISTS || data.information == kernel::FILE_DOES_NOT_EXIST) {
                        const std::wstring openResult = names::RenderOpenResult(data.information);

                        if (!openResult.empty()) {
                            details += std::format(L", OpenResult: {}", openResult);
                        }

                    }

                    const std::wstring_view ecpText = text::Extract(createSupplement.ecpText);
                    const bool truncated = createSupplement.captured & protocol::CREATE_TRUNCATED_ECP_TEXT;

                    if (!ecpText.empty() || truncated) {
                        details += L", ";
                        details += text::MarkTruncated(ecpText, truncated);
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
