#include "details.h"

#include "..\kernel.h"
#include "..\text.h"

#include "..\..\..\inc\protocol.h"

#include <Windows.h>
#include <sddl.h>

#include <format>
#include <map>
#include <span>
#include <string>
#include <string_view>
#include <utility>

using namespace mimo;

namespace {

    constexpr DWORD ACCOUNT_NAME_CHARS = 256u;

    std::map<std::wstring, std::wstring> accountNames;

    std::wstring RenderFlags(ULONG flags, std::span<const kernel::FlagName> names, std::wstring_view separator) {
        std::wstring result;

        for (const kernel::FlagName& entry : names) {

            if ((flags & entry.flag) != entry.flag) continue;

            result += entry.name;
            result += separator;
            flags &= ~entry.flag;
        }

        if (flags) {
            result += std::format(L"0x{:X}", flags);
            result += separator;
        }

        if (!result.empty()) {
            result.resize(result.size() - separator.size());
        }

        return result;
    }


    std::wstring RenderDisposition(ULONG disposition) {
        const wchar_t* const name = kernel::CreateDispositionName(disposition);

        if (*name) return name;

        return std::to_wstring(disposition);
    }


    std::wstring RenderAttributes(ULONG fileAttributes) {

        if (!fileAttributes) return L"n/a";

        return RenderFlags(fileAttributes, kernel::FILE_ATTRIBUTE_LETTERS, L"");
    }


    std::wstring RenderShareMode(ULONG shareAccess) {

        if (!shareAccess) return L"None";

        return RenderFlags(shareAccess, kernel::SHARE_ACCESS_NAMES, L"|");
    }


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


    std::wstring RenderCreate(const protocol::RecordData& data) {
        const protocol::FltParameters& parameters = data.parameters;
        const protocol::CreateSupplement& createSupplement = data.supplement.create;
        std::wstring result;

        if (createSupplement.captured & protocol::CREATE_CAPTURED_DESIRED_ACCESS) {
            result += std::format(L"Desired Access: {}, ", RenderFlags(createSupplement.desiredAccess, kernel::DESIRED_ACCESS_NAMES, L"|"));
        }

        const ULONG disposition = parameters.create.options >> 24;
        const ULONG createOptions = parameters.create.options & 0x00FFFFFFu;

        result += std::format(L"Disposition: {}, ", RenderDisposition(disposition));

        const std::wstring options = RenderFlags(createOptions, kernel::CREATE_OPTION_NAMES, L"|");

        if (!options.empty()) {
            result += std::format(L"Options: {}, ", options);
        }

        result += std::format(L"Attributes: {}, ", RenderAttributes(parameters.create.fileAttributes));
        result += std::format(L"ShareMode: {}, ", RenderShareMode(parameters.create.shareAccess));

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
            const wchar_t* const openResult = kernel::OpenResultName(data.information);

            if (*openResult) {
                result += std::format(L"OpenResult: {}, ", openResult);
            }
        }

        const std::wstring_view ecpText = text::Extract(createSupplement.ecpText);

        if (!ecpText.empty()) {
            result += ecpText;
            result += L", ";
        }

        if (!result.empty()) {
            result.resize(result.size() - 2u);
        }

        return result;
    }


    std::wstring RenderOffset(int64_t byteOffset) {

        if (byteOffset == kernel::FILE_WRITE_TO_END_OF_FILE) return L"EOF";

        if (byteOffset == kernel::FILE_USE_FILE_POINTER_POSITION) return L"Current";

        return std::to_wstring(byteOffset);
    }


    std::wstring RenderReadWriteIoFlags(const protocol::RecordData& data) {
        std::wstring ioFlags;

        if (data.irpFlags & kernel::IRP_NOCACHE) {
            ioFlags += L"Non-cached|";
        }

        if (data.irpFlags & kernel::IRP_PAGING_IO) {
            ioFlags += L"Paging I/O|";
        }

        if ((data.irpFlags & kernel::IRP_PAGING_IO) && (data.irpFlags & kernel::IRP_SYNCHRONOUS_PAGING_IO)) {
            ioFlags += L"Synchronous Paging I/O|";
        }
        else if (data.irpFlags & kernel::IRP_SYNCHRONOUS_API) {
            ioFlags += L"Synchronous|";
        }

        if (data.operationFlags & kernel::SL_WRITE_THROUGH) {
            ioFlags += L"Write Through|";
        }

        if (!ioFlags.empty()) {
            ioFlags.resize(ioFlags.size() - 1u);
        }

        return ioFlags;
    }


    std::wstring RenderReadWrite(const protocol::RecordData& data) {
        const protocol::FltParameters& parameters = data.parameters;

        if (data.callbackMinorId & kernel::IRP_MN_COMPLETE) {

            if (!parameters.readWrite.mdlAddress) return {};

            return std::format(L"Mdl: 0x{:X}", parameters.readWrite.mdlAddress);
        }

        std::wstring details = std::format(L"Offset: {}, Length: {}", RenderOffset(parameters.readWrite.byteOffset), parameters.readWrite.length);

        if ((data.operationFlags & kernel::SL_KEY_SPECIFIED) || parameters.readWrite.key) {
            details += std::format(L", Key: 0x{:X}", parameters.readWrite.key);
        }

        const std::wstring ioFlags = RenderReadWriteIoFlags(data);

        if (!ioFlags.empty()) {
            details += L", ";
            details += ioFlags;
        }

        if (data.status >= 0 && data.information != parameters.readWrite.length) {
            details += std::format(L", Transferred: {}", data.information);
        }

        return details;
    }

}

namespace mimo {

    namespace log {

        namespace details {

            std::wstring Render(const protocol::RecordData& data) {

                if (data.transactionNotify) return {};

                switch (data.callbackMajorId) {

                    case kernel::IRP_MJ_CREATE:

                        return RenderCreate(data);

                    case kernel::IRP_MJ_READ:
                    case kernel::IRP_MJ_WRITE:

                        return RenderReadWrite(data);

                }

                return {};
            }

        }

    }

}
