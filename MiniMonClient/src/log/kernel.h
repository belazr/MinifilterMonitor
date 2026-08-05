#pragma once

#include <cstddef>
#include <cstdint>

namespace mimo {

    namespace log {

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

            // directory control stack-location SL_* flags carried in RecordData::operationFlags
            inline constexpr uint8_t SL_RESTART_SCAN        = 0x01u;
            inline constexpr uint8_t SL_RETURN_SINGLE_ENTRY = 0x02u;
            inline constexpr uint8_t SL_INDEX_SPECIFIED     = 0x04u;

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
            inline constexpr uint8_t IRP_MN_TRACK_LINK       = 0x04u;

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

            static_assert(sizeof(FILE_BASIC_INFORMATION) == 40u, "kernel::FILE_BASIC_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_BASIC_INFORMATION, FileAttributes) == 32u, "kernel::FILE_BASIC_INFORMATION x64 layout drift");

            struct FILE_STANDARD_INFORMATION {
                int64_t AllocationSize;
                int64_t EndOfFile;
                uint32_t NumberOfLinks;
                uint8_t DeletePending;
                uint8_t Directory;
            };

            static_assert(sizeof(FILE_STANDARD_INFORMATION) == 24u, "kernel::FILE_STANDARD_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_STANDARD_INFORMATION, DeletePending) == 20u, "kernel::FILE_STANDARD_INFORMATION x64 layout drift");

            struct FILE_INTERNAL_INFORMATION {
                int64_t IndexNumber;
            };

            static_assert(sizeof(FILE_INTERNAL_INFORMATION) == 8u, "kernel::FILE_INTERNAL_INFORMATION x64 layout drift");

            struct FILE_EA_INFORMATION {
                uint32_t EaSize;
            };

            static_assert(sizeof(FILE_EA_INFORMATION) == 4u, "kernel::FILE_EA_INFORMATION x64 layout drift");

            struct FILE_ACCESS_INFORMATION {
                uint32_t AccessFlags;
            };

            static_assert(sizeof(FILE_ACCESS_INFORMATION) == 4u, "kernel::FILE_ACCESS_INFORMATION x64 layout drift");

            struct FILE_NAME_INFORMATION {
                uint32_t FileNameLength;
                wchar_t FileName[1u];
            };

            static_assert(sizeof(FILE_NAME_INFORMATION) == 8u, "kernel::FILE_NAME_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_NAME_INFORMATION, FileName) == 4u, "kernel::FILE_NAME_INFORMATION x64 layout drift");

            struct FILE_POSITION_INFORMATION {
                int64_t CurrentByteOffset;
            };

            static_assert(sizeof(FILE_POSITION_INFORMATION) == 8u, "kernel::FILE_POSITION_INFORMATION x64 layout drift");

            struct FILE_MODE_INFORMATION {
                uint32_t Mode;
            };

            static_assert(sizeof(FILE_MODE_INFORMATION) == 4u, "kernel::FILE_MODE_INFORMATION x64 layout drift");

            struct FILE_ALIGNMENT_INFORMATION {
                uint32_t AlignmentRequirement;
            };

            static_assert(sizeof(FILE_ALIGNMENT_INFORMATION) == 4u, "kernel::FILE_ALIGNMENT_INFORMATION x64 layout drift");

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

            static_assert(sizeof(FILE_ALL_INFORMATION) == 104u, "kernel::FILE_ALL_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_ALL_INFORMATION, StandardInformation) == 40u, "kernel::FILE_ALL_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_ALL_INFORMATION, PositionInformation) == 80u, "kernel::FILE_ALL_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_ALL_INFORMATION, NameInformation) == 96u, "kernel::FILE_ALL_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_ALL_INFORMATION, NameInformation.FileName) == 100u, "kernel::FILE_ALL_INFORMATION x64 layout drift");

            struct FILE_STREAM_INFORMATION {
                uint32_t NextEntryOffset;
                uint32_t StreamNameLength;
                int64_t StreamSize;
                int64_t StreamAllocationSize;
                wchar_t StreamName[1u];
            };

            static_assert(sizeof(FILE_STREAM_INFORMATION) == 32u, "kernel::FILE_STREAM_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_STREAM_INFORMATION, StreamName) == 24u, "kernel::FILE_STREAM_INFORMATION x64 layout drift");

            struct FILE_COMPRESSION_INFORMATION {
                int64_t CompressedFileSize;
                uint16_t CompressionFormat;
                uint8_t CompressionUnitShift;
                uint8_t ChunkShift;
                uint8_t ClusterShift;
                uint8_t Reserved[3u];
            };

            static_assert(sizeof(FILE_COMPRESSION_INFORMATION) == 16u, "kernel::FILE_COMPRESSION_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_COMPRESSION_INFORMATION, CompressionFormat) == 8u, "kernel::FILE_COMPRESSION_INFORMATION x64 layout drift");

            struct FILE_NETWORK_OPEN_INFORMATION {
                int64_t CreationTime;
                int64_t LastAccessTime;
                int64_t LastWriteTime;
                int64_t ChangeTime;
                int64_t AllocationSize;
                int64_t EndOfFile;
                uint32_t FileAttributes;
            };

            static_assert(sizeof(FILE_NETWORK_OPEN_INFORMATION) == 56u, "kernel::FILE_NETWORK_OPEN_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_NETWORK_OPEN_INFORMATION, FileAttributes) == 48u, "kernel::FILE_NETWORK_OPEN_INFORMATION x64 layout drift");

            struct FILE_ATTRIBUTE_TAG_INFORMATION {
                uint32_t FileAttributes;
                uint32_t ReparseTag;
            };

            static_assert(sizeof(FILE_ATTRIBUTE_TAG_INFORMATION) == 8u, "kernel::FILE_ATTRIBUTE_TAG_INFORMATION x64 layout drift");

            struct FILE_LINK_ENTRY_INFORMATION {
                uint32_t NextEntryOffset;
                int64_t ParentFileId;
                uint32_t FileNameLength;    // character count
                wchar_t FileName[1u];
            };

            static_assert(sizeof(FILE_LINK_ENTRY_INFORMATION) == 24u, "kernel::FILE_LINK_ENTRY_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_LINK_ENTRY_INFORMATION, ParentFileId) == 8u, "kernel::FILE_LINK_ENTRY_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_LINK_ENTRY_INFORMATION, FileName) == 20u, "kernel::FILE_LINK_ENTRY_INFORMATION x64 layout drift");

            struct FILE_LINKS_INFORMATION {
                uint32_t BytesNeeded;
                uint32_t EntriesReturned;
                FILE_LINK_ENTRY_INFORMATION Entry;
            };

            static_assert(sizeof(FILE_LINKS_INFORMATION) == 32u, "kernel::FILE_LINKS_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_LINKS_INFORMATION, Entry) == 8u, "kernel::FILE_LINKS_INFORMATION x64 layout drift");

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

            static_assert(sizeof(FILE_REMOTE_PROTOCOL_INFORMATION) == 116u, "kernel::FILE_REMOTE_PROTOCOL_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_REMOTE_PROTOCOL_INFORMATION, Flags) == 16u, "kernel::FILE_REMOTE_PROTOCOL_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_REMOTE_PROTOCOL_INFORMATION, GenericReserved) == 20u, "kernel::FILE_REMOTE_PROTOCOL_INFORMATION x64 layout drift");

            struct FILE_ID_INFORMATION {
                uint64_t VolumeSerialNumber;
                uint8_t FileId[16u];
            };

            static_assert(sizeof(FILE_ID_INFORMATION) == 24u, "kernel::FILE_ID_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_ID_INFORMATION, FileId) == 8u, "kernel::FILE_ID_INFORMATION x64 layout drift");

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

            static_assert(sizeof(FILE_STAT_INFORMATION) == 72u, "kernel::FILE_STAT_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_STAT_INFORMATION, FileAttributes) == 56u, "kernel::FILE_STAT_INFORMATION x64 layout drift");

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

            static_assert(sizeof(FILE_STAT_LX_INFORMATION) == 96u, "kernel::FILE_STAT_LX_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_STAT_LX_INFORMATION, LxFlags) == sizeof(FILE_STAT_INFORMATION), "kernel::FILE_STAT_LX_INFORMATION does not extend FILE_STAT_INFORMATION");

            struct FILE_CASE_SENSITIVE_INFORMATION {
                uint32_t Flags;
            };

            static_assert(sizeof(FILE_CASE_SENSITIVE_INFORMATION) == 4u, "kernel::FILE_CASE_SENSITIVE_INFORMATION x64 layout drift");

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
                int64_t VolumeSerialNumber;
                uint8_t FileId128[16u];
            };

            static_assert(sizeof(FILE_STAT_BASIC_INFORMATION) == 104u, "kernel::FILE_STAT_BASIC_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_STAT_BASIC_INFORMATION, DeviceType) == 68u, "kernel::FILE_STAT_BASIC_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_STAT_BASIC_INFORMATION, FileId128) == 88u, "kernel::FILE_STAT_BASIC_INFORMATION x64 layout drift");

            // FILE_LINK_INFORMATION shares this layout
            struct FILE_RENAME_INFORMATION {
                uint8_t ReplaceIfExists;
                uint64_t RootDirectory;     // HANDLE in the kernel, fixed 64-bit for the x64 layout
                uint32_t FileNameLength;
                wchar_t FileName[1u];
            };

            static_assert(sizeof(FILE_RENAME_INFORMATION) == 24u, "kernel::FILE_RENAME_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_RENAME_INFORMATION, RootDirectory) == 8u, "kernel::FILE_RENAME_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_RENAME_INFORMATION, FileNameLength) == 16u, "kernel::FILE_RENAME_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_RENAME_INFORMATION, FileName) == 20u, "kernel::FILE_RENAME_INFORMATION x64 layout drift");

            struct FILE_DISPOSITION_INFORMATION {
                uint8_t DeletePending;      // MS-FSCC spelling, the ntifs.h member name DeleteFile is a winbase.h macro
            };

            static_assert(sizeof(FILE_DISPOSITION_INFORMATION) == 1u, "kernel::FILE_DISPOSITION_INFORMATION x64 layout drift");

            struct FILE_ALLOCATION_INFORMATION {
                int64_t AllocationSize;
            };

            static_assert(sizeof(FILE_ALLOCATION_INFORMATION) == 8u, "kernel::FILE_ALLOCATION_INFORMATION x64 layout drift");

            struct FILE_END_OF_FILE_INFORMATION {
                int64_t EndOfFile;
            };

            static_assert(sizeof(FILE_END_OF_FILE_INFORMATION) == 8u, "kernel::FILE_END_OF_FILE_INFORMATION x64 layout drift");

            struct FILE_VALID_DATA_LENGTH_INFORMATION {
                int64_t ValidDataLength;
            };

            static_assert(sizeof(FILE_VALID_DATA_LENGTH_INFORMATION) == 8u, "kernel::FILE_VALID_DATA_LENGTH_INFORMATION x64 layout drift");

            struct FILE_DISPOSITION_INFORMATION_EX {
                uint32_t Flags;
            };

            static_assert(sizeof(FILE_DISPOSITION_INFORMATION_EX) == 4u, "kernel::FILE_DISPOSITION_INFORMATION_EX x64 layout drift");

            // FILE_LINK_INFORMATION_EX shares this layout
            struct FILE_RENAME_INFORMATION_EX {
                uint32_t Flags;
                uint64_t RootDirectory;     // HANDLE in the kernel, fixed 64-bit for the x64 layout
                uint32_t FileNameLength;
                wchar_t FileName[1u];
            };

            static_assert(sizeof(FILE_RENAME_INFORMATION_EX) == 24u, "kernel::FILE_RENAME_INFORMATION_EX x64 layout drift");
            static_assert(offsetof(FILE_RENAME_INFORMATION_EX, RootDirectory) == 8u, "kernel::FILE_RENAME_INFORMATION_EX x64 layout drift");
            static_assert(offsetof(FILE_RENAME_INFORMATION_EX, FileNameLength) == 16u, "kernel::FILE_RENAME_INFORMATION_EX x64 layout drift");
            static_assert(offsetof(FILE_RENAME_INFORMATION_EX, FileName) == 20u, "kernel::FILE_RENAME_INFORMATION_EX x64 layout drift");

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

            static_assert(sizeof(FILE_FS_VOLUME_INFORMATION) == 24u, "kernel::FILE_FS_VOLUME_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_FS_VOLUME_INFORMATION, VolumeLabel) == 18u, "kernel::FILE_FS_VOLUME_INFORMATION x64 layout drift");

            struct FILE_FS_LABEL_INFORMATION {
                uint32_t VolumeLabelLength;
                wchar_t VolumeLabel[1u];
            };

            static_assert(sizeof(FILE_FS_LABEL_INFORMATION) == 8u, "kernel::FILE_FS_LABEL_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_FS_LABEL_INFORMATION, VolumeLabel) == 4u, "kernel::FILE_FS_LABEL_INFORMATION x64 layout drift");

            struct FILE_FS_SIZE_INFORMATION {
                int64_t TotalAllocationUnits;
                int64_t AvailableAllocationUnits;
                uint32_t SectorsPerAllocationUnit;
                uint32_t BytesPerSector;
            };

            static_assert(sizeof(FILE_FS_SIZE_INFORMATION) == 24u, "kernel::FILE_FS_SIZE_INFORMATION x64 layout drift");

            struct FILE_FS_DEVICE_INFORMATION {
                uint32_t DeviceType;
                uint32_t Characteristics;
            };

            static_assert(sizeof(FILE_FS_DEVICE_INFORMATION) == 8u, "kernel::FILE_FS_DEVICE_INFORMATION x64 layout drift");

            struct FILE_FS_ATTRIBUTE_INFORMATION {
                uint32_t FileSystemAttributes;
                int32_t MaximumComponentNameLength;
                uint32_t FileSystemNameLength;
                wchar_t FileSystemName[1u];
            };

            static_assert(sizeof(FILE_FS_ATTRIBUTE_INFORMATION) == 16u, "kernel::FILE_FS_ATTRIBUTE_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_FS_ATTRIBUTE_INFORMATION, FileSystemName) == 12u, "kernel::FILE_FS_ATTRIBUTE_INFORMATION x64 layout drift");

            struct FILE_FS_FULL_SIZE_INFORMATION {
                int64_t TotalAllocationUnits;
                int64_t CallerAvailableAllocationUnits;
                int64_t ActualAvailableAllocationUnits;
                uint32_t SectorsPerAllocationUnit;
                uint32_t BytesPerSector;
            };

            static_assert(sizeof(FILE_FS_FULL_SIZE_INFORMATION) == 32u, "kernel::FILE_FS_FULL_SIZE_INFORMATION x64 layout drift");

            struct FILE_FS_SECTOR_SIZE_INFORMATION {
                uint32_t LogicalBytesPerSector;
                uint32_t PhysicalBytesPerSectorForAtomicity;
                uint32_t PhysicalBytesPerSectorForPerformance;
                uint32_t FileSystemEffectivePhysicalBytesPerSectorForAtomicity;
                uint32_t Flags;
                uint32_t ByteOffsetForSectorAlignment;
                uint32_t ByteOffsetForPartitionAlignment;
            };

            static_assert(sizeof(FILE_FS_SECTOR_SIZE_INFORMATION) == 28u, "kernel::FILE_FS_SECTOR_SIZE_INFORMATION x64 layout drift");

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

            static_assert(sizeof(FILE_DIRECTORY_INFORMATION) == 72u, "kernel::FILE_DIRECTORY_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_DIRECTORY_INFORMATION, FileName) == 64u, "kernel::FILE_DIRECTORY_INFORMATION x64 layout drift");

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

            static_assert(sizeof(FILE_FULL_DIR_INFORMATION) == 72u, "kernel::FILE_FULL_DIR_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_FULL_DIR_INFORMATION, FileName) == 68u, "kernel::FILE_FULL_DIR_INFORMATION x64 layout drift");

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

            static_assert(sizeof(FILE_BOTH_DIR_INFORMATION) == 96u, "kernel::FILE_BOTH_DIR_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_BOTH_DIR_INFORMATION, FileName) == 94u, "kernel::FILE_BOTH_DIR_INFORMATION x64 layout drift");

            struct FILE_NAMES_INFORMATION {
                uint32_t NextEntryOffset;
                uint32_t FileIndex;
                uint32_t FileNameLength;
                wchar_t FileName[1u];
            };

            static_assert(sizeof(FILE_NAMES_INFORMATION) == 16u, "kernel::FILE_NAMES_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_NAMES_INFORMATION, FileName) == 12u, "kernel::FILE_NAMES_INFORMATION x64 layout drift");

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

            static_assert(sizeof(FILE_ID_BOTH_DIR_INFORMATION) == 112u, "kernel::FILE_ID_BOTH_DIR_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_ID_BOTH_DIR_INFORMATION, FileId) == 96u, "kernel::FILE_ID_BOTH_DIR_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_ID_BOTH_DIR_INFORMATION, FileName) == 104u, "kernel::FILE_ID_BOTH_DIR_INFORMATION x64 layout drift");

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

            static_assert(sizeof(FILE_ID_FULL_DIR_INFORMATION) == 88u, "kernel::FILE_ID_FULL_DIR_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_ID_FULL_DIR_INFORMATION, FileId) == 72u, "kernel::FILE_ID_FULL_DIR_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_ID_FULL_DIR_INFORMATION, FileName) == 80u, "kernel::FILE_ID_FULL_DIR_INFORMATION x64 layout drift");

        }

    }

}
