#pragma once

#include <cstddef>
#include <cstdint>

namespace mimo {

    namespace trace {

        namespace kernel {

            // operation category flags carried in RecordData::flags
            inline constexpr uint32_t FLT_CALLBACK_DATA_IRP_OPERATION       = 0x00000001u;
            inline constexpr uint32_t FLT_CALLBACK_DATA_FAST_IO_OPERATION   = 0x00000002u;
            inline constexpr uint32_t FLT_CALLBACK_DATA_FS_FILTER_OPERATION = 0x00000004u;

            // IRP flags carried in RecordData::irpFlags
            inline constexpr uint32_t IRP_NOCACHE                 = 0x00000001u;
            inline constexpr uint32_t IRP_PAGING_IO               = 0x00000002u;
            inline constexpr uint32_t IRP_SYNCHRONOUS_API         = 0x00000004u;
            inline constexpr uint32_t IRP_SYNCHRONOUS_PAGING_IO   = 0x00000040u;

            // read/write stack-location SL_* flags carried in RecordData::operationFlags
            inline constexpr uint8_t SL_KEY_SPECIFIED = 0x01u;
            inline constexpr uint8_t SL_WRITE_THROUGH = 0x04u;

            // directory control query stack-location SL_* flags carried in RecordData::operationFlags
            inline constexpr uint8_t SL_RESTART_SCAN        = 0x01u;
            inline constexpr uint8_t SL_RETURN_SINGLE_ENTRY = 0x02u;
            inline constexpr uint8_t SL_INDEX_SPECIFIED     = 0x04u;

            // directory control notify stack-location SL_* flag carried in RecordData::operationFlags
            inline constexpr uint8_t SL_WATCH_TREE = 0x01u;

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
            inline constexpr uint8_t IRP_MJ_MAXIMUM_FUNCTION         = 0x1Bu;

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

            // IRP minor codes, meaning is scoped by the major code

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

            // power
            inline constexpr uint8_t IRP_MN_WAIT_WAKE      = 0x00u;
            inline constexpr uint8_t IRP_MN_POWER_SEQUENCE = 0x01u;
            inline constexpr uint8_t IRP_MN_SET_POWER      = 0x02u;
            inline constexpr uint8_t IRP_MN_QUERY_POWER    = 0x03u;

            // system control (WMI)
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

            // WSL reparse tags live in kernel-only ntifs.h
            inline constexpr uint32_t IO_REPARSE_TAG_LX_SYMLINK = 0xA000001Du;
            inline constexpr uint32_t IO_REPARSE_TAG_LX_FIFO    = 0x80000024u;
            inline constexpr uint32_t IO_REPARSE_TAG_LX_CHR     = 0x80000025u;
            inline constexpr uint32_t IO_REPARSE_TAG_LX_BLK     = 0x80000026u;

            inline constexpr uint32_t FILE_SUPERSEDE    = 0x00000000u;
            inline constexpr uint32_t FILE_OPEN         = 0x00000001u;
            inline constexpr uint32_t FILE_CREATE       = 0x00000002u;
            inline constexpr uint32_t FILE_OPEN_IF      = 0x00000003u;
            inline constexpr uint32_t FILE_OVERWRITE    = 0x00000004u;
            inline constexpr uint32_t FILE_OVERWRITE_IF = 0x00000005u;

            inline constexpr uint64_t FILE_SUPERSEDED     = 0u;
            inline constexpr uint64_t FILE_OPENED         = 1u;
            inline constexpr uint64_t FILE_CREATED        = 2u;
            inline constexpr uint64_t FILE_OVERWRITTEN    = 3u;
            inline constexpr uint64_t FILE_EXISTS         = 4u;
            inline constexpr uint64_t FILE_DOES_NOT_EXIST = 5u;

            inline constexpr uint32_t FILE_DIRECTORY_FILE            = 0x00000001u;
            inline constexpr uint32_t FILE_WRITE_THROUGH             = 0x00000002u;
            inline constexpr uint32_t FILE_SEQUENTIAL_ONLY           = 0x00000004u;
            inline constexpr uint32_t FILE_NO_INTERMEDIATE_BUFFERING = 0x00000008u;
            inline constexpr uint32_t FILE_SYNCHRONOUS_IO_ALERT      = 0x00000010u;
            inline constexpr uint32_t FILE_SYNCHRONOUS_IO_NONALERT   = 0x00000020u;
            inline constexpr uint32_t FILE_NON_DIRECTORY_FILE        = 0x00000040u;
            inline constexpr uint32_t FILE_CREATE_TREE_CONNECTION    = 0x00000080u;
            inline constexpr uint32_t FILE_COMPLETE_IF_OPLOCKED      = 0x00000100u;
            inline constexpr uint32_t FILE_NO_EA_KNOWLEDGE           = 0x00000200u;
            inline constexpr uint32_t FILE_OPEN_REMOTE_INSTANCE      = 0x00000400u;
            inline constexpr uint32_t FILE_RANDOM_ACCESS             = 0x00000800u;
            inline constexpr uint32_t FILE_DELETE_ON_CLOSE           = 0x00001000u;
            inline constexpr uint32_t FILE_OPEN_BY_FILE_ID           = 0x00002000u;
            inline constexpr uint32_t FILE_OPEN_FOR_BACKUP_INTENT    = 0x00004000u;
            inline constexpr uint32_t FILE_NO_COMPRESSION            = 0x00008000u;
            inline constexpr uint32_t FILE_OPEN_REQUIRING_OPLOCK     = 0x00010000u;
            inline constexpr uint32_t FILE_DISALLOW_EXCLUSIVE        = 0x00020000u;
            inline constexpr uint32_t FILE_SESSION_AWARE             = 0x00040000u;
            inline constexpr uint32_t FILE_RESERVE_OPFILTER          = 0x00100000u;
            inline constexpr uint32_t FILE_OPEN_REPARSE_POINT        = 0x00200000u;
            inline constexpr uint32_t FILE_OPEN_NO_RECALL            = 0x00400000u;
            inline constexpr uint32_t FILE_OPEN_FOR_FREE_SPACE_QUERY = 0x00800000u;

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

            // FILE_INFORMATION_CLASS payload layouts

            struct FILE_BASIC_INFORMATION {
                int64_t CreationTime;
                int64_t LastAccessTime;
                int64_t LastWriteTime;
                int64_t ChangeTime;
                uint32_t FileAttributes;
            };

            static_assert(sizeof(FILE_BASIC_INFORMATION) == 40u, "trace::kernel::FILE_BASIC_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_BASIC_INFORMATION, FileAttributes) == 32u, "trace::kernel::FILE_BASIC_INFORMATION x64 layout drift");

            struct FILE_STANDARD_INFORMATION {
                int64_t AllocationSize;
                int64_t EndOfFile;
                uint32_t NumberOfLinks;
                uint8_t DeletePending;
                uint8_t Directory;
            };

            static_assert(sizeof(FILE_STANDARD_INFORMATION) == 24u, "trace::kernel::FILE_STANDARD_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_STANDARD_INFORMATION, DeletePending) == 20u, "trace::kernel::FILE_STANDARD_INFORMATION x64 layout drift");

            struct FILE_INTERNAL_INFORMATION {
                int64_t IndexNumber;
            };

            static_assert(sizeof(FILE_INTERNAL_INFORMATION) == 8u, "trace::kernel::FILE_INTERNAL_INFORMATION x64 layout drift");

            struct FILE_EA_INFORMATION {
                uint32_t EaSize;
            };

            static_assert(sizeof(FILE_EA_INFORMATION) == 4u, "trace::kernel::FILE_EA_INFORMATION x64 layout drift");

            struct FILE_ACCESS_INFORMATION {
                uint32_t AccessFlags;
            };

            static_assert(sizeof(FILE_ACCESS_INFORMATION) == 4u, "trace::kernel::FILE_ACCESS_INFORMATION x64 layout drift");

            struct FILE_NAME_INFORMATION {
                uint32_t FileNameLength;
                wchar_t FileName[1u];
            };

            static_assert(sizeof(FILE_NAME_INFORMATION) == 8u, "trace::kernel::FILE_NAME_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_NAME_INFORMATION, FileName) == 4u, "trace::kernel::FILE_NAME_INFORMATION x64 layout drift");

            struct FILE_POSITION_INFORMATION {
                int64_t CurrentByteOffset;
            };

            static_assert(sizeof(FILE_POSITION_INFORMATION) == 8u, "trace::kernel::FILE_POSITION_INFORMATION x64 layout drift");

            struct FILE_MODE_INFORMATION {
                uint32_t Mode;
            };

            static_assert(sizeof(FILE_MODE_INFORMATION) == 4u, "trace::kernel::FILE_MODE_INFORMATION x64 layout drift");

            struct FILE_ALIGNMENT_INFORMATION {
                uint32_t AlignmentRequirement;
            };

            static_assert(sizeof(FILE_ALIGNMENT_INFORMATION) == 4u, "trace::kernel::FILE_ALIGNMENT_INFORMATION x64 layout drift");

            struct FILE_ALL_INFORMATION {
                FILE_BASIC_INFORMATION BasicInformation;
                FILE_STANDARD_INFORMATION StandardInformation;
                FILE_INTERNAL_INFORMATION InternalInformation;
                FILE_EA_INFORMATION EaInformation;
                FILE_ACCESS_INFORMATION AccessInformation;
                FILE_POSITION_INFORMATION PositionInformation;
                FILE_MODE_INFORMATION ModeInformation;
                FILE_ALIGNMENT_INFORMATION AlignmentInformation;
                FILE_NAME_INFORMATION NameInformation;
            };

            static_assert(sizeof(FILE_ALL_INFORMATION) == 104u, "trace::kernel::FILE_ALL_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_ALL_INFORMATION, StandardInformation) == 40u, "trace::kernel::FILE_ALL_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_ALL_INFORMATION, PositionInformation) == 80u, "trace::kernel::FILE_ALL_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_ALL_INFORMATION, NameInformation) == 96u, "trace::kernel::FILE_ALL_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_ALL_INFORMATION, NameInformation.FileName) == 100u, "trace::kernel::FILE_ALL_INFORMATION x64 layout drift");

            struct FILE_STREAM_INFORMATION {
                uint32_t NextEntryOffset;
                uint32_t StreamNameLength;
                int64_t StreamSize;
                int64_t StreamAllocationSize;
                wchar_t StreamName[1u];
            };

            static_assert(sizeof(FILE_STREAM_INFORMATION) == 32u, "trace::kernel::FILE_STREAM_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_STREAM_INFORMATION, StreamName) == 24u, "trace::kernel::FILE_STREAM_INFORMATION x64 layout drift");

            struct FILE_COMPRESSION_INFORMATION {
                int64_t CompressedFileSize;
                uint16_t CompressionFormat;
                uint8_t CompressionUnitShift;
                uint8_t ChunkShift;
                uint8_t ClusterShift;
                uint8_t Reserved[3u];
            };

            static_assert(sizeof(FILE_COMPRESSION_INFORMATION) == 16u, "trace::kernel::FILE_COMPRESSION_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_COMPRESSION_INFORMATION, CompressionFormat) == 8u, "trace::kernel::FILE_COMPRESSION_INFORMATION x64 layout drift");

            struct FILE_NETWORK_OPEN_INFORMATION {
                int64_t CreationTime;
                int64_t LastAccessTime;
                int64_t LastWriteTime;
                int64_t ChangeTime;
                int64_t AllocationSize;
                int64_t EndOfFile;
                uint32_t FileAttributes;
            };

            static_assert(sizeof(FILE_NETWORK_OPEN_INFORMATION) == 56u, "trace::kernel::FILE_NETWORK_OPEN_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_NETWORK_OPEN_INFORMATION, FileAttributes) == 48u, "trace::kernel::FILE_NETWORK_OPEN_INFORMATION x64 layout drift");

            struct FILE_ATTRIBUTE_TAG_INFORMATION {
                uint32_t FileAttributes;
                uint32_t ReparseTag;
            };

            static_assert(sizeof(FILE_ATTRIBUTE_TAG_INFORMATION) == 8u, "trace::kernel::FILE_ATTRIBUTE_TAG_INFORMATION x64 layout drift");

            struct FILE_LINK_ENTRY_INFORMATION {
                uint32_t NextEntryOffset;
                int64_t ParentFileId;
                uint32_t FileNameLength;    // character count
                wchar_t FileName[1u];
            };

            static_assert(sizeof(FILE_LINK_ENTRY_INFORMATION) == 24u, "trace::kernel::FILE_LINK_ENTRY_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_LINK_ENTRY_INFORMATION, ParentFileId) == 8u, "trace::kernel::FILE_LINK_ENTRY_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_LINK_ENTRY_INFORMATION, FileName) == 20u, "trace::kernel::FILE_LINK_ENTRY_INFORMATION x64 layout drift");

            struct FILE_LINKS_INFORMATION {
                uint32_t BytesNeeded;
                uint32_t EntriesReturned;
                FILE_LINK_ENTRY_INFORMATION Entry;
            };

            static_assert(sizeof(FILE_LINKS_INFORMATION) == 32u, "trace::kernel::FILE_LINKS_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_LINKS_INFORMATION, Entry) == 8u, "trace::kernel::FILE_LINKS_INFORMATION x64 layout drift");

            struct FILE_REMOTE_PROTOCOL_INFORMATION {
                uint16_t StructureVersion;
                uint16_t StructureSize;
                uint32_t Protocol;
                uint16_t ProtocolMajorVersion;
                uint16_t ProtocolMinorVersion;
                uint16_t ProtocolRevision;
                uint16_t Reserved;
                uint32_t Flags;
                uint32_t GenericReserved[8u];
                uint32_t ProtocolSpecific[16u];    // Smb2 capability union in the kernel
            };

            static_assert(sizeof(FILE_REMOTE_PROTOCOL_INFORMATION) == 116u, "trace::kernel::FILE_REMOTE_PROTOCOL_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_REMOTE_PROTOCOL_INFORMATION, Flags) == 16u, "trace::kernel::FILE_REMOTE_PROTOCOL_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_REMOTE_PROTOCOL_INFORMATION, GenericReserved) == 20u, "trace::kernel::FILE_REMOTE_PROTOCOL_INFORMATION x64 layout drift");

            struct FILE_ID_INFORMATION {
                uint64_t VolumeSerialNumber;
                uint8_t FileId[16u];
            };

            static_assert(sizeof(FILE_ID_INFORMATION) == 24u, "trace::kernel::FILE_ID_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_ID_INFORMATION, FileId) == 8u, "trace::kernel::FILE_ID_INFORMATION x64 layout drift");

            struct FILE_STAT_INFORMATION {
                int64_t FileId;
                int64_t CreationTime;
                int64_t LastAccessTime;
                int64_t LastWriteTime;
                int64_t ChangeTime;
                int64_t AllocationSize;
                int64_t EndOfFile;
                uint32_t FileAttributes;
                uint32_t ReparseTag;
                uint32_t NumberOfLinks;
                uint32_t EffectiveAccess;
            };

            static_assert(sizeof(FILE_STAT_INFORMATION) == 72u, "trace::kernel::FILE_STAT_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_STAT_INFORMATION, FileAttributes) == 56u, "trace::kernel::FILE_STAT_INFORMATION x64 layout drift");

            struct FILE_STAT_LX_INFORMATION {
                int64_t FileId;
                int64_t CreationTime;
                int64_t LastAccessTime;
                int64_t LastWriteTime;
                int64_t ChangeTime;
                int64_t AllocationSize;
                int64_t EndOfFile;
                uint32_t FileAttributes;
                uint32_t ReparseTag;
                uint32_t NumberOfLinks;
                uint32_t EffectiveAccess;
                uint32_t LxFlags;
                uint32_t LxUid;
                uint32_t LxGid;
                uint32_t LxMode;
                uint32_t LxDeviceIdMajor;
                uint32_t LxDeviceIdMinor;
            };

            static_assert(sizeof(FILE_STAT_LX_INFORMATION) == 96u, "trace::kernel::FILE_STAT_LX_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_STAT_LX_INFORMATION, LxFlags) == sizeof(FILE_STAT_INFORMATION), "trace::kernel::FILE_STAT_LX_INFORMATION does not extend FILE_STAT_INFORMATION");

            struct FILE_CASE_SENSITIVE_INFORMATION {
                uint32_t Flags;
            };

            static_assert(sizeof(FILE_CASE_SENSITIVE_INFORMATION) == 4u, "trace::kernel::FILE_CASE_SENSITIVE_INFORMATION x64 layout drift");

            struct FILE_STAT_BASIC_INFORMATION {
                int64_t FileId;
                int64_t CreationTime;
                int64_t LastAccessTime;
                int64_t LastWriteTime;
                int64_t ChangeTime;
                int64_t AllocationSize;
                int64_t EndOfFile;
                uint32_t FileAttributes;
                uint32_t ReparseTag;
                uint32_t NumberOfLinks;
                uint32_t DeviceType;
                uint32_t DeviceCharacteristics;
                uint32_t Reserved;
                uint64_t VolumeSerialNumber;
                uint8_t FileId128[16u];
            };

            static_assert(sizeof(FILE_STAT_BASIC_INFORMATION) == 104u, "trace::kernel::FILE_STAT_BASIC_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_STAT_BASIC_INFORMATION, DeviceType) == 68u, "trace::kernel::FILE_STAT_BASIC_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_STAT_BASIC_INFORMATION, FileId128) == 88u, "trace::kernel::FILE_STAT_BASIC_INFORMATION x64 layout drift");

            // FILE_LINK_INFORMATION shares this layout
            struct FILE_RENAME_INFORMATION {
                uint8_t ReplaceIfExists;
                uint64_t RootDirectory;     // HANDLE in the kernel, fixed 64-bit for the x64 layout
                uint32_t FileNameLength;
                wchar_t FileName[1u];
            };

            static_assert(sizeof(FILE_RENAME_INFORMATION) == 24u, "trace::kernel::FILE_RENAME_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_RENAME_INFORMATION, RootDirectory) == 8u, "trace::kernel::FILE_RENAME_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_RENAME_INFORMATION, FileNameLength) == 16u, "trace::kernel::FILE_RENAME_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_RENAME_INFORMATION, FileName) == 20u, "trace::kernel::FILE_RENAME_INFORMATION x64 layout drift");

            struct FILE_DISPOSITION_INFORMATION {
                uint8_t DeletePending;      // MS-FSCC spelling, the ntifs.h member name DeleteFile is a winbase.h macro
            };

            static_assert(sizeof(FILE_DISPOSITION_INFORMATION) == 1u, "trace::kernel::FILE_DISPOSITION_INFORMATION x64 layout drift");

            struct FILE_ALLOCATION_INFORMATION {
                int64_t AllocationSize;
            };

            static_assert(sizeof(FILE_ALLOCATION_INFORMATION) == 8u, "trace::kernel::FILE_ALLOCATION_INFORMATION x64 layout drift");

            struct FILE_END_OF_FILE_INFORMATION {
                int64_t EndOfFile;
            };

            static_assert(sizeof(FILE_END_OF_FILE_INFORMATION) == 8u, "trace::kernel::FILE_END_OF_FILE_INFORMATION x64 layout drift");

            struct FILE_VALID_DATA_LENGTH_INFORMATION {
                int64_t ValidDataLength;
            };

            static_assert(sizeof(FILE_VALID_DATA_LENGTH_INFORMATION) == 8u, "trace::kernel::FILE_VALID_DATA_LENGTH_INFORMATION x64 layout drift");

            struct FILE_DISPOSITION_INFORMATION_EX {
                uint32_t Flags;
            };

            static_assert(sizeof(FILE_DISPOSITION_INFORMATION_EX) == 4u, "trace::kernel::FILE_DISPOSITION_INFORMATION_EX x64 layout drift");

            // FILE_LINK_INFORMATION_EX shares this layout
            struct FILE_RENAME_INFORMATION_EX {
                uint32_t Flags;
                uint64_t RootDirectory;     // HANDLE in the kernel, fixed 64-bit for the x64 layout
                uint32_t FileNameLength;
                wchar_t FileName[1u];
            };

            static_assert(sizeof(FILE_RENAME_INFORMATION_EX) == 24u, "trace::kernel::FILE_RENAME_INFORMATION_EX x64 layout drift");
            static_assert(offsetof(FILE_RENAME_INFORMATION_EX, RootDirectory) == 8u, "trace::kernel::FILE_RENAME_INFORMATION_EX x64 layout drift");
            static_assert(offsetof(FILE_RENAME_INFORMATION_EX, FileNameLength) == 16u, "trace::kernel::FILE_RENAME_INFORMATION_EX x64 layout drift");
            static_assert(offsetof(FILE_RENAME_INFORMATION_EX, FileName) == 20u, "trace::kernel::FILE_RENAME_INFORMATION_EX x64 layout drift");

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

            // FILE_RENAME_INFORMATION_EX flags, FILE_LINK_INFORMATION_EX shares the values
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

            // FS_INFORMATION_CLASS payload layouts

            struct FILE_FS_VOLUME_INFORMATION {
                int64_t VolumeCreationTime;
                uint32_t VolumeSerialNumber;
                uint32_t VolumeLabelLength;
                uint8_t SupportsObjects;
                wchar_t VolumeLabel[1u];
            };

            static_assert(sizeof(FILE_FS_VOLUME_INFORMATION) == 24u, "trace::kernel::FILE_FS_VOLUME_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_FS_VOLUME_INFORMATION, VolumeLabel) == 18u, "trace::kernel::FILE_FS_VOLUME_INFORMATION x64 layout drift");

            struct FILE_FS_LABEL_INFORMATION {
                uint32_t VolumeLabelLength;
                wchar_t VolumeLabel[1u];
            };

            static_assert(sizeof(FILE_FS_LABEL_INFORMATION) == 8u, "trace::kernel::FILE_FS_LABEL_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_FS_LABEL_INFORMATION, VolumeLabel) == 4u, "trace::kernel::FILE_FS_LABEL_INFORMATION x64 layout drift");

            struct FILE_FS_SIZE_INFORMATION {
                int64_t TotalAllocationUnits;
                int64_t AvailableAllocationUnits;
                uint32_t SectorsPerAllocationUnit;
                uint32_t BytesPerSector;
            };

            static_assert(sizeof(FILE_FS_SIZE_INFORMATION) == 24u, "trace::kernel::FILE_FS_SIZE_INFORMATION x64 layout drift");

            struct FILE_FS_DEVICE_INFORMATION {
                uint32_t DeviceType;
                uint32_t Characteristics;
            };

            static_assert(sizeof(FILE_FS_DEVICE_INFORMATION) == 8u, "trace::kernel::FILE_FS_DEVICE_INFORMATION x64 layout drift");

            struct FILE_FS_ATTRIBUTE_INFORMATION {
                uint32_t FileSystemAttributes;
                int32_t MaximumComponentNameLength;
                uint32_t FileSystemNameLength;
                wchar_t FileSystemName[1u];
            };

            static_assert(sizeof(FILE_FS_ATTRIBUTE_INFORMATION) == 16u, "trace::kernel::FILE_FS_ATTRIBUTE_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_FS_ATTRIBUTE_INFORMATION, FileSystemName) == 12u, "trace::kernel::FILE_FS_ATTRIBUTE_INFORMATION x64 layout drift");

            struct FILE_FS_FULL_SIZE_INFORMATION {
                int64_t TotalAllocationUnits;
                int64_t CallerAvailableAllocationUnits;
                int64_t ActualAvailableAllocationUnits;
                uint32_t SectorsPerAllocationUnit;
                uint32_t BytesPerSector;
            };

            static_assert(sizeof(FILE_FS_FULL_SIZE_INFORMATION) == 32u, "trace::kernel::FILE_FS_FULL_SIZE_INFORMATION x64 layout drift");

            struct FILE_FS_SECTOR_SIZE_INFORMATION {
                uint32_t LogicalBytesPerSector;
                uint32_t PhysicalBytesPerSectorForAtomicity;
                uint32_t PhysicalBytesPerSectorForPerformance;
                uint32_t FileSystemEffectivePhysicalBytesPerSectorForAtomicity;
                uint32_t Flags;
                uint32_t ByteOffsetForSectorAlignment;
                uint32_t ByteOffsetForPartitionAlignment;
            };

            static_assert(sizeof(FILE_FS_SECTOR_SIZE_INFORMATION) == 28u, "trace::kernel::FILE_FS_SECTOR_SIZE_INFORMATION x64 layout drift");

            // completion filter values
            inline constexpr uint32_t FILE_NOTIFY_CHANGE_EA           = 0x00000080u;
            inline constexpr uint32_t FILE_NOTIFY_CHANGE_STREAM_NAME  = 0x00000200u;
            inline constexpr uint32_t FILE_NOTIFY_CHANGE_STREAM_SIZE  = 0x00000400u;
            inline constexpr uint32_t FILE_NOTIFY_CHANGE_STREAM_WRITE = 0x00000800u;

            // DIRECTORY_NOTIFY_INFORMATION_CLASS codes
            inline constexpr uint32_t DirectoryNotifyInformation         = 1u;
            inline constexpr uint32_t DirectoryNotifyExtendedInformation = 2u;
            inline constexpr uint32_t DirectoryNotifyFullInformation     = 3u;

            // directory enumeration entry layouts

            struct FILE_DIRECTORY_INFORMATION {
                uint32_t NextEntryOffset;
                uint32_t FileIndex;
                int64_t CreationTime;
                int64_t LastAccessTime;
                int64_t LastWriteTime;
                int64_t ChangeTime;
                int64_t EndOfFile;
                int64_t AllocationSize;
                uint32_t FileAttributes;
                uint32_t FileNameLength;
                wchar_t FileName[1u];
            };

            static_assert(sizeof(FILE_DIRECTORY_INFORMATION) == 72u, "trace::kernel::FILE_DIRECTORY_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_DIRECTORY_INFORMATION, FileName) == 64u, "trace::kernel::FILE_DIRECTORY_INFORMATION x64 layout drift");

            struct FILE_FULL_DIR_INFORMATION {
                uint32_t NextEntryOffset;
                uint32_t FileIndex;
                int64_t CreationTime;
                int64_t LastAccessTime;
                int64_t LastWriteTime;
                int64_t ChangeTime;
                int64_t EndOfFile;
                int64_t AllocationSize;
                uint32_t FileAttributes;
                uint32_t FileNameLength;
                uint32_t EaSize;
                wchar_t FileName[1u];
            };

            static_assert(sizeof(FILE_FULL_DIR_INFORMATION) == 72u, "trace::kernel::FILE_FULL_DIR_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_FULL_DIR_INFORMATION, FileName) == 68u, "trace::kernel::FILE_FULL_DIR_INFORMATION x64 layout drift");

            struct FILE_BOTH_DIR_INFORMATION {
                uint32_t NextEntryOffset;
                uint32_t FileIndex;
                int64_t CreationTime;
                int64_t LastAccessTime;
                int64_t LastWriteTime;
                int64_t ChangeTime;
                int64_t EndOfFile;
                int64_t AllocationSize;
                uint32_t FileAttributes;
                uint32_t FileNameLength;
                uint32_t EaSize;
                int8_t ShortNameLength;
                wchar_t ShortName[12u];
                wchar_t FileName[1u];
            };

            static_assert(sizeof(FILE_BOTH_DIR_INFORMATION) == 96u, "trace::kernel::FILE_BOTH_DIR_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_BOTH_DIR_INFORMATION, FileName) == 94u, "trace::kernel::FILE_BOTH_DIR_INFORMATION x64 layout drift");

            struct FILE_NAMES_INFORMATION {
                uint32_t NextEntryOffset;
                uint32_t FileIndex;
                uint32_t FileNameLength;
                wchar_t FileName[1u];
            };

            static_assert(sizeof(FILE_NAMES_INFORMATION) == 16u, "trace::kernel::FILE_NAMES_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_NAMES_INFORMATION, FileName) == 12u, "trace::kernel::FILE_NAMES_INFORMATION x64 layout drift");

            struct FILE_ID_BOTH_DIR_INFORMATION {
                uint32_t NextEntryOffset;
                uint32_t FileIndex;
                int64_t CreationTime;
                int64_t LastAccessTime;
                int64_t LastWriteTime;
                int64_t ChangeTime;
                int64_t EndOfFile;
                int64_t AllocationSize;
                uint32_t FileAttributes;
                uint32_t FileNameLength;
                uint32_t EaSize;
                int8_t ShortNameLength;
                wchar_t ShortName[12u];
                int64_t FileId;
                wchar_t FileName[1u];
            };

            static_assert(sizeof(FILE_ID_BOTH_DIR_INFORMATION) == 112u, "trace::kernel::FILE_ID_BOTH_DIR_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_ID_BOTH_DIR_INFORMATION, FileId) == 96u, "trace::kernel::FILE_ID_BOTH_DIR_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_ID_BOTH_DIR_INFORMATION, FileName) == 104u, "trace::kernel::FILE_ID_BOTH_DIR_INFORMATION x64 layout drift");

            struct FILE_ID_FULL_DIR_INFORMATION {
                uint32_t NextEntryOffset;
                uint32_t FileIndex;
                int64_t CreationTime;
                int64_t LastAccessTime;
                int64_t LastWriteTime;
                int64_t ChangeTime;
                int64_t EndOfFile;
                int64_t AllocationSize;
                uint32_t FileAttributes;
                uint32_t FileNameLength;
                uint32_t EaSize;
                int64_t FileId;
                wchar_t FileName[1u];
            };

            static_assert(sizeof(FILE_ID_FULL_DIR_INFORMATION) == 88u, "trace::kernel::FILE_ID_FULL_DIR_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_ID_FULL_DIR_INFORMATION, FileId) == 72u, "trace::kernel::FILE_ID_FULL_DIR_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_ID_FULL_DIR_INFORMATION, FileName) == 80u, "trace::kernel::FILE_ID_FULL_DIR_INFORMATION x64 layout drift");

            // FSCTL payload layouts

            struct FILESYSTEM_STATISTICS {
                uint16_t FileSystemType;
                uint16_t Version;
                uint32_t SizeOfCompleteStructure;
                uint32_t UserFileReads;
                uint32_t UserFileReadBytes;
                uint32_t UserDiskReads;
                uint32_t UserFileWrites;
                uint32_t UserFileWriteBytes;
                uint32_t UserDiskWrites;
                uint32_t MetaDataReads;
                uint32_t MetaDataReadBytes;
                uint32_t MetaDataDiskReads;
                uint32_t MetaDataWrites;
                uint32_t MetaDataWriteBytes;
                uint32_t MetaDataDiskWrites;
            };

            static_assert(sizeof(FILESYSTEM_STATISTICS) == 56u, "trace::kernel::FILESYSTEM_STATISTICS x64 layout drift");
            static_assert(offsetof(FILESYSTEM_STATISTICS, UserFileReads) == 8u, "trace::kernel::FILESYSTEM_STATISTICS x64 layout drift");

            struct NTFS_VOLUME_DATA_BUFFER {
                uint64_t VolumeSerialNumber;
                int64_t NumberSectors;
                int64_t TotalClusters;
                int64_t FreeClusters;
                int64_t TotalReserved;
                uint32_t BytesPerSector;
                uint32_t BytesPerCluster;
                uint32_t BytesPerFileRecordSegment;
                uint32_t ClustersPerFileRecordSegment;
                int64_t MftValidDataLength;
                int64_t MftStartLcn;
                int64_t Mft2StartLcn;
                int64_t MftZoneStart;
                int64_t MftZoneEnd;
            };

            static_assert(sizeof(NTFS_VOLUME_DATA_BUFFER) == 96u, "trace::kernel::NTFS_VOLUME_DATA_BUFFER x64 layout drift");
            static_assert(offsetof(NTFS_VOLUME_DATA_BUFFER, MftValidDataLength) == 56u, "trace::kernel::NTFS_VOLUME_DATA_BUFFER x64 layout drift");

            struct MOVE_FILE_DATA {
                uint64_t FileHandle;    // HANDLE in the kernel
                int64_t StartingVcn;
                int64_t StartingLcn;
                uint32_t ClusterCount;
            };

            static_assert(sizeof(MOVE_FILE_DATA) == 32u, "trace::kernel::MOVE_FILE_DATA x64 layout drift");
            static_assert(offsetof(MOVE_FILE_DATA, ClusterCount) == 24u, "trace::kernel::MOVE_FILE_DATA x64 layout drift");

            struct FILE_OBJECTID_BUFFER {
                uint8_t ObjectId[16u];
                uint8_t BirthVolumeId[16u];
                uint8_t BirthObjectId[16u];
                uint8_t DomainId[16u];
            };

            static_assert(sizeof(FILE_OBJECTID_BUFFER) == 64u, "trace::kernel::FILE_OBJECTID_BUFFER x64 layout drift");
            static_assert(offsetof(FILE_OBJECTID_BUFFER, BirthVolumeId) == 16u, "trace::kernel::FILE_OBJECTID_BUFFER x64 layout drift");

            // symbolic link reparse point flag
            inline constexpr uint32_t SYMLINK_FLAG_RELATIVE = 0x00000001u;

            struct REPARSE_DATA_BUFFER {
                uint32_t ReparseTag;
                uint16_t ReparseDataLength;
                uint16_t Reserved;
                union {
                    struct {
                        uint16_t SubstituteNameOffset;
                        uint16_t SubstituteNameLength;
                        uint16_t PrintNameOffset;
                        uint16_t PrintNameLength;
                        uint32_t Flags;
                        wchar_t PathBuffer[1u];
                    } SymbolicLinkReparseBuffer;
                    struct {
                        uint16_t SubstituteNameOffset;
                        uint16_t SubstituteNameLength;
                        uint16_t PrintNameOffset;
                        uint16_t PrintNameLength;
                        wchar_t PathBuffer[1u];
                    } MountPointReparseBuffer;
                    struct {
                        uint8_t DataBuffer[1u];
                    } GenericReparseBuffer;
                };
            };

            static_assert(sizeof(REPARSE_DATA_BUFFER) == 24u, "trace::kernel::REPARSE_DATA_BUFFER x64 layout drift");
            static_assert(offsetof(REPARSE_DATA_BUFFER, SymbolicLinkReparseBuffer) == 8u, "trace::kernel::REPARSE_DATA_BUFFER x64 layout drift");
            static_assert(offsetof(REPARSE_DATA_BUFFER, SymbolicLinkReparseBuffer.PathBuffer) == 20u, "trace::kernel::REPARSE_DATA_BUFFER x64 layout drift");
            static_assert(offsetof(REPARSE_DATA_BUFFER, MountPointReparseBuffer.PathBuffer) == 16u, "trace::kernel::REPARSE_DATA_BUFFER x64 layout drift");

            struct READ_USN_JOURNAL_DATA_V0 {
                int64_t StartUsn;
                uint32_t ReasonMask;
                uint32_t ReturnOnlyOnClose;
                uint64_t Timeout;
                uint64_t BytesToWaitFor;
                uint64_t UsnJournalID;
            };

            static_assert(sizeof(READ_USN_JOURNAL_DATA_V0) == 40u, "trace::kernel::READ_USN_JOURNAL_DATA_V0 x64 layout drift");
            static_assert(offsetof(READ_USN_JOURNAL_DATA_V0, UsnJournalID) == 32u, "trace::kernel::READ_USN_JOURNAL_DATA_V0 x64 layout drift");

            struct USN_RECORD_V2 {
                uint32_t RecordLength;
                uint16_t MajorVersion;
                uint16_t MinorVersion;
                uint64_t FileReferenceNumber;
                uint64_t ParentFileReferenceNumber;
                int64_t Usn;
                int64_t TimeStamp;
                uint32_t Reason;
                uint32_t SourceInfo;
                uint32_t SecurityId;
                uint32_t FileAttributes;
                uint16_t FileNameLength;
                uint16_t FileNameOffset;
                wchar_t FileName[1u];
            };

            static_assert(sizeof(USN_RECORD_V2) == 64u, "trace::kernel::USN_RECORD_V2 x64 layout drift");
            static_assert(offsetof(USN_RECORD_V2, FileName) == 60u, "trace::kernel::USN_RECORD_V2 x64 layout drift");

            struct FILE_SET_SPARSE_BUFFER {
                uint8_t SetSparse;      // BOOLEAN in the kernel
            };

            static_assert(sizeof(FILE_SET_SPARSE_BUFFER) == 1u, "trace::kernel::FILE_SET_SPARSE_BUFFER x64 layout drift");

            struct FILE_ZERO_DATA_INFORMATION {
                int64_t FileOffset;
                int64_t BeyondFinalZero;
            };

            static_assert(sizeof(FILE_ZERO_DATA_INFORMATION) == 16u, "trace::kernel::FILE_ZERO_DATA_INFORMATION x64 layout drift");

            struct FILE_ALLOCATED_RANGE_BUFFER {
                int64_t FileOffset;
                int64_t Length;
            };

            static_assert(sizeof(FILE_ALLOCATED_RANGE_BUFFER) == 16u, "trace::kernel::FILE_ALLOCATED_RANGE_BUFFER x64 layout drift");

            struct CREATE_USN_JOURNAL_DATA {
                uint64_t MaximumSize;
                uint64_t AllocationDelta;
            };

            static_assert(sizeof(CREATE_USN_JOURNAL_DATA) == 16u, "trace::kernel::CREATE_USN_JOURNAL_DATA x64 layout drift");

            struct USN_JOURNAL_DATA_V0 {
                uint64_t UsnJournalID;
                int64_t FirstUsn;
                int64_t NextUsn;
                int64_t LowestValidUsn;
                int64_t MaxUsn;
                uint64_t MaximumSize;
                uint64_t AllocationDelta;
            };

            static_assert(sizeof(USN_JOURNAL_DATA_V0) == 56u, "trace::kernel::USN_JOURNAL_DATA_V0 x64 layout drift");
            static_assert(offsetof(USN_JOURNAL_DATA_V0, AllocationDelta) == 48u, "trace::kernel::USN_JOURNAL_DATA_V0 x64 layout drift");

            struct FILE_LEVEL_TRIM_RANGE {
                uint64_t Offset;
                uint64_t Length;
            };

            static_assert(sizeof(FILE_LEVEL_TRIM_RANGE) == 16u, "trace::kernel::FILE_LEVEL_TRIM_RANGE x64 layout drift");

            struct FILE_LEVEL_TRIM {
                uint32_t Key;
                uint32_t NumRanges;
                FILE_LEVEL_TRIM_RANGE Ranges[1u];
            };

            static_assert(sizeof(FILE_LEVEL_TRIM) == 24u, "trace::kernel::FILE_LEVEL_TRIM x64 layout drift");
            static_assert(offsetof(FILE_LEVEL_TRIM, Ranges) == 8u, "trace::kernel::FILE_LEVEL_TRIM x64 layout drift");

            struct REQUEST_OPLOCK_INPUT_BUFFER {
                uint16_t StructureVersion;
                uint16_t StructureLength;
                uint32_t RequestedOplockLevel;
                uint32_t Flags;
            };

            static_assert(sizeof(REQUEST_OPLOCK_INPUT_BUFFER) == 12u, "trace::kernel::REQUEST_OPLOCK_INPUT_BUFFER x64 layout drift");
            static_assert(offsetof(REQUEST_OPLOCK_INPUT_BUFFER, RequestedOplockLevel) == 4u, "trace::kernel::REQUEST_OPLOCK_INPUT_BUFFER x64 layout drift");

            struct REQUEST_OPLOCK_OUTPUT_BUFFER {
                uint16_t StructureVersion;
                uint16_t StructureLength;
                uint32_t OriginalOplockLevel;
                uint32_t NewOplockLevel;
                uint32_t Flags;
                uint32_t AccessMode;    // ACCESS_MASK in the kernel
                uint16_t ShareMode;
            };

            static_assert(sizeof(REQUEST_OPLOCK_OUTPUT_BUFFER) == 24u, "trace::kernel::REQUEST_OPLOCK_OUTPUT_BUFFER x64 layout drift");
            static_assert(offsetof(REQUEST_OPLOCK_OUTPUT_BUFFER, AccessMode) == 16u, "trace::kernel::REQUEST_OPLOCK_OUTPUT_BUFFER x64 layout drift");

            struct FILE_REGION_INPUT {
                int64_t FileOffset;
                int64_t Length;
                uint32_t DesiredUsage;
            };

            static_assert(sizeof(FILE_REGION_INPUT) == 24u, "trace::kernel::FILE_REGION_INPUT x64 layout drift");
            static_assert(offsetof(FILE_REGION_INPUT, DesiredUsage) == 16u, "trace::kernel::FILE_REGION_INPUT x64 layout drift");

            struct FILE_REGION_INFO {
                int64_t FileOffset;
                int64_t Length;
                uint32_t Usage;
                uint32_t Reserved;
            };

            static_assert(sizeof(FILE_REGION_INFO) == 24u, "trace::kernel::FILE_REGION_INFO x64 layout drift");

            struct FILE_REGION_OUTPUT {
                uint32_t Flags;
                uint32_t TotalRegionEntryCount;
                uint32_t RegionEntryCount;
                uint32_t Reserved;
                FILE_REGION_INFO Region[1u];
            };

            static_assert(sizeof(FILE_REGION_OUTPUT) == 40u, "trace::kernel::FILE_REGION_OUTPUT x64 layout drift");
            static_assert(offsetof(FILE_REGION_OUTPUT, Region) == 16u, "trace::kernel::FILE_REGION_OUTPUT x64 layout drift");

            struct DUPLICATE_EXTENTS_DATA {
                uint64_t FileHandle;    // HANDLE in the kernel
                int64_t SourceFileOffset;
                int64_t TargetFileOffset;
                int64_t ByteCount;
            };

            static_assert(sizeof(DUPLICATE_EXTENTS_DATA) == 32u, "trace::kernel::DUPLICATE_EXTENTS_DATA x64 layout drift");
            static_assert(offsetof(DUPLICATE_EXTENTS_DATA, SourceFileOffset) == 8u, "trace::kernel::DUPLICATE_EXTENTS_DATA x64 layout drift");

            struct FILESYSTEM_STATISTICS_EX {
                uint16_t FileSystemType;
                uint16_t Version;
                uint32_t SizeOfCompleteStructure;
                uint64_t UserFileReads;
                uint64_t UserFileReadBytes;
                uint64_t UserDiskReads;
                uint64_t UserFileWrites;
                uint64_t UserFileWriteBytes;
                uint64_t UserDiskWrites;
                uint64_t MetaDataReads;
                uint64_t MetaDataReadBytes;
                uint64_t MetaDataDiskReads;
                uint64_t MetaDataWrites;
                uint64_t MetaDataWriteBytes;
                uint64_t MetaDataDiskWrites;
            };

            static_assert(sizeof(FILESYSTEM_STATISTICS_EX) == 104u, "trace::kernel::FILESYSTEM_STATISTICS_EX x64 layout drift");
            static_assert(offsetof(FILESYSTEM_STATISTICS_EX, UserFileReads) == 8u, "trace::kernel::FILESYSTEM_STATISTICS_EX x64 layout drift");

            // IOCTL payload layouts

            struct DISK_GEOMETRY {
                int64_t Cylinders;
                uint32_t MediaType;
                uint32_t TracksPerCylinder;
                uint32_t SectorsPerTrack;
                uint32_t BytesPerSector;
            };

            static_assert(sizeof(DISK_GEOMETRY) == 24u, "trace::kernel::DISK_GEOMETRY x64 layout drift");
            static_assert(offsetof(DISK_GEOMETRY, MediaType) == 8u, "trace::kernel::DISK_GEOMETRY x64 layout drift");

            struct DISK_GEOMETRY_EX {
                DISK_GEOMETRY Geometry;
                int64_t DiskSize;
                uint8_t Data[1u];
            };

            static_assert(sizeof(DISK_GEOMETRY_EX) == 40u, "trace::kernel::DISK_GEOMETRY_EX x64 layout drift");
            static_assert(offsetof(DISK_GEOMETRY_EX, DiskSize) == 24u, "trace::kernel::DISK_GEOMETRY_EX x64 layout drift");
            static_assert(offsetof(DISK_GEOMETRY_EX, Data) == 32u, "trace::kernel::DISK_GEOMETRY_EX x64 layout drift");

            struct GET_LENGTH_INFORMATION {
                int64_t Length;
            };

            static_assert(sizeof(GET_LENGTH_INFORMATION) == 8u, "trace::kernel::GET_LENGTH_INFORMATION x64 layout drift");

            struct STORAGE_DEVICE_NUMBER {
                uint32_t DeviceType;    // DEVICE_TYPE in the kernel
                uint32_t DeviceNumber;
                uint32_t PartitionNumber;
            };

            static_assert(sizeof(STORAGE_DEVICE_NUMBER) == 12u, "trace::kernel::STORAGE_DEVICE_NUMBER x64 layout drift");

            struct STORAGE_PROPERTY_QUERY {
                uint32_t PropertyId;
                uint32_t QueryType;
                uint8_t AdditionalParameters[1u];
            };

            static_assert(sizeof(STORAGE_PROPERTY_QUERY) == 12u, "trace::kernel::STORAGE_PROPERTY_QUERY x64 layout drift");
            static_assert(offsetof(STORAGE_PROPERTY_QUERY, AdditionalParameters) == 8u, "trace::kernel::STORAGE_PROPERTY_QUERY x64 layout drift");

            struct STORAGE_DESCRIPTOR_HEADER {
                uint32_t Version;
                uint32_t Size;
            };

            static_assert(sizeof(STORAGE_DESCRIPTOR_HEADER) == 8u, "trace::kernel::STORAGE_DESCRIPTOR_HEADER x64 layout drift");

            struct DISK_EXTENT {
                uint32_t DiskNumber;
                int64_t StartingOffset;
                int64_t ExtentLength;
            };

            static_assert(sizeof(DISK_EXTENT) == 24u, "trace::kernel::DISK_EXTENT x64 layout drift");
            static_assert(offsetof(DISK_EXTENT, StartingOffset) == 8u, "trace::kernel::DISK_EXTENT x64 layout drift");

            struct VOLUME_DISK_EXTENTS {
                uint32_t NumberOfDiskExtents;
                DISK_EXTENT Extents[1u];
            };

            static_assert(sizeof(VOLUME_DISK_EXTENTS) == 32u, "trace::kernel::VOLUME_DISK_EXTENTS x64 layout drift");
            static_assert(offsetof(VOLUME_DISK_EXTENTS, Extents) == 8u, "trace::kernel::VOLUME_DISK_EXTENTS x64 layout drift");

        }

    }

}
