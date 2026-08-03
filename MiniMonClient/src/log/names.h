#pragma once

#include <cstdint>
#include <string>

namespace mimo {

    namespace log {

        namespace names {

            std::wstring RenderOperationCategory(uint32_t flags);

            std::wstring RenderMajorFunction(uint8_t major);

            std::wstring RenderMinorFunction(uint8_t major, uint8_t minor);

            std::wstring RenderReparseTag(uint32_t tag);

            std::wstring RenderTransactionNotify(uint32_t notification);

            std::wstring RenderCreateDisposition(uint32_t disposition);

            std::wstring RenderOpenResult(uint64_t information);

            std::wstring RenderDesiredAccess(uint32_t desiredAccess);

            std::wstring RenderCreateOptions(uint32_t createOptions);

            std::wstring RenderShareAccess(uint32_t shareAccess);

            std::wstring RenderFileAttributes(uint32_t fileAttributes);

            std::wstring RenderFileInformationClass(uint32_t fileInformationClass);

            std::wstring RenderCompressionFormat(uint16_t format);

            std::wstring RenderRemoteProtocol(uint32_t protocol);

            std::wstring RenderRemoteProtocolFlags(uint32_t flags);

            std::wstring RenderDispositionFlags(uint32_t flags);

            std::wstring RenderRenameFlags(uint32_t flags);

            std::wstring RenderQueryDirectoryFlags(uint8_t operationFlags);

            std::wstring RenderCompletionFilter(uint32_t completionFilter);

            std::wstring RenderDirectoryNotifyInformationClass(uint32_t directoryNotifyInformationClass);

        }

    }

}
