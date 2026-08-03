#include "create.h"

#include "..\names.h"

#include "..\..\kernel.h"
#include "..\..\text.h"

#include "..\..\..\..\inc\protocol.h"

#include <Windows.h>
#include <sddl.h>

#include <format>
#include <map>
#include <string>
#include <string_view>
#include <utility>

using namespace mimo;

namespace {

    constexpr DWORD ACCOUNT_NAME_CHARS = 256u;

    std::map<std::wstring, std::wstring> accountNames;

    std::wstring RenderImpersonated(const protocol::CreateSupplement& createSupplement) {
        const PSID pSid = const_cast<uint8_t*>(createSupplement.impersonatedSid);
        wchar_t* pSidText = nullptr;

        if (!ConvertSidToStringSidW(pSid, &pSidText)) return L"[invalid sid]";

        std::wstring sidText = pSidText;
        LocalFree(pSidText);

        const auto [it, inserted] = accountNames.try_emplace(std::move(sidText));

        if (!inserted) return it->second;

        wchar_t name[ACCOUNT_NAME_CHARS]{};
        wchar_t domain[ACCOUNT_NAME_CHARS]{};
        DWORD nameChars = ACCOUNT_NAME_CHARS;
        DWORD domainChars = ACCOUNT_NAME_CHARS;
        SID_NAME_USE use{};

        if (LookupAccountSidW(nullptr, pSid, name, &nameChars, domain, &domainChars, &use)) {
            it->second = *domain ? std::format(L"{}\\{}", domain, name) : name;
        }
        else {
            it->second = it->first;
        }

        return it->second;
    }

}

namespace mimo {

    namespace log {

        namespace details {

            namespace create {

                std::wstring Render(const protocol::RecordData& data) {
                    const protocol::FltParameters& parameters = data.parameters;
                    const protocol::CreateSupplement& createSupplement = data.supplement.create;
                    std::wstring result;

                    if (createSupplement.captured & protocol::CREATE_CAPTURED_DESIRED_ACCESS) {
                        result += std::format(L"Desired Access: {}, ", names::RenderDesiredAccess(createSupplement.desiredAccess));
                    }

                    const ULONG disposition = parameters.create.options >> 24;
                    const ULONG createOptions = parameters.create.options & 0x00FFFFFFu;

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
                        result += std::format(L"Impersonating: {}, ", RenderImpersonated(createSupplement));
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
