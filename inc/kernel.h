#pragma once

#include <cstdint>

namespace mimo {

    namespace kernel {

        // RecordData::flags, operation category flags
        inline constexpr uint32_t FLTFL_CALLBACK_DATA_IRP_OPERATION       = 0x00000001u;
        inline constexpr uint32_t FLTFL_CALLBACK_DATA_FAST_IO_OPERATION   = 0x00000002u;
        inline constexpr uint32_t FLTFL_CALLBACK_DATA_FS_FILTER_OPERATION = 0x00000004u;

        // RecordData::topLevelIrp, IoGetTopLevelIrp sentinels
        inline constexpr uint64_t FSRTL_FSP_TOP_LEVEL_IRP               = 0x01u;
        inline constexpr uint64_t FSRTL_CACHE_TOP_LEVEL_IRP             = 0x02u;
        inline constexpr uint64_t FSRTL_MOD_WRITE_TOP_LEVEL_IRP         = 0x03u;
        inline constexpr uint64_t FSRTL_FAST_IO_TOP_LEVEL_IRP           = 0x04u;
        inline constexpr uint64_t FSRTL_NETWORK1_TOP_LEVEL_IRP          = 0x05u;
        inline constexpr uint64_t FSRTL_NETWORK2_TOP_LEVEL_IRP          = 0x06u;
        inline constexpr uint64_t FSRTL_ASYNC_CACHED_READ_TOP_LEVEL_IRP = 0x07u;
        inline constexpr uint64_t FSRTL_VOLSNAP_TOP_LEVEL_IRP           = 0x08u;

        // RecordData::irpFlags, IRP flags
        inline constexpr uint32_t IRP_NOCACHE                = 0x00000001u;
        inline constexpr uint32_t IRP_PAGING_IO              = 0x00000002u;
        inline constexpr uint32_t IRP_MOUNT_COMPLETION       = 0x00000002u;    // volume mount or verify
        inline constexpr uint32_t IRP_SYNCHRONOUS_API        = 0x00000004u;
        inline constexpr uint32_t IRP_ASSOCIATED_IRP         = 0x00000008u;
        inline constexpr uint32_t IRP_BUFFERED_IO            = 0x00000010u;
        inline constexpr uint32_t IRP_DEALLOCATE_BUFFER      = 0x00000020u;
        inline constexpr uint32_t IRP_INPUT_OPERATION        = 0x00000040u;    // buffered I/O returning data
        inline constexpr uint32_t IRP_SYNCHRONOUS_PAGING_IO  = 0x00000040u;
        inline constexpr uint32_t IRP_CREATE_OPERATION       = 0x00000080u;
        inline constexpr uint32_t IRP_READ_OPERATION         = 0x00000100u;
        inline constexpr uint32_t IRP_WRITE_OPERATION        = 0x00000200u;
        inline constexpr uint32_t IRP_CLOSE_OPERATION        = 0x00000400u;
        inline constexpr uint32_t IRP_DEFER_IO_COMPLETION    = 0x00000800u;
        inline constexpr uint32_t IRP_OB_QUERY_NAME          = 0x00001000u;
        inline constexpr uint32_t IRP_HOLD_DEVICE_QUEUE      = 0x00002000u;
        inline constexpr uint32_t IRP_UM_DRIVER_INITIATED_IO = 0x00400000u;

        // RecordData::operationFlags, create stack-location SL_* flags
        inline constexpr uint8_t SL_FORCE_ACCESS_CHECK        = 0x01u;
        inline constexpr uint8_t SL_OPEN_PAGING_FILE          = 0x02u;
        inline constexpr uint8_t SL_OPEN_TARGET_DIRECTORY     = 0x04u;
        inline constexpr uint8_t SL_STOP_ON_SYMLINK           = 0x08u;
        inline constexpr uint8_t SL_IGNORE_READONLY_ATTRIBUTE = 0x40u;
        inline constexpr uint8_t SL_CASE_SENSITIVE            = 0x80u;

        // RecordData::operationFlags, read/write stack-location SL_* flags
        inline constexpr uint8_t SL_KEY_SPECIFIED                   = 0x01u;
        inline constexpr uint8_t SL_OVERRIDE_VERIFY_VOLUME          = 0x02u;
        inline constexpr uint8_t SL_WRITE_THROUGH                   = 0x04u;
        inline constexpr uint8_t SL_FT_SEQUENTIAL_WRITE             = 0x08u;
        inline constexpr uint8_t SL_FORCE_DIRECT_WRITE              = 0x10u;
        inline constexpr uint8_t SL_REALTIME_STREAM                 = 0x20u;    // optical media
        inline constexpr uint8_t SL_PERSISTENT_MEMORY_FIXED_MAPPING = 0x20u;    // persistent memory writes
        inline constexpr uint8_t SL_BYPASS_IO                       = 0x40u;

        // RecordData::operationFlags, query information and set information stack-location SL_* flags
        inline constexpr uint8_t SL_BYPASS_ACCESS_CHECK            = 0x01u;    // rename/link classes
        inline constexpr uint8_t SL_INFO_FORCE_ACCESS_CHECK        = 0x01u;    // FileCaseSensitiveInformation
        inline constexpr uint8_t SL_INFO_IGNORE_READONLY_ATTRIBUTE = 0x40u;

        // RecordData::operationFlags, query EA, query quota and directory control query stack-location SL_* flags
        inline constexpr uint8_t SL_RESTART_SCAN                = 0x01u;
        inline constexpr uint8_t SL_RETURN_SINGLE_ENTRY         = 0x02u;
        inline constexpr uint8_t SL_INDEX_SPECIFIED             = 0x04u;
        inline constexpr uint8_t SL_RETURN_ON_DISK_ENTRIES_ONLY = 0x08u;
        inline constexpr uint8_t SL_NO_CURSOR_UPDATE            = 0x10u;

        // RecordData::operationFlags, flush buffers stack-location SL_* flag
        inline constexpr uint8_t SL_FORCE_ASYNCHRONOUS = 0x01u;

        // RecordData::operationFlags, directory control notify stack-location SL_* flag
        inline constexpr uint8_t SL_WATCH_TREE = 0x01u;

        // RecordData::operationFlags, file system control verify volume stack-location SL_* flag
        inline constexpr uint8_t SL_ALLOW_RAW_MOUNT = 0x01u;

        // RecordData::operationFlags, device control and file system control stack-location SL_* flags
        inline constexpr uint8_t SL_READ_ACCESS_GRANTED  = 0x01u;
        inline constexpr uint8_t SL_WRITE_ACCESS_GRANTED = 0x04u;

        // RecordData::operationFlags, lock control stack-location SL_* flags
        inline constexpr uint8_t SL_FAIL_IMMEDIATELY = 0x01u;
        inline constexpr uint8_t SL_EXCLUSIVE_LOCK   = 0x02u;

        // RecordData::requestorMode, KPROCESSOR_MODE values
        inline constexpr uint8_t KernelMode = 0u;
        inline constexpr uint8_t UserMode   = 1u;

        // RecordData::ioPriorityHint, IO_PRIORITY_HINT values
        inline constexpr uint8_t IoPriorityVeryLow  = 0u;
        inline constexpr uint8_t IoPriorityLow      = 1u;
        inline constexpr uint8_t IoPriorityNormal   = 2u;
        inline constexpr uint8_t IoPriorityHigh     = 3u;
        inline constexpr uint8_t IoPriorityCritical = 4u;

        // standard IRP major codes
        inline constexpr uint8_t IRP_MJ_CREATE                   = 0x00u;
        inline constexpr uint8_t IRP_MJ_CREATE_NAMED_PIPE        = 0x01u;
        inline constexpr uint8_t IRP_MJ_CLOSE                    = 0x02u;
        inline constexpr uint8_t IRP_MJ_READ                     = 0x03u;
        inline constexpr uint8_t IRP_MJ_WRITE                    = 0x04u;
        inline constexpr uint8_t IRP_MJ_QUERY_INFORMATION        = 0x05u;
        inline constexpr uint8_t IRP_MJ_SET_INFORMATION          = 0x06u;
        inline constexpr uint8_t IRP_MJ_QUERY_EA                 = 0x07u;
        inline constexpr uint8_t IRP_MJ_SET_EA                   = 0x08u;
        inline constexpr uint8_t IRP_MJ_FLUSH_BUFFERS            = 0x09u;
        inline constexpr uint8_t IRP_MJ_QUERY_VOLUME_INFORMATION = 0x0Au;
        inline constexpr uint8_t IRP_MJ_SET_VOLUME_INFORMATION   = 0x0Bu;
        inline constexpr uint8_t IRP_MJ_DIRECTORY_CONTROL        = 0x0Cu;
        inline constexpr uint8_t IRP_MJ_FILE_SYSTEM_CONTROL      = 0x0Du;
        inline constexpr uint8_t IRP_MJ_DEVICE_CONTROL           = 0x0Eu;
        inline constexpr uint8_t IRP_MJ_INTERNAL_DEVICE_CONTROL  = 0x0Fu;
        inline constexpr uint8_t IRP_MJ_SHUTDOWN                 = 0x10u;
        inline constexpr uint8_t IRP_MJ_LOCK_CONTROL             = 0x11u;
        inline constexpr uint8_t IRP_MJ_CLEANUP                  = 0x12u;
        inline constexpr uint8_t IRP_MJ_CREATE_MAILSLOT          = 0x13u;
        inline constexpr uint8_t IRP_MJ_QUERY_SECURITY           = 0x14u;
        inline constexpr uint8_t IRP_MJ_SET_SECURITY             = 0x15u;
        inline constexpr uint8_t IRP_MJ_POWER                    = 0x16u;
        inline constexpr uint8_t IRP_MJ_SYSTEM_CONTROL           = 0x17u;
        inline constexpr uint8_t IRP_MJ_DEVICE_CHANGE            = 0x18u;
        inline constexpr uint8_t IRP_MJ_QUERY_QUOTA              = 0x19u;
        inline constexpr uint8_t IRP_MJ_SET_QUOTA                = 0x1Au;
        inline constexpr uint8_t IRP_MJ_PNP                      = 0x1Bu;
        inline constexpr uint8_t IRP_MJ_SCSI                     = IRP_MJ_INTERNAL_DEVICE_CONTROL;

        // FltMgr extension major codes
        inline constexpr uint8_t IRP_MJ_ACQUIRE_FOR_SECTION_SYNCHRONIZATION = static_cast<uint8_t>(-1);
        inline constexpr uint8_t IRP_MJ_RELEASE_FOR_SECTION_SYNCHRONIZATION = static_cast<uint8_t>(-2);
        inline constexpr uint8_t IRP_MJ_ACQUIRE_FOR_MOD_WRITE               = static_cast<uint8_t>(-3);
        inline constexpr uint8_t IRP_MJ_RELEASE_FOR_MOD_WRITE               = static_cast<uint8_t>(-4);
        inline constexpr uint8_t IRP_MJ_ACQUIRE_FOR_CC_FLUSH                = static_cast<uint8_t>(-5);
        inline constexpr uint8_t IRP_MJ_RELEASE_FOR_CC_FLUSH                = static_cast<uint8_t>(-6);
        inline constexpr uint8_t IRP_MJ_QUERY_OPEN                          = static_cast<uint8_t>(-7);

        inline constexpr uint8_t IRP_MJ_FAST_IO_CHECK_IF_POSSIBLE           = static_cast<uint8_t>(-13);
        inline constexpr uint8_t IRP_MJ_NETWORK_QUERY_OPEN                  = static_cast<uint8_t>(-14);
        inline constexpr uint8_t IRP_MJ_MDL_READ                            = static_cast<uint8_t>(-15);
        inline constexpr uint8_t IRP_MJ_MDL_READ_COMPLETE                   = static_cast<uint8_t>(-16);
        inline constexpr uint8_t IRP_MJ_PREPARE_MDL_WRITE                   = static_cast<uint8_t>(-17);
        inline constexpr uint8_t IRP_MJ_MDL_WRITE_COMPLETE                  = static_cast<uint8_t>(-18);
        inline constexpr uint8_t IRP_MJ_VOLUME_MOUNT                        = static_cast<uint8_t>(-19);
        inline constexpr uint8_t IRP_MJ_VOLUME_DISMOUNT                     = static_cast<uint8_t>(-20);

        // IRP minor codes

        // directory control
        inline constexpr uint8_t IRP_MN_QUERY_DIRECTORY            = 0x01u;
        inline constexpr uint8_t IRP_MN_NOTIFY_CHANGE_DIRECTORY    = 0x02u;
        inline constexpr uint8_t IRP_MN_NOTIFY_CHANGE_DIRECTORY_EX = 0x03u;

        // file system control
        inline constexpr uint8_t IRP_MN_USER_FS_REQUEST  = 0x00u;
        inline constexpr uint8_t IRP_MN_MOUNT_VOLUME     = 0x01u;
        inline constexpr uint8_t IRP_MN_VERIFY_VOLUME    = 0x02u;
        inline constexpr uint8_t IRP_MN_LOAD_FILE_SYSTEM = 0x03u;
        inline constexpr uint8_t IRP_MN_KERNEL_CALL      = 0x04u;

        // lock control
        inline constexpr uint8_t IRP_MN_LOCK              = 0x01u;
        inline constexpr uint8_t IRP_MN_UNLOCK_SINGLE     = 0x02u;
        inline constexpr uint8_t IRP_MN_UNLOCK_ALL        = 0x03u;
        inline constexpr uint8_t IRP_MN_UNLOCK_ALL_BY_KEY = 0x04u;

        // flush buffers
        inline constexpr uint8_t IRP_MN_FLUSH_AND_PURGE      = 0x01u;
        inline constexpr uint8_t IRP_MN_FLUSH_DATA_ONLY      = 0x02u;
        inline constexpr uint8_t IRP_MN_FLUSH_NO_SYNC        = 0x03u;
        inline constexpr uint8_t IRP_MN_FLUSH_DATA_SYNC_ONLY = 0x04u;

        // read/write
        inline constexpr uint8_t IRP_MN_NORMAL           = 0x00u;
        inline constexpr uint8_t IRP_MN_DPC              = 0x01u;
        inline constexpr uint8_t IRP_MN_MDL              = 0x02u;
        inline constexpr uint8_t IRP_MN_COMPLETE         = 0x04u;
        inline constexpr uint8_t IRP_MN_COMPRESSED       = 0x08u;
        inline constexpr uint8_t IRP_MN_MDL_DPC          = IRP_MN_MDL | IRP_MN_DPC;
        inline constexpr uint8_t IRP_MN_COMPLETE_MDL     = IRP_MN_COMPLETE | IRP_MN_MDL;
        inline constexpr uint8_t IRP_MN_COMPLETE_MDL_DPC = IRP_MN_COMPLETE_MDL | IRP_MN_DPC;

        // device control
        inline constexpr uint8_t IRP_MN_SCSI_CLASS = 0x01u;

        // PnP
        inline constexpr uint8_t IRP_MN_START_DEVICE                 = 0x00u;
        inline constexpr uint8_t IRP_MN_QUERY_REMOVE_DEVICE          = 0x01u;
        inline constexpr uint8_t IRP_MN_REMOVE_DEVICE                = 0x02u;
        inline constexpr uint8_t IRP_MN_CANCEL_REMOVE_DEVICE         = 0x03u;
        inline constexpr uint8_t IRP_MN_STOP_DEVICE                  = 0x04u;
        inline constexpr uint8_t IRP_MN_QUERY_STOP_DEVICE            = 0x05u;
        inline constexpr uint8_t IRP_MN_CANCEL_STOP_DEVICE           = 0x06u;
        inline constexpr uint8_t IRP_MN_QUERY_DEVICE_RELATIONS       = 0x07u;
        inline constexpr uint8_t IRP_MN_QUERY_INTERFACE              = 0x08u;
        inline constexpr uint8_t IRP_MN_QUERY_CAPABILITIES           = 0x09u;
        inline constexpr uint8_t IRP_MN_QUERY_RESOURCES              = 0x0Au;
        inline constexpr uint8_t IRP_MN_QUERY_RESOURCE_REQUIREMENTS  = 0x0Bu;
        inline constexpr uint8_t IRP_MN_QUERY_DEVICE_TEXT            = 0x0Cu;
        inline constexpr uint8_t IRP_MN_FILTER_RESOURCE_REQUIREMENTS = 0x0Du;
        inline constexpr uint8_t IRP_MN_READ_CONFIG                  = 0x0Fu;
        inline constexpr uint8_t IRP_MN_WRITE_CONFIG                 = 0x10u;
        inline constexpr uint8_t IRP_MN_EJECT                        = 0x11u;
        inline constexpr uint8_t IRP_MN_SET_LOCK                     = 0x12u;
        inline constexpr uint8_t IRP_MN_QUERY_ID                     = 0x13u;
        inline constexpr uint8_t IRP_MN_QUERY_PNP_DEVICE_STATE       = 0x14u;
        inline constexpr uint8_t IRP_MN_QUERY_BUS_INFORMATION        = 0x15u;
        inline constexpr uint8_t IRP_MN_DEVICE_USAGE_NOTIFICATION    = 0x16u;
        inline constexpr uint8_t IRP_MN_SURPRISE_REMOVAL             = 0x17u;
        inline constexpr uint8_t IRP_MN_QUERY_LEGACY_BUS_INFORMATION = 0x18u;
        inline constexpr uint8_t IRP_MN_DEVICE_ENUMERATED            = 0x19u;

        // power
        inline constexpr uint8_t IRP_MN_WAIT_WAKE      = 0x00u;
        inline constexpr uint8_t IRP_MN_POWER_SEQUENCE = 0x01u;
        inline constexpr uint8_t IRP_MN_SET_POWER      = 0x02u;
        inline constexpr uint8_t IRP_MN_QUERY_POWER    = 0x03u;

        // system control
        inline constexpr uint8_t IRP_MN_QUERY_ALL_DATA         = 0x00u;
        inline constexpr uint8_t IRP_MN_QUERY_SINGLE_INSTANCE  = 0x01u;
        inline constexpr uint8_t IRP_MN_CHANGE_SINGLE_INSTANCE = 0x02u;
        inline constexpr uint8_t IRP_MN_CHANGE_SINGLE_ITEM     = 0x03u;
        inline constexpr uint8_t IRP_MN_ENABLE_EVENTS          = 0x04u;
        inline constexpr uint8_t IRP_MN_DISABLE_EVENTS         = 0x05u;
        inline constexpr uint8_t IRP_MN_ENABLE_COLLECTION      = 0x06u;
        inline constexpr uint8_t IRP_MN_DISABLE_COLLECTION     = 0x07u;
        inline constexpr uint8_t IRP_MN_REGINFO                = 0x08u;
        inline constexpr uint8_t IRP_MN_EXECUTE_METHOD         = 0x09u;
        inline constexpr uint8_t IRP_MN_REGINFO_EX             = 0x0Bu;

        // ntifs.h WSL reparse tags
        inline constexpr uint32_t IO_REPARSE_TAG_LX_SYMLINK = 0xA000001Du;
        inline constexpr uint32_t IO_REPARSE_TAG_LX_FIFO    = 0x80000024u;
        inline constexpr uint32_t IO_REPARSE_TAG_LX_CHR     = 0x80000025u;
        inline constexpr uint32_t IO_REPARSE_TAG_LX_BLK     = 0x80000026u;

        inline constexpr uint32_t FILE_DISALLOW_EXCLUSIVE = 0x00020000u;
        inline constexpr uint32_t FILE_SESSION_AWARE      = 0x00040000u;

        // read/write ByteOffset sentinels
        inline constexpr int64_t FILE_WRITE_TO_END_OF_FILE      = -1;
        inline constexpr int64_t FILE_USE_FILE_POINTER_POSITION = -2;

        // FILE_INFORMATION_CLASS codes
        inline constexpr uint32_t FileDirectoryInformation                       = 1u;
        inline constexpr uint32_t FileFullDirectoryInformation                   = 2u;
        inline constexpr uint32_t FileBothDirectoryInformation                   = 3u;
        inline constexpr uint32_t FileBasicInformation                           = 4u;
        inline constexpr uint32_t FileStandardInformation                        = 5u;
        inline constexpr uint32_t FileInternalInformation                        = 6u;
        inline constexpr uint32_t FileEaInformation                              = 7u;
        inline constexpr uint32_t FileAccessInformation                          = 8u;
        inline constexpr uint32_t FileNameInformation                            = 9u;
        inline constexpr uint32_t FileRenameInformation                          = 10u;
        inline constexpr uint32_t FileLinkInformation                            = 11u;
        inline constexpr uint32_t FileNamesInformation                           = 12u;
        inline constexpr uint32_t FileDispositionInformation                     = 13u;
        inline constexpr uint32_t FilePositionInformation                        = 14u;
        inline constexpr uint32_t FileFullEaInformation                          = 15u;
        inline constexpr uint32_t FileModeInformation                            = 16u;
        inline constexpr uint32_t FileAlignmentInformation                       = 17u;
        inline constexpr uint32_t FileAllInformation                             = 18u;
        inline constexpr uint32_t FileAllocationInformation                      = 19u;
        inline constexpr uint32_t FileEndOfFileInformation                       = 20u;
        inline constexpr uint32_t FileAlternateNameInformation                   = 21u;
        inline constexpr uint32_t FileStreamInformation                          = 22u;
        inline constexpr uint32_t FilePipeInformation                            = 23u;
        inline constexpr uint32_t FilePipeLocalInformation                       = 24u;
        inline constexpr uint32_t FilePipeRemoteInformation                      = 25u;
        inline constexpr uint32_t FileMailslotQueryInformation                   = 26u;
        inline constexpr uint32_t FileMailslotSetInformation                     = 27u;
        inline constexpr uint32_t FileCompressionInformation                     = 28u;
        inline constexpr uint32_t FileObjectIdInformation                        = 29u;
        inline constexpr uint32_t FileCompletionInformation                      = 30u;
        inline constexpr uint32_t FileMoveClusterInformation                     = 31u;
        inline constexpr uint32_t FileQuotaInformation                           = 32u;
        inline constexpr uint32_t FileReparsePointInformation                    = 33u;
        inline constexpr uint32_t FileNetworkOpenInformation                     = 34u;
        inline constexpr uint32_t FileAttributeTagInformation                    = 35u;
        inline constexpr uint32_t FileTrackingInformation                        = 36u;
        inline constexpr uint32_t FileIdBothDirectoryInformation                 = 37u;
        inline constexpr uint32_t FileIdFullDirectoryInformation                 = 38u;
        inline constexpr uint32_t FileValidDataLengthInformation                 = 39u;
        inline constexpr uint32_t FileShortNameInformation                       = 40u;
        inline constexpr uint32_t FileIoCompletionNotificationInformation        = 41u;
        inline constexpr uint32_t FileIoStatusBlockRangeInformation              = 42u;
        inline constexpr uint32_t FileIoPriorityHintInformation                  = 43u;
        inline constexpr uint32_t FileSfioReserveInformation                     = 44u;
        inline constexpr uint32_t FileSfioVolumeInformation                      = 45u;
        inline constexpr uint32_t FileHardLinkInformation                        = 46u;
        inline constexpr uint32_t FileProcessIdsUsingFileInformation             = 47u;
        inline constexpr uint32_t FileNormalizedNameInformation                  = 48u;
        inline constexpr uint32_t FileNetworkPhysicalNameInformation             = 49u;
        inline constexpr uint32_t FileIdGlobalTxDirectoryInformation             = 50u;
        inline constexpr uint32_t FileIsRemoteDeviceInformation                  = 51u;
        inline constexpr uint32_t FileUnusedInformation                          = 52u;
        inline constexpr uint32_t FileNumaNodeInformation                        = 53u;
        inline constexpr uint32_t FileStandardLinkInformation                    = 54u;
        inline constexpr uint32_t FileRemoteProtocolInformation                  = 55u;
        inline constexpr uint32_t FileRenameInformationBypassAccessCheck         = 56u;
        inline constexpr uint32_t FileLinkInformationBypassAccessCheck           = 57u;
        inline constexpr uint32_t FileVolumeNameInformation                      = 58u;
        inline constexpr uint32_t FileIdInformation                              = 59u;
        inline constexpr uint32_t FileIdExtdDirectoryInformation                 = 60u;
        inline constexpr uint32_t FileReplaceCompletionInformation               = 61u;
        inline constexpr uint32_t FileHardLinkFullIdInformation                  = 62u;
        inline constexpr uint32_t FileIdExtdBothDirectoryInformation             = 63u;
        inline constexpr uint32_t FileDispositionInformationEx                   = 64u;
        inline constexpr uint32_t FileRenameInformationEx                        = 65u;
        inline constexpr uint32_t FileRenameInformationExBypassAccessCheck       = 66u;
        inline constexpr uint32_t FileDesiredStorageClassInformation             = 67u;
        inline constexpr uint32_t FileStatInformation                            = 68u;
        inline constexpr uint32_t FileMemoryPartitionInformation                 = 69u;
        inline constexpr uint32_t FileStatLxInformation                          = 70u;
        inline constexpr uint32_t FileCaseSensitiveInformation                   = 71u;
        inline constexpr uint32_t FileLinkInformationEx                          = 72u;
        inline constexpr uint32_t FileLinkInformationExBypassAccessCheck         = 73u;
        inline constexpr uint32_t FileStorageReserveIdInformation                = 74u;
        inline constexpr uint32_t FileCaseSensitiveInformationForceAccessCheck   = 75u;
        inline constexpr uint32_t FileKnownFolderInformation                     = 76u;
        inline constexpr uint32_t FileStatBasicInformation                       = 77u;
        inline constexpr uint32_t FileId64ExtdDirectoryInformation               = 78u;
        inline constexpr uint32_t FileId64ExtdBothDirectoryInformation           = 79u;
        inline constexpr uint32_t FileIdAllExtdDirectoryInformation              = 80u;
        inline constexpr uint32_t FileIdAllExtdBothDirectoryInformation          = 81u;
        inline constexpr uint32_t FileStreamReservationInformation               = 82u;
        inline constexpr uint32_t FileMupProviderInfo                            = 83u;

        // FILE_REMOTE_PROTOCOL_INFORMATION flags
        inline constexpr uint32_t REMOTE_PROTOCOL_FLAG_LOOPBACK          = 0x00000001u;
        inline constexpr uint32_t REMOTE_PROTOCOL_FLAG_OFFLINE           = 0x00000002u;
        inline constexpr uint32_t REMOTE_PROTOCOL_FLAG_PERSISTENT_HANDLE = 0x00000004u;
        inline constexpr uint32_t REMOTE_PROTOCOL_FLAG_PRIVACY           = 0x00000008u;
        inline constexpr uint32_t REMOTE_PROTOCOL_FLAG_INTEGRITY         = 0x00000010u;
        inline constexpr uint32_t REMOTE_PROTOCOL_FLAG_MUTUAL_AUTH       = 0x00000020u;

        // FILE_DISPOSITION_INFORMATION_EX flags
        inline constexpr uint32_t FILE_DISPOSITION_DO_NOT_DELETE             = 0x00000000u;
        inline constexpr uint32_t FILE_DISPOSITION_DELETE                    = 0x00000001u;
        inline constexpr uint32_t FILE_DISPOSITION_POSIX_SEMANTICS           = 0x00000002u;
        inline constexpr uint32_t FILE_DISPOSITION_FORCE_IMAGE_SECTION_CHECK = 0x00000004u;
        inline constexpr uint32_t FILE_DISPOSITION_ON_CLOSE                  = 0x00000008u;
        inline constexpr uint32_t FILE_DISPOSITION_IGNORE_READONLY_ATTRIBUTE = 0x00000010u;

        // FILE_RENAME_INFORMATION_EX flags
        inline constexpr uint32_t FILE_RENAME_REPLACE_IF_EXISTS                    = 0x00000001u;
        inline constexpr uint32_t FILE_RENAME_POSIX_SEMANTICS                      = 0x00000002u;
        inline constexpr uint32_t FILE_RENAME_SUPPRESS_PIN_STATE_INHERITANCE       = 0x00000004u;
        inline constexpr uint32_t FILE_RENAME_SUPPRESS_STORAGE_RESERVE_INHERITANCE = 0x00000008u;
        inline constexpr uint32_t FILE_RENAME_NO_INCREASE_AVAILABLE_SPACE          = 0x00000010u;
        inline constexpr uint32_t FILE_RENAME_NO_DECREASE_AVAILABLE_SPACE          = 0x00000020u;
        inline constexpr uint32_t FILE_RENAME_PRESERVE_AVAILABLE_SPACE             = 0x00000030u;
        inline constexpr uint32_t FILE_RENAME_IGNORE_READONLY_ATTRIBUTE            = 0x00000040u;
        inline constexpr uint32_t FILE_RENAME_FORCE_RESIZE_TARGET_SR               = 0x00000080u;
        inline constexpr uint32_t FILE_RENAME_FORCE_RESIZE_SOURCE_SR               = 0x00000100u;
        inline constexpr uint32_t FILE_RENAME_FORCE_RESIZE_SR                      = 0x00000180u;

        // FILE_LINK_INFORMATION flags
        inline constexpr uint32_t FILE_LINK_REPLACE_IF_EXISTS                    = 0x00000001u;
        inline constexpr uint32_t FILE_LINK_POSIX_SEMANTICS                      = 0x00000002u;
        inline constexpr uint32_t FILE_LINK_SUPPRESS_STORAGE_RESERVE_INHERITANCE = 0x00000008u;
        inline constexpr uint32_t FILE_LINK_NO_INCREASE_AVAILABLE_SPACE          = 0x00000010u;
        inline constexpr uint32_t FILE_LINK_NO_DECREASE_AVAILABLE_SPACE          = 0x00000020u;
        inline constexpr uint32_t FILE_LINK_PRESERVE_AVAILABLE_SPACE             = 0x00000030u;
        inline constexpr uint32_t FILE_LINK_IGNORE_READONLY_ATTRIBUTE            = 0x00000040u;
        inline constexpr uint32_t FILE_LINK_FORCE_RESIZE_TARGET_SR               = 0x00000080u;
        inline constexpr uint32_t FILE_LINK_FORCE_RESIZE_SOURCE_SR               = 0x00000100u;
        inline constexpr uint32_t FILE_LINK_FORCE_RESIZE_SR                      = 0x00000180u;

        // FILE_KNOWN_FOLDER_TYPE values
        inline constexpr uint32_t KnownFolderNone      = 0u;
        inline constexpr uint32_t KnownFolderDesktop   = 1u;
        inline constexpr uint32_t KnownFolderDocuments = 2u;
        inline constexpr uint32_t KnownFolderDownloads = 3u;
        inline constexpr uint32_t KnownFolderMusic     = 4u;
        inline constexpr uint32_t KnownFolderPictures  = 5u;
        inline constexpr uint32_t KnownFolderVideos    = 6u;
        inline constexpr uint32_t KnownFolderOther     = 7u;

        // FILE_FULL_EA_INFORMATION flag
        inline constexpr uint8_t FILE_NEED_EA = 0x80u;

        // FS_INFORMATION_CLASS codes
        inline constexpr uint32_t FileFsVolumeInformation       = 1u;
        inline constexpr uint32_t FileFsLabelInformation        = 2u;
        inline constexpr uint32_t FileFsSizeInformation         = 3u;
        inline constexpr uint32_t FileFsDeviceInformation       = 4u;
        inline constexpr uint32_t FileFsAttributeInformation    = 5u;
        inline constexpr uint32_t FileFsControlInformation      = 6u;
        inline constexpr uint32_t FileFsFullSizeInformation     = 7u;
        inline constexpr uint32_t FileFsObjectIdInformation     = 8u;
        inline constexpr uint32_t FileFsDriverPathInformation   = 9u;
        inline constexpr uint32_t FileFsVolumeFlagsInformation  = 10u;
        inline constexpr uint32_t FileFsSectorSizeInformation   = 11u;
        inline constexpr uint32_t FileFsDataCopyInformation     = 12u;
        inline constexpr uint32_t FileFsMetadataSizeInformation = 13u;
        inline constexpr uint32_t FileFsFullSizeInformationEx   = 14u;
        inline constexpr uint32_t FileFsGuidInformation         = 15u;

        // FILE_FS_CONTROL_INFORMATION flags
        inline constexpr uint32_t FILE_VC_QUOTA_TRACK            = 0x00000001u;
        inline constexpr uint32_t FILE_VC_QUOTA_ENFORCE          = 0x00000002u;
        inline constexpr uint32_t FILE_VC_CONTENT_INDEX_DISABLED = 0x00000008u;
        inline constexpr uint32_t FILE_VC_LOG_QUOTA_THRESHOLD    = 0x00000010u;
        inline constexpr uint32_t FILE_VC_LOG_QUOTA_LIMIT        = 0x00000020u;
        inline constexpr uint32_t FILE_VC_LOG_VOLUME_THRESHOLD   = 0x00000040u;
        inline constexpr uint32_t FILE_VC_LOG_VOLUME_LIMIT       = 0x00000080u;
        inline constexpr uint32_t FILE_VC_QUOTAS_INCOMPLETE      = 0x00000100u;
        inline constexpr uint32_t FILE_VC_QUOTAS_REBUILDING      = 0x00000200u;

        // ntifs.h completion filter flags
        inline constexpr uint32_t FILE_NOTIFY_CHANGE_EA           = 0x00000080u;
        inline constexpr uint32_t FILE_NOTIFY_CHANGE_STREAM_NAME  = 0x00000200u;
        inline constexpr uint32_t FILE_NOTIFY_CHANGE_STREAM_SIZE  = 0x00000400u;
        inline constexpr uint32_t FILE_NOTIFY_CHANGE_STREAM_WRITE = 0x00000800u;

        // DIRECTORY_NOTIFY_INFORMATION_CLASS codes
        inline constexpr uint32_t DirectoryNotifyInformation         = 1u;
        inline constexpr uint32_t DirectoryNotifyExtendedInformation = 2u;
        inline constexpr uint32_t DirectoryNotifyFullInformation     = 3u;

        // ntifs.h stream and tunnelling file actions
        inline constexpr uint32_t FILE_ACTION_ADDED_STREAM           = 0x00000006u;
        inline constexpr uint32_t FILE_ACTION_REMOVED_STREAM         = 0x00000007u;
        inline constexpr uint32_t FILE_ACTION_MODIFIED_STREAM        = 0x00000008u;
        inline constexpr uint32_t FILE_ACTION_REMOVED_BY_DELETE      = 0x00000009u;
        inline constexpr uint32_t FILE_ACTION_ID_NOT_TUNNELLED       = 0x0000000Au;
        inline constexpr uint32_t FILE_ACTION_TUNNELLED_ID_COLLISION = 0x0000000Bu;

        // REPARSE_DATA_BUFFER symbolic link flag
        inline constexpr uint32_t SYMLINK_FLAG_RELATIVE = 0x00000001u;

        // IOCTL codes
        inline constexpr uint32_t IOCTL_MOUNTDEV_QUERY_UNIQUE_ID           = 0x4D0000u;
        inline constexpr uint32_t IOCTL_MOUNTDEV_QUERY_SUGGESTED_LINK_NAME = 0x4D000Cu;
        inline constexpr uint32_t IOCTL_MOUNTDEV_LINK_CREATED              = 0x4DC010u;
        inline constexpr uint32_t IOCTL_MOUNTDEV_LINK_DELETED              = 0x4DC014u;
        inline constexpr uint32_t IOCTL_MOUNTDEV_QUERY_STABLE_GUID         = 0x4D0018u;
        inline constexpr uint32_t IOCTL_MOUNTDEV_QUERY_INTERFACE_NAME      = 0x4D001Cu;
        inline constexpr uint32_t IOCTL_MOUNTDEV_QUERY_DEVICE_NAME         = 0x4D0008u;

        // DEVICE_RELATION_TYPE values
        inline constexpr uint32_t BusRelations         = 0u;
        inline constexpr uint32_t EjectionRelations    = 1u;
        inline constexpr uint32_t PowerRelations       = 2u;
        inline constexpr uint32_t RemovalRelations     = 3u;
        inline constexpr uint32_t TargetDeviceRelation = 4u;
        inline constexpr uint32_t SingleBusRelations   = 5u;
        inline constexpr uint32_t TransportRelations   = 6u;

        // DEVICE_USAGE_NOTIFICATION_TYPE values
        inline constexpr uint32_t DeviceUsageTypeUndefined          = 0u;
        inline constexpr uint32_t DeviceUsageTypePaging             = 1u;
        inline constexpr uint32_t DeviceUsageTypeHibernation        = 2u;
        inline constexpr uint32_t DeviceUsageTypeDumpFile           = 3u;
        inline constexpr uint32_t DeviceUsageTypeBoot               = 4u;
        inline constexpr uint32_t DeviceUsageTypePostDisplay        = 5u;
        inline constexpr uint32_t DeviceUsageTypeGuestAssigned      = 6u;
        inline constexpr uint32_t DeviceUsageTypeInlineCryptoEngine = 7u;

        // FS_FILTER_SECTION_SYNC_TYPE values
        inline constexpr uint32_t SyncTypeOther         = 0u;
        inline constexpr uint32_t SyncTypeCreateSection = 1u;

        inline constexpr uint32_t FS_FILTER_SECTION_SYNC_IN_FLAG_DONT_UPDATE_LAST_ACCESS = 0x00000001u;
        inline constexpr uint32_t FS_FILTER_SECTION_SYNC_IN_FLAG_DONT_UPDATE_LAST_WRITE  = 0x00000002u;

    }

}
