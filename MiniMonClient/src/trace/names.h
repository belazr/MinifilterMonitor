#pragma once

#include <cstdint>
#include <string>

namespace mimo {

    namespace trace {

        namespace names {

            std::wstring RenderRequestorMode(uint8_t mode);

            std::wstring RenderIoPriorityHint(uint8_t hint);

            std::wstring RenderIrql(uint8_t irql);

            std::wstring RenderOperationCategory(uint32_t flags);

            std::wstring RenderMajorFunction(uint8_t major);

            std::wstring RenderMinorFunction(uint8_t major, uint8_t minor);

            std::wstring RenderReparseTag(uint32_t tag);

            std::wstring RenderTransactionNotify(uint32_t notification);

            std::wstring RenderCreateDisposition(uint32_t disposition);

            std::wstring RenderOpenResult(uint64_t information);

            std::wstring RenderDesiredAccess(uint32_t access);

            std::wstring RenderCreateOptions(uint32_t options);

            std::wstring RenderShareAccess(uint32_t access);

            std::wstring RenderFileAttributes(uint32_t attributes);

            std::wstring RenderCreateFlags(uint8_t flags);

            std::wstring RenderIrpFlags(uint32_t flags);

            std::wstring RenderReadWriteFlags(uint8_t flags);

            std::wstring RenderFileInformationClass(uint32_t fileInformationClass);

            std::wstring RenderInformationFlags(uint32_t infoClass, uint8_t flags);

            std::wstring RenderCompressionFormat(uint16_t format);

            std::wstring RenderRemoteProtocol(uint32_t protocol);

            std::wstring RenderRemoteProtocolFlags(uint32_t flags);

            std::wstring RenderDispositionFlags(uint32_t flags);

            std::wstring RenderRenameFlags(uint32_t flags);

            std::wstring RenderStorageTierClass(uint32_t tier);

            std::wstring RenderDesiredStorageClassFlags(uint32_t flags);

            std::wstring RenderStorageReserveId(uint32_t id);

            std::wstring RenderKnownFolderType(uint32_t type);

            std::wstring RenderEaFlags(uint8_t flags);

            std::wstring RenderScanFlags(uint8_t flags);

            std::wstring RenderFlushFlags(uint8_t flags);

            std::wstring RenderFsInformationClass(uint32_t fsInformationClass);

            std::wstring RenderFileSystemAttributes(uint32_t attributes);

            std::wstring RenderFileSystemControlFlags(uint32_t flags);

            std::wstring RenderNotifyFlags(uint8_t flags);

            std::wstring RenderCompletionFilter(uint32_t filter);

            std::wstring RenderDirectoryNotifyInformationClass(uint32_t infoClass);

            std::wstring RenderFileAction(uint32_t action);

            std::wstring RenderVerifyVolumeFlags(uint8_t flags);

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

            std::wstring RenderFilePrefetchType(uint32_t type);

            std::wstring RenderPersistentVolumeState(uint32_t flags);

            std::wstring RenderMarkHandleInfo(uint32_t info);

            std::wstring RenderUsnSourceInfo(uint32_t info);

            std::wstring RenderTxfsRmFlags(uint32_t flags);

            std::wstring RenderTxfsLoggingMode(uint16_t mode);

            std::wstring RenderTxfsRmState(uint32_t state);

            std::wstring RenderUsnJournalFlags(uint32_t flags);

            std::wstring RenderDeviceIoFlags(uint8_t flags);

            std::wstring RenderStoragePropertyId(uint32_t propertyId);

            std::wstring RenderStorageQueryType(uint32_t type);

            std::wstring RenderMediaType(uint32_t mediaType);

            std::wstring RenderPartitionStyle(uint32_t style);

            std::wstring RenderDiskAttributes(uint32_t attributes);

            std::wstring RenderStorageDeviceFlags(uint32_t flags);

            std::wstring RenderSecurityInformation(uint32_t information);

            std::wstring RenderDeviceRelationType(uint32_t type);

            std::wstring RenderDeviceUsageNotificationType(uint32_t type);

            std::wstring RenderSectionSyncType(uint32_t type);

            std::wstring RenderPageProtection(uint32_t protection);

            std::wstring RenderSectionSyncFlags(uint32_t flags);

            std::wstring RenderAllocationAttributes(uint32_t attributes);

            std::wstring RenderFileSystemType(uint32_t fileSystemType);

        }

    }

}
