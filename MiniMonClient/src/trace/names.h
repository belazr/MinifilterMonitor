#pragma once

#include <cstdint>
#include <string>

namespace mimo {

    namespace trace {

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

            std::wstring RenderFsInformationClass(uint32_t fsInformationClass);

            std::wstring RenderFileSystemAttributes(uint32_t fileSystemAttributes);

            std::wstring RenderQueryDirectoryFlags(uint8_t operationFlags);

            std::wstring RenderCompletionFilter(uint32_t completionFilter);

            std::wstring RenderDirectoryNotifyInformationClass(uint32_t directoryNotifyInformationClass);

            std::wstring RenderFsControlCode(uint32_t fsControlCode);

            std::wstring RenderIoControlCode(uint32_t ioControlCode);

            std::wstring RenderDeviceType(uint32_t deviceType);

            std::wstring RenderSymlinkFlags(uint32_t flags);

            std::wstring RenderFileSystemStatisticsType(uint16_t type);

            std::wstring RenderOplockLevel(uint32_t level);

            std::wstring RenderOplockInputFlags(uint32_t flags);

            std::wstring RenderOplockOutputFlags(uint32_t flags);

            std::wstring RenderFileRegionUsage(uint32_t usage);

            std::wstring RenderUsnReason(uint32_t reason);

            std::wstring RenderStoragePropertyId(uint32_t propertyId);

            std::wstring RenderStorageQueryType(uint32_t queryType);

            std::wstring RenderMediaType(uint32_t mediaType);

        }

    }

}
