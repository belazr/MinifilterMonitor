#include "create.h"

#include "..\kernel.h"
#include "..\names.h"
#include "..\values.h"

#include "..\..\text.h"

#include "..\..\..\..\inc\protocol.h"

#include <cstdint>
#include <format>
#include <string>
#include <string_view>

namespace mimo {

    namespace trace {

        namespace details {

            namespace create {

                std::wstring Render(const protocol::RecordData& data) {
                    const protocol::FltParameters& parameters = data.parameters;
                    const protocol::CreateSupplement& createSupplement = data.supplement.create;
                    std::wstring result;

                    if (createSupplement.captured & protocol::CREATE_CAPTURED_DESIRED_ACCESS) {
                        result += std::format(L"Desired Access: {}, ", names::RenderDesiredAccess(createSupplement.desiredAccess));
                    }

                    const uint32_t disposition = parameters.create.options >> 24;
                    const uint32_t createOptions = parameters.create.options & 0x00FFFFFFu;

                    result += std::format(L"Disposition: {}, ", names::RenderCreateDisposition(disposition));

                    const std::wstring options = names::RenderCreateOptions(createOptions);

                    if (!options.empty()) {
                        result += std::format(L"Options: {}, ", options);
                    }

                    result += std::format(L"Attributes: {}, ", names::RenderFileAttributes(parameters.create.fileAttributes));
                    result += std::format(L"ShareMode: {}, ", names::RenderShareAccess(parameters.create.shareAccess));

                    if (disposition == kernel::FILE_OPEN) {
                        result += L"AllocationSize: n/a, ";
                    }
                    else {
                        result += std::format(L"AllocationSize: {}, ", parameters.create.allocationSize);
                    }

                    if (parameters.create.eaLength) {
                        result += std::format(L"EaLength: {}, ", parameters.create.eaLength);
                    }

                    if (createSupplement.captured & protocol::CREATE_CAPTURED_IMPERSONATED_SID) {
                        result += std::format(L"Impersonating: {}, ", values::RenderSid(createSupplement.impersonatedSid));
                    }

                    if (data.status == 0 || data.information == kernel::FILE_EXISTS || data.information == kernel::FILE_DOES_NOT_EXIST) {
                        const std::wstring openResult = names::RenderOpenResult(data.information);

                        if (!openResult.empty()) {
                            result += std::format(L"OpenResult: {}, ", openResult);
                        }
                    }

                    const std::wstring_view ecpText = text::Extract(createSupplement.ecpText);

                    if (!ecpText.empty()) {
                        result += text::MarkTruncated(ecpText, createSupplement.captured & protocol::CREATE_TRUNCATED_ECP_TEXT);
                        result += L", ";
                    }

                    if (!result.empty()) {
                        result.resize(result.size() - 2u);
                    }

                    return result;
                }

            }

        }

    }

}
