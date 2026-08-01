#pragma once

#include <Windows.h>

#include <cstddef>

namespace mimo {

    namespace kernel {

        // operation category flags carried in RecordData::flags
        inline constexpr ULONG FLT_CALLBACK_DATA_IRP_OPERATION       = 0x00000001u;
        inline constexpr ULONG FLT_CALLBACK_DATA_FAST_IO_OPERATION   = 0x00000002u;
        inline constexpr ULONG FLT_CALLBACK_DATA_FS_FILTER_OPERATION = 0x00000004u;

        // IRP flags carried in RecordData::irpFlags
        inline constexpr ULONG IRP_NOCACHE                 = 0x00000001u;
        inline constexpr ULONG IRP_PAGING_IO               = 0x00000002u;
        inline constexpr ULONG IRP_SYNCHRONOUS_API         = 0x00000004u;
        inline constexpr ULONG IRP_SYNCHRONOUS_PAGING_IO   = 0x00000040u;

        // stack-location SL_* flags carried in RecordData::operationFlags
        inline constexpr UCHAR SL_KEY_SPECIFIED = 0x01u;
        inline constexpr UCHAR SL_WRITE_THROUGH = 0x04u;

        // standard IRP major codes
        inline constexpr UCHAR IRP_MJ_CREATE                   = 0x00u;
        inline constexpr UCHAR IRP_MJ_CREATE_NAMED_PIPE        = 0x01u;
        inline constexpr UCHAR IRP_MJ_CLOSE                    = 0x02u;
        inline constexpr UCHAR IRP_MJ_READ                     = 0x03u;
        inline constexpr UCHAR IRP_MJ_WRITE                    = 0x04u;
        inline constexpr UCHAR IRP_MJ_QUERY_INFORMATION        = 0x05u;
        inline constexpr UCHAR IRP_MJ_SET_INFORMATION          = 0x06u;
        inline constexpr UCHAR IRP_MJ_QUERY_EA                 = 0x07u;
        inline constexpr UCHAR IRP_MJ_SET_EA                   = 0x08u;
        inline constexpr UCHAR IRP_MJ_FLUSH_BUFFERS            = 0x09u;
        inline constexpr UCHAR IRP_MJ_QUERY_VOLUME_INFORMATION = 0x0Au;
        inline constexpr UCHAR IRP_MJ_SET_VOLUME_INFORMATION   = 0x0Bu;
        inline constexpr UCHAR IRP_MJ_DIRECTORY_CONTROL        = 0x0Cu;
        inline constexpr UCHAR IRP_MJ_FILE_SYSTEM_CONTROL      = 0x0Du;
        inline constexpr UCHAR IRP_MJ_DEVICE_CONTROL           = 0x0Eu;
        inline constexpr UCHAR IRP_MJ_INTERNAL_DEVICE_CONTROL  = 0x0Fu;
        inline constexpr UCHAR IRP_MJ_SHUTDOWN                 = 0x10u;
        inline constexpr UCHAR IRP_MJ_LOCK_CONTROL             = 0x11u;
        inline constexpr UCHAR IRP_MJ_CLEANUP                  = 0x12u;
        inline constexpr UCHAR IRP_MJ_CREATE_MAILSLOT          = 0x13u;
        inline constexpr UCHAR IRP_MJ_QUERY_SECURITY           = 0x14u;
        inline constexpr UCHAR IRP_MJ_SET_SECURITY             = 0x15u;
        inline constexpr UCHAR IRP_MJ_POWER                    = 0x16u;
        inline constexpr UCHAR IRP_MJ_SYSTEM_CONTROL           = 0x17u;
        inline constexpr UCHAR IRP_MJ_DEVICE_CHANGE            = 0x18u;
        inline constexpr UCHAR IRP_MJ_QUERY_QUOTA              = 0x19u;
        inline constexpr UCHAR IRP_MJ_SET_QUOTA                = 0x1Au;
        inline constexpr UCHAR IRP_MJ_PNP                      = 0x1Bu;
        inline constexpr UCHAR IRP_MJ_MAXIMUM_FUNCTION         = 0x1Bu;

        // FltMgr extension major codes
        inline constexpr UCHAR IRP_MJ_ACQUIRE_FOR_SECTION_SYNCHRONIZATION = static_cast<UCHAR>(-1);
        inline constexpr UCHAR IRP_MJ_RELEASE_FOR_SECTION_SYNCHRONIZATION = static_cast<UCHAR>(-2);
        inline constexpr UCHAR IRP_MJ_ACQUIRE_FOR_MOD_WRITE               = static_cast<UCHAR>(-3);
        inline constexpr UCHAR IRP_MJ_RELEASE_FOR_MOD_WRITE               = static_cast<UCHAR>(-4);
        inline constexpr UCHAR IRP_MJ_ACQUIRE_FOR_CC_FLUSH                = static_cast<UCHAR>(-5);
        inline constexpr UCHAR IRP_MJ_RELEASE_FOR_CC_FLUSH                = static_cast<UCHAR>(-6);
        inline constexpr UCHAR IRP_MJ_QUERY_OPEN                          = static_cast<UCHAR>(-7);

        inline constexpr UCHAR IRP_MJ_FAST_IO_CHECK_IF_POSSIBLE           = static_cast<UCHAR>(-13);
        inline constexpr UCHAR IRP_MJ_NETWORK_QUERY_OPEN                  = static_cast<UCHAR>(-14);
        inline constexpr UCHAR IRP_MJ_MDL_READ                            = static_cast<UCHAR>(-15);
        inline constexpr UCHAR IRP_MJ_MDL_READ_COMPLETE                   = static_cast<UCHAR>(-16);
        inline constexpr UCHAR IRP_MJ_PREPARE_MDL_WRITE                   = static_cast<UCHAR>(-17);
        inline constexpr UCHAR IRP_MJ_MDL_WRITE_COMPLETE                  = static_cast<UCHAR>(-18);
        inline constexpr UCHAR IRP_MJ_VOLUME_MOUNT                        = static_cast<UCHAR>(-19);
        inline constexpr UCHAR IRP_MJ_VOLUME_DISMOUNT                     = static_cast<UCHAR>(-20);

        // IRP minor codes, meaning is scoped by the major code

        // directory control
        inline constexpr UCHAR IRP_MN_QUERY_DIRECTORY         = 0x01u;
        inline constexpr UCHAR IRP_MN_NOTIFY_CHANGE_DIRECTORY = 0x02u;

        // file system control
        inline constexpr UCHAR IRP_MN_USER_FS_REQUEST  = 0x00u;
        inline constexpr UCHAR IRP_MN_MOUNT_VOLUME     = 0x01u;
        inline constexpr UCHAR IRP_MN_VERIFY_VOLUME    = 0x02u;
        inline constexpr UCHAR IRP_MN_LOAD_FILE_SYSTEM = 0x03u;
        inline constexpr UCHAR IRP_MN_TRACK_LINK       = 0x04u;

        // lock control
        inline constexpr UCHAR IRP_MN_LOCK              = 0x01u;
        inline constexpr UCHAR IRP_MN_UNLOCK_SINGLE     = 0x02u;
        inline constexpr UCHAR IRP_MN_UNLOCK_ALL        = 0x03u;
        inline constexpr UCHAR IRP_MN_UNLOCK_ALL_BY_KEY = 0x04u;

        // read/write
        inline constexpr UCHAR IRP_MN_NORMAL           = 0x00u;
        inline constexpr UCHAR IRP_MN_DPC              = 0x01u;
        inline constexpr UCHAR IRP_MN_MDL              = 0x02u;
        inline constexpr UCHAR IRP_MN_COMPLETE         = 0x04u;
        inline constexpr UCHAR IRP_MN_COMPRESSED       = 0x08u;
        inline constexpr UCHAR IRP_MN_MDL_DPC          = IRP_MN_MDL | IRP_MN_DPC;
        inline constexpr UCHAR IRP_MN_COMPLETE_MDL     = IRP_MN_COMPLETE | IRP_MN_MDL;
        inline constexpr UCHAR IRP_MN_COMPLETE_MDL_DPC = IRP_MN_COMPLETE_MDL | IRP_MN_DPC;

        // device control
        inline constexpr UCHAR IRP_MN_SCSI_CLASS = 0x01u;

        // PnP
        inline constexpr UCHAR IRP_MN_START_DEVICE                 = 0x00u;
        inline constexpr UCHAR IRP_MN_QUERY_REMOVE_DEVICE          = 0x01u;
        inline constexpr UCHAR IRP_MN_REMOVE_DEVICE                = 0x02u;
        inline constexpr UCHAR IRP_MN_CANCEL_REMOVE_DEVICE         = 0x03u;
        inline constexpr UCHAR IRP_MN_STOP_DEVICE                  = 0x04u;
        inline constexpr UCHAR IRP_MN_QUERY_STOP_DEVICE            = 0x05u;
        inline constexpr UCHAR IRP_MN_CANCEL_STOP_DEVICE           = 0x06u;
        inline constexpr UCHAR IRP_MN_QUERY_DEVICE_RELATIONS       = 0x07u;
        inline constexpr UCHAR IRP_MN_QUERY_INTERFACE              = 0x08u;
        inline constexpr UCHAR IRP_MN_QUERY_CAPABILITIES           = 0x09u;
        inline constexpr UCHAR IRP_MN_QUERY_RESOURCES              = 0x0Au;
        inline constexpr UCHAR IRP_MN_QUERY_RESOURCE_REQUIREMENTS  = 0x0Bu;
        inline constexpr UCHAR IRP_MN_QUERY_DEVICE_TEXT            = 0x0Cu;
        inline constexpr UCHAR IRP_MN_FILTER_RESOURCE_REQUIREMENTS = 0x0Du;
        inline constexpr UCHAR IRP_MN_READ_CONFIG                  = 0x0Fu;
        inline constexpr UCHAR IRP_MN_WRITE_CONFIG                 = 0x10u;
        inline constexpr UCHAR IRP_MN_EJECT                        = 0x11u;
        inline constexpr UCHAR IRP_MN_SET_LOCK                     = 0x12u;
        inline constexpr UCHAR IRP_MN_QUERY_ID                     = 0x13u;
        inline constexpr UCHAR IRP_MN_QUERY_PNP_DEVICE_STATE       = 0x14u;
        inline constexpr UCHAR IRP_MN_QUERY_BUS_INFORMATION        = 0x15u;
        inline constexpr UCHAR IRP_MN_DEVICE_USAGE_NOTIFICATION    = 0x16u;
        inline constexpr UCHAR IRP_MN_SURPRISE_REMOVAL             = 0x17u;
        inline constexpr UCHAR IRP_MN_QUERY_LEGACY_BUS_INFORMATION = 0x18u;

        // power
        inline constexpr UCHAR IRP_MN_WAIT_WAKE      = 0x00u;
        inline constexpr UCHAR IRP_MN_POWER_SEQUENCE = 0x01u;
        inline constexpr UCHAR IRP_MN_SET_POWER      = 0x02u;
        inline constexpr UCHAR IRP_MN_QUERY_POWER    = 0x03u;

        // system control (WMI)
        inline constexpr UCHAR IRP_MN_QUERY_ALL_DATA         = 0x00u;
        inline constexpr UCHAR IRP_MN_QUERY_SINGLE_INSTANCE  = 0x01u;
        inline constexpr UCHAR IRP_MN_CHANGE_SINGLE_INSTANCE = 0x02u;
        inline constexpr UCHAR IRP_MN_CHANGE_SINGLE_ITEM     = 0x03u;
        inline constexpr UCHAR IRP_MN_ENABLE_EVENTS          = 0x04u;
        inline constexpr UCHAR IRP_MN_DISABLE_EVENTS         = 0x05u;
        inline constexpr UCHAR IRP_MN_ENABLE_COLLECTION      = 0x06u;
        inline constexpr UCHAR IRP_MN_DISABLE_COLLECTION     = 0x07u;
        inline constexpr UCHAR IRP_MN_REGINFO                = 0x08u;
        inline constexpr UCHAR IRP_MN_EXECUTE_METHOD         = 0x09u;

        // WSL reparse tags live in kernel-only ntifs.h
        inline constexpr ULONG IO_REPARSE_TAG_LX_SYMLINK = 0xA000001Du;
        inline constexpr ULONG IO_REPARSE_TAG_LX_FIFO    = 0x80000024u;
        inline constexpr ULONG IO_REPARSE_TAG_LX_CHR     = 0x80000025u;
        inline constexpr ULONG IO_REPARSE_TAG_LX_BLK     = 0x80000026u;

        inline constexpr ULONG FILE_SUPERSEDE    = 0x00000000u;
        inline constexpr ULONG FILE_OPEN         = 0x00000001u;
        inline constexpr ULONG FILE_CREATE       = 0x00000002u;
        inline constexpr ULONG FILE_OPEN_IF      = 0x00000003u;
        inline constexpr ULONG FILE_OVERWRITE    = 0x00000004u;
        inline constexpr ULONG FILE_OVERWRITE_IF = 0x00000005u;

        inline constexpr ULONGLONG FILE_SUPERSEDED     = 0u;
        inline constexpr ULONGLONG FILE_OPENED         = 1u;
        inline constexpr ULONGLONG FILE_CREATED        = 2u;
        inline constexpr ULONGLONG FILE_OVERWRITTEN    = 3u;
        inline constexpr ULONGLONG FILE_EXISTS         = 4u;
        inline constexpr ULONGLONG FILE_DOES_NOT_EXIST = 5u;

        inline constexpr ULONG FILE_DIRECTORY_FILE            = 0x00000001u;
        inline constexpr ULONG FILE_WRITE_THROUGH             = 0x00000002u;
        inline constexpr ULONG FILE_SEQUENTIAL_ONLY           = 0x00000004u;
        inline constexpr ULONG FILE_NO_INTERMEDIATE_BUFFERING = 0x00000008u;
        inline constexpr ULONG FILE_SYNCHRONOUS_IO_ALERT      = 0x00000010u;
        inline constexpr ULONG FILE_SYNCHRONOUS_IO_NONALERT   = 0x00000020u;
        inline constexpr ULONG FILE_NON_DIRECTORY_FILE        = 0x00000040u;
        inline constexpr ULONG FILE_CREATE_TREE_CONNECTION    = 0x00000080u;
        inline constexpr ULONG FILE_COMPLETE_IF_OPLOCKED      = 0x00000100u;
        inline constexpr ULONG FILE_NO_EA_KNOWLEDGE           = 0x00000200u;
        inline constexpr ULONG FILE_OPEN_REMOTE_INSTANCE      = 0x00000400u;
        inline constexpr ULONG FILE_RANDOM_ACCESS             = 0x00000800u;
        inline constexpr ULONG FILE_DELETE_ON_CLOSE           = 0x00001000u;
        inline constexpr ULONG FILE_OPEN_BY_FILE_ID           = 0x00002000u;
        inline constexpr ULONG FILE_OPEN_FOR_BACKUP_INTENT    = 0x00004000u;
        inline constexpr ULONG FILE_NO_COMPRESSION            = 0x00008000u;
        inline constexpr ULONG FILE_OPEN_REQUIRING_OPLOCK     = 0x00010000u;
        inline constexpr ULONG FILE_DISALLOW_EXCLUSIVE        = 0x00020000u;
        inline constexpr ULONG FILE_SESSION_AWARE             = 0x00040000u;
        inline constexpr ULONG FILE_RESERVE_OPFILTER          = 0x00100000u;
        inline constexpr ULONG FILE_OPEN_REPARSE_POINT        = 0x00200000u;
        inline constexpr ULONG FILE_OPEN_NO_RECALL            = 0x00400000u;
        inline constexpr ULONG FILE_OPEN_FOR_FREE_SPACE_QUERY = 0x00800000u;

        // read/write ByteOffset sentinels
        inline constexpr LONGLONG FILE_WRITE_TO_END_OF_FILE      = -1;
        inline constexpr LONGLONG FILE_USE_FILE_POINTER_POSITION = -2;

        // FILE_INFORMATION_CLASS codes
        inline constexpr ULONG FileDirectoryInformation                       = 1u;
        inline constexpr ULONG FileFullDirectoryInformation                   = 2u;
        inline constexpr ULONG FileBothDirectoryInformation                   = 3u;
        inline constexpr ULONG FileBasicInformation                           = 4u;
        inline constexpr ULONG FileStandardInformation                        = 5u;
        inline constexpr ULONG FileInternalInformation                        = 6u;
        inline constexpr ULONG FileEaInformation                              = 7u;
        inline constexpr ULONG FileAccessInformation                          = 8u;
        inline constexpr ULONG FileNameInformation                            = 9u;
        inline constexpr ULONG FileRenameInformation                          = 10u;
        inline constexpr ULONG FileLinkInformation                            = 11u;
        inline constexpr ULONG FileNamesInformation                           = 12u;
        inline constexpr ULONG FileDispositionInformation                     = 13u;
        inline constexpr ULONG FilePositionInformation                        = 14u;
        inline constexpr ULONG FileFullEaInformation                          = 15u;
        inline constexpr ULONG FileModeInformation                            = 16u;
        inline constexpr ULONG FileAlignmentInformation                       = 17u;
        inline constexpr ULONG FileAllInformation                             = 18u;
        inline constexpr ULONG FileAllocationInformation                      = 19u;
        inline constexpr ULONG FileEndOfFileInformation                       = 20u;
        inline constexpr ULONG FileAlternateNameInformation                   = 21u;
        inline constexpr ULONG FileStreamInformation                          = 22u;
        inline constexpr ULONG FilePipeInformation                            = 23u;
        inline constexpr ULONG FilePipeLocalInformation                       = 24u;
        inline constexpr ULONG FilePipeRemoteInformation                      = 25u;
        inline constexpr ULONG FileMailslotQueryInformation                   = 26u;
        inline constexpr ULONG FileMailslotSetInformation                     = 27u;
        inline constexpr ULONG FileCompressionInformation                     = 28u;
        inline constexpr ULONG FileObjectIdInformation                        = 29u;
        inline constexpr ULONG FileCompletionInformation                      = 30u;
        inline constexpr ULONG FileMoveClusterInformation                     = 31u;
        inline constexpr ULONG FileQuotaInformation                           = 32u;
        inline constexpr ULONG FileReparsePointInformation                    = 33u;
        inline constexpr ULONG FileNetworkOpenInformation                     = 34u;
        inline constexpr ULONG FileAttributeTagInformation                    = 35u;
        inline constexpr ULONG FileTrackingInformation                        = 36u;
        inline constexpr ULONG FileIdBothDirectoryInformation                 = 37u;
        inline constexpr ULONG FileIdFullDirectoryInformation                 = 38u;
        inline constexpr ULONG FileValidDataLengthInformation                 = 39u;
        inline constexpr ULONG FileShortNameInformation                       = 40u;
        inline constexpr ULONG FileIoCompletionNotificationInformation        = 41u;
        inline constexpr ULONG FileIoStatusBlockRangeInformation              = 42u;
        inline constexpr ULONG FileIoPriorityHintInformation                  = 43u;
        inline constexpr ULONG FileSfioReserveInformation                     = 44u;
        inline constexpr ULONG FileSfioVolumeInformation                      = 45u;
        inline constexpr ULONG FileHardLinkInformation                        = 46u;
        inline constexpr ULONG FileProcessIdsUsingFileInformation             = 47u;
        inline constexpr ULONG FileNormalizedNameInformation                  = 48u;
        inline constexpr ULONG FileNetworkPhysicalNameInformation             = 49u;
        inline constexpr ULONG FileIdGlobalTxDirectoryInformation             = 50u;
        inline constexpr ULONG FileIsRemoteDeviceInformation                  = 51u;
        inline constexpr ULONG FileUnusedInformation                          = 52u;
        inline constexpr ULONG FileNumaNodeInformation                        = 53u;
        inline constexpr ULONG FileStandardLinkInformation                    = 54u;
        inline constexpr ULONG FileRemoteProtocolInformation                  = 55u;
        inline constexpr ULONG FileRenameInformationBypassAccessCheck         = 56u;
        inline constexpr ULONG FileLinkInformationBypassAccessCheck           = 57u;
        inline constexpr ULONG FileVolumeNameInformation                      = 58u;
        inline constexpr ULONG FileIdInformation                              = 59u;
        inline constexpr ULONG FileIdExtdDirectoryInformation                 = 60u;
        inline constexpr ULONG FileReplaceCompletionInformation               = 61u;
        inline constexpr ULONG FileHardLinkFullIdInformation                  = 62u;
        inline constexpr ULONG FileIdExtdBothDirectoryInformation             = 63u;
        inline constexpr ULONG FileDispositionInformationEx                   = 64u;
        inline constexpr ULONG FileRenameInformationEx                        = 65u;
        inline constexpr ULONG FileRenameInformationExBypassAccessCheck       = 66u;
        inline constexpr ULONG FileDesiredStorageClassInformation             = 67u;
        inline constexpr ULONG FileStatInformation                            = 68u;
        inline constexpr ULONG FileMemoryPartitionInformation                 = 69u;
        inline constexpr ULONG FileStatLxInformation                          = 70u;
        inline constexpr ULONG FileCaseSensitiveInformation                   = 71u;
        inline constexpr ULONG FileLinkInformationEx                          = 72u;
        inline constexpr ULONG FileLinkInformationExBypassAccessCheck         = 73u;
        inline constexpr ULONG FileStorageReserveIdInformation                = 74u;
        inline constexpr ULONG FileCaseSensitiveInformationForceAccessCheck   = 75u;
        inline constexpr ULONG FileKnownFolderInformation                     = 76u;
        inline constexpr ULONG FileStatBasicInformation                       = 77u;
        inline constexpr ULONG FileId64ExtdDirectoryInformation               = 78u;
        inline constexpr ULONG FileId64ExtdBothDirectoryInformation           = 79u;
        inline constexpr ULONG FileIdAllExtdDirectoryInformation              = 80u;
        inline constexpr ULONG FileIdAllExtdBothDirectoryInformation          = 81u;
        inline constexpr ULONG FileStreamReservationInformation               = 82u;
        inline constexpr ULONG FileMupProviderInfo                            = 83u;

        // FILE_INFORMATION_CLASS payload layouts

        struct FILE_BASIC_INFORMATION {
            LONGLONG CreationTime;
            LONGLONG LastAccessTime;
            LONGLONG LastWriteTime;
            LONGLONG ChangeTime;
            ULONG FileAttributes;
        };

        static_assert(sizeof(FILE_BASIC_INFORMATION) == 40u, "kernel::FILE_BASIC_INFORMATION x64 layout drift");
        static_assert(offsetof(FILE_BASIC_INFORMATION, FileAttributes) == 32u, "kernel::FILE_BASIC_INFORMATION x64 layout drift");

        struct FILE_STANDARD_INFORMATION {
            LONGLONG AllocationSize;
            LONGLONG EndOfFile;
            ULONG NumberOfLinks;
            BOOLEAN DeletePending;
            BOOLEAN Directory;
        };

        static_assert(sizeof(FILE_STANDARD_INFORMATION) == 24u, "kernel::FILE_STANDARD_INFORMATION x64 layout drift");
        static_assert(offsetof(FILE_STANDARD_INFORMATION, DeletePending) == 20u, "kernel::FILE_STANDARD_INFORMATION x64 layout drift");

        struct FILE_INTERNAL_INFORMATION {
            LONGLONG IndexNumber;
        };

        static_assert(sizeof(FILE_INTERNAL_INFORMATION) == 8u, "kernel::FILE_INTERNAL_INFORMATION x64 layout drift");

        struct FILE_EA_INFORMATION {
            ULONG EaSize;
        };

        static_assert(sizeof(FILE_EA_INFORMATION) == 4u, "kernel::FILE_EA_INFORMATION x64 layout drift");

        struct FILE_ACCESS_INFORMATION {
            ACCESS_MASK AccessFlags;
        };

        static_assert(sizeof(FILE_ACCESS_INFORMATION) == 4u, "kernel::FILE_ACCESS_INFORMATION x64 layout drift");

        struct FILE_NAME_INFORMATION {
            ULONG FileNameLength;
            WCHAR FileName[1u];
        };

        static_assert(sizeof(FILE_NAME_INFORMATION) == 8u, "kernel::FILE_NAME_INFORMATION x64 layout drift");
        static_assert(offsetof(FILE_NAME_INFORMATION, FileName) == 4u, "kernel::FILE_NAME_INFORMATION x64 layout drift");

        struct FILE_POSITION_INFORMATION {
            LONGLONG CurrentByteOffset;
        };

        static_assert(sizeof(FILE_POSITION_INFORMATION) == 8u, "kernel::FILE_POSITION_INFORMATION x64 layout drift");

        struct FILE_MODE_INFORMATION {
            ULONG Mode;
        };

        static_assert(sizeof(FILE_MODE_INFORMATION) == 4u, "kernel::FILE_MODE_INFORMATION x64 layout drift");

        struct FILE_ALIGNMENT_INFORMATION {
            ULONG AlignmentRequirement;
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
            ULONG NextEntryOffset;
            ULONG StreamNameLength;
            LONGLONG StreamSize;
            LONGLONG StreamAllocationSize;
            WCHAR StreamName[1u];
        };

        static_assert(sizeof(FILE_STREAM_INFORMATION) == 32u, "kernel::FILE_STREAM_INFORMATION x64 layout drift");
        static_assert(offsetof(FILE_STREAM_INFORMATION, StreamName) == 24u, "kernel::FILE_STREAM_INFORMATION x64 layout drift");

        struct FILE_COMPRESSION_INFORMATION {
            LONGLONG CompressedFileSize;
            USHORT CompressionFormat;
            UCHAR CompressionUnitShift;
            UCHAR ChunkShift;
            UCHAR ClusterShift;
            UCHAR Reserved[3u];
        };

        static_assert(sizeof(FILE_COMPRESSION_INFORMATION) == 16u, "kernel::FILE_COMPRESSION_INFORMATION x64 layout drift");
        static_assert(offsetof(FILE_COMPRESSION_INFORMATION, CompressionFormat) == 8u, "kernel::FILE_COMPRESSION_INFORMATION x64 layout drift");

        struct FILE_NETWORK_OPEN_INFORMATION {
            LONGLONG CreationTime;
            LONGLONG LastAccessTime;
            LONGLONG LastWriteTime;
            LONGLONG ChangeTime;
            LONGLONG AllocationSize;
            LONGLONG EndOfFile;
            ULONG FileAttributes;
        };

        static_assert(sizeof(FILE_NETWORK_OPEN_INFORMATION) == 56u, "kernel::FILE_NETWORK_OPEN_INFORMATION x64 layout drift");
        static_assert(offsetof(FILE_NETWORK_OPEN_INFORMATION, FileAttributes) == 48u, "kernel::FILE_NETWORK_OPEN_INFORMATION x64 layout drift");

        struct FILE_ATTRIBUTE_TAG_INFORMATION {
            ULONG FileAttributes;
            ULONG ReparseTag;
        };

        static_assert(sizeof(FILE_ATTRIBUTE_TAG_INFORMATION) == 8u, "kernel::FILE_ATTRIBUTE_TAG_INFORMATION x64 layout drift");

        struct FILE_LINK_ENTRY_INFORMATION {
            ULONG NextEntryOffset;
            LONGLONG ParentFileId;
            ULONG FileNameLength;    // character count
            WCHAR FileName[1u];
        };

        static_assert(sizeof(FILE_LINK_ENTRY_INFORMATION) == 24u, "kernel::FILE_LINK_ENTRY_INFORMATION x64 layout drift");
        static_assert(offsetof(FILE_LINK_ENTRY_INFORMATION, ParentFileId) == 8u, "kernel::FILE_LINK_ENTRY_INFORMATION x64 layout drift");
        static_assert(offsetof(FILE_LINK_ENTRY_INFORMATION, FileName) == 20u, "kernel::FILE_LINK_ENTRY_INFORMATION x64 layout drift");

        struct FILE_LINKS_INFORMATION {
            ULONG BytesNeeded;
            ULONG EntriesReturned;
            FILE_LINK_ENTRY_INFORMATION Entry;
        };

        static_assert(sizeof(FILE_LINKS_INFORMATION) == 32u, "kernel::FILE_LINKS_INFORMATION x64 layout drift");
        static_assert(offsetof(FILE_LINKS_INFORMATION, Entry) == 8u, "kernel::FILE_LINKS_INFORMATION x64 layout drift");

        struct FILE_REMOTE_PROTOCOL_INFORMATION {
            USHORT StructureVersion;
            USHORT StructureSize;
            ULONG Protocol;
            USHORT ProtocolMajorVersion;
            USHORT ProtocolMinorVersion;
            USHORT ProtocolRevision;
            USHORT Reserved;
            ULONG Flags;
            ULONG GenericReserved[8u];
            ULONG ProtocolSpecific[16u];    // Smb2 capability union in the kernel
        };

        static_assert(sizeof(FILE_REMOTE_PROTOCOL_INFORMATION) == 116u, "kernel::FILE_REMOTE_PROTOCOL_INFORMATION x64 layout drift");
        static_assert(offsetof(FILE_REMOTE_PROTOCOL_INFORMATION, Flags) == 16u, "kernel::FILE_REMOTE_PROTOCOL_INFORMATION x64 layout drift");
        static_assert(offsetof(FILE_REMOTE_PROTOCOL_INFORMATION, GenericReserved) == 20u, "kernel::FILE_REMOTE_PROTOCOL_INFORMATION x64 layout drift");

        struct FILE_ID_INFORMATION {
            ULONGLONG VolumeSerialNumber;
            UCHAR FileId[16u];
        };

        static_assert(sizeof(FILE_ID_INFORMATION) == 24u, "kernel::FILE_ID_INFORMATION x64 layout drift");
        static_assert(offsetof(FILE_ID_INFORMATION, FileId) == 8u, "kernel::FILE_ID_INFORMATION x64 layout drift");

        struct FILE_STAT_INFORMATION {
            LONGLONG FileId;
            LONGLONG CreationTime;
            LONGLONG LastAccessTime;
            LONGLONG LastWriteTime;
            LONGLONG ChangeTime;
            LONGLONG AllocationSize;
            LONGLONG EndOfFile;
            ULONG FileAttributes;
            ULONG ReparseTag;
            ULONG NumberOfLinks;
            ACCESS_MASK EffectiveAccess;
        };

        static_assert(sizeof(FILE_STAT_INFORMATION) == 72u, "kernel::FILE_STAT_INFORMATION x64 layout drift");
        static_assert(offsetof(FILE_STAT_INFORMATION, FileAttributes) == 56u, "kernel::FILE_STAT_INFORMATION x64 layout drift");

        struct FILE_STAT_LX_INFORMATION {
            LONGLONG FileId;
            LONGLONG CreationTime;
            LONGLONG LastAccessTime;
            LONGLONG LastWriteTime;
            LONGLONG ChangeTime;
            LONGLONG AllocationSize;
            LONGLONG EndOfFile;
            ULONG FileAttributes;
            ULONG ReparseTag;
            ULONG NumberOfLinks;
            ACCESS_MASK EffectiveAccess;
            ULONG LxFlags;
            ULONG LxUid;
            ULONG LxGid;
            ULONG LxMode;
            ULONG LxDeviceIdMajor;
            ULONG LxDeviceIdMinor;
        };

        static_assert(sizeof(FILE_STAT_LX_INFORMATION) == 96u, "kernel::FILE_STAT_LX_INFORMATION x64 layout drift");
        static_assert(offsetof(FILE_STAT_LX_INFORMATION, LxFlags) == sizeof(FILE_STAT_INFORMATION), "kernel::FILE_STAT_LX_INFORMATION does not extend FILE_STAT_INFORMATION");

        struct FILE_CASE_SENSITIVE_INFORMATION {
            ULONG Flags;
        };

        static_assert(sizeof(FILE_CASE_SENSITIVE_INFORMATION) == 4u, "kernel::FILE_CASE_SENSITIVE_INFORMATION x64 layout drift");

        struct FILE_STAT_BASIC_INFORMATION {
            LONGLONG FileId;
            LONGLONG CreationTime;
            LONGLONG LastAccessTime;
            LONGLONG LastWriteTime;
            LONGLONG ChangeTime;
            LONGLONG AllocationSize;
            LONGLONG EndOfFile;
            ULONG FileAttributes;
            ULONG ReparseTag;
            ULONG NumberOfLinks;
            ULONG DeviceType;
            ULONG DeviceCharacteristics;
            ULONG Reserved;
            LONGLONG VolumeSerialNumber;
            UCHAR FileId128[16u];
        };

        static_assert(sizeof(FILE_STAT_BASIC_INFORMATION) == 104u, "kernel::FILE_STAT_BASIC_INFORMATION x64 layout drift");
        static_assert(offsetof(FILE_STAT_BASIC_INFORMATION, DeviceType) == 68u, "kernel::FILE_STAT_BASIC_INFORMATION x64 layout drift");
        static_assert(offsetof(FILE_STAT_BASIC_INFORMATION, FileId128) == 88u, "kernel::FILE_STAT_BASIC_INFORMATION x64 layout drift");

        // FILE_LINK_INFORMATION shares this layout
        struct FILE_RENAME_INFORMATION {
            BOOLEAN ReplaceIfExists;
            ULONGLONG RootDirectory;    // HANDLE in the kernel, fixed 64-bit for the x64 layout
            ULONG FileNameLength;
            WCHAR FileName[1u];
        };

        static_assert(sizeof(FILE_RENAME_INFORMATION) == 24u, "kernel::FILE_RENAME_INFORMATION x64 layout drift");
        static_assert(offsetof(FILE_RENAME_INFORMATION, RootDirectory) == 8u, "kernel::FILE_RENAME_INFORMATION x64 layout drift");
        static_assert(offsetof(FILE_RENAME_INFORMATION, FileNameLength) == 16u, "kernel::FILE_RENAME_INFORMATION x64 layout drift");
        static_assert(offsetof(FILE_RENAME_INFORMATION, FileName) == 20u, "kernel::FILE_RENAME_INFORMATION x64 layout drift");

        struct FILE_DISPOSITION_INFORMATION {
            BOOLEAN DeletePending;      // MS-FSCC spelling, the ntifs.h member name DeleteFile is a winbase.h macro
        };

        static_assert(sizeof(FILE_DISPOSITION_INFORMATION) == 1u, "kernel::FILE_DISPOSITION_INFORMATION x64 layout drift");

        struct FILE_ALLOCATION_INFORMATION {
            LONGLONG AllocationSize;
        };

        static_assert(sizeof(FILE_ALLOCATION_INFORMATION) == 8u, "kernel::FILE_ALLOCATION_INFORMATION x64 layout drift");

        struct FILE_END_OF_FILE_INFORMATION {
            LONGLONG EndOfFile;
        };

        static_assert(sizeof(FILE_END_OF_FILE_INFORMATION) == 8u, "kernel::FILE_END_OF_FILE_INFORMATION x64 layout drift");

        struct FILE_VALID_DATA_LENGTH_INFORMATION {
            LONGLONG ValidDataLength;
        };

        static_assert(sizeof(FILE_VALID_DATA_LENGTH_INFORMATION) == 8u, "kernel::FILE_VALID_DATA_LENGTH_INFORMATION x64 layout drift");

        struct FILE_DISPOSITION_INFORMATION_EX {
            ULONG Flags;
        };

        static_assert(sizeof(FILE_DISPOSITION_INFORMATION_EX) == 4u, "kernel::FILE_DISPOSITION_INFORMATION_EX x64 layout drift");

        // FILE_LINK_INFORMATION_EX shares this layout
        struct FILE_RENAME_INFORMATION_EX {
            ULONG Flags;
            ULONGLONG RootDirectory;    // HANDLE in the kernel, fixed 64-bit for the x64 layout
            ULONG FileNameLength;
            WCHAR FileName[1u];
        };

        static_assert(sizeof(FILE_RENAME_INFORMATION_EX) == 24u, "kernel::FILE_RENAME_INFORMATION_EX x64 layout drift");
        static_assert(offsetof(FILE_RENAME_INFORMATION_EX, RootDirectory) == 8u, "kernel::FILE_RENAME_INFORMATION_EX x64 layout drift");
        static_assert(offsetof(FILE_RENAME_INFORMATION_EX, FileNameLength) == 16u, "kernel::FILE_RENAME_INFORMATION_EX x64 layout drift");
        static_assert(offsetof(FILE_RENAME_INFORMATION_EX, FileName) == 20u, "kernel::FILE_RENAME_INFORMATION_EX x64 layout drift");

        inline constexpr ULONG REMOTE_PROTOCOL_FLAG_LOOPBACK          = 0x00000001u;
        inline constexpr ULONG REMOTE_PROTOCOL_FLAG_OFFLINE           = 0x00000002u;
        inline constexpr ULONG REMOTE_PROTOCOL_FLAG_PERSISTENT_HANDLE = 0x00000004u;
        inline constexpr ULONG REMOTE_PROTOCOL_FLAG_PRIVACY           = 0x00000008u;
        inline constexpr ULONG REMOTE_PROTOCOL_FLAG_INTEGRITY         = 0x00000010u;
        inline constexpr ULONG REMOTE_PROTOCOL_FLAG_MUTUAL_AUTH       = 0x00000020u;

        // FILE_DISPOSITION_INFORMATION_EX flags
        inline constexpr ULONG FILE_DISPOSITION_DO_NOT_DELETE             = 0x00000000u;
        inline constexpr ULONG FILE_DISPOSITION_DELETE                    = 0x00000001u;
        inline constexpr ULONG FILE_DISPOSITION_POSIX_SEMANTICS           = 0x00000002u;
        inline constexpr ULONG FILE_DISPOSITION_FORCE_IMAGE_SECTION_CHECK = 0x00000004u;
        inline constexpr ULONG FILE_DISPOSITION_ON_CLOSE                  = 0x00000008u;
        inline constexpr ULONG FILE_DISPOSITION_IGNORE_READONLY_ATTRIBUTE = 0x00000010u;

        // FILE_RENAME_INFORMATION_EX flags, FILE_LINK_INFORMATION_EX shares the values
        inline constexpr ULONG FILE_RENAME_REPLACE_IF_EXISTS                    = 0x00000001u;
        inline constexpr ULONG FILE_RENAME_POSIX_SEMANTICS                      = 0x00000002u;
        inline constexpr ULONG FILE_RENAME_SUPPRESS_PIN_STATE_INHERITANCE       = 0x00000004u;
        inline constexpr ULONG FILE_RENAME_SUPPRESS_STORAGE_RESERVE_INHERITANCE = 0x00000008u;
        inline constexpr ULONG FILE_RENAME_NO_INCREASE_AVAILABLE_SPACE          = 0x00000010u;
        inline constexpr ULONG FILE_RENAME_NO_DECREASE_AVAILABLE_SPACE          = 0x00000020u;
        inline constexpr ULONG FILE_RENAME_PRESERVE_AVAILABLE_SPACE             = 0x00000030u;
        inline constexpr ULONG FILE_RENAME_IGNORE_READONLY_ATTRIBUTE            = 0x00000040u;
        inline constexpr ULONG FILE_RENAME_FORCE_RESIZE_TARGET_SR               = 0x00000080u;
        inline constexpr ULONG FILE_RENAME_FORCE_RESIZE_SOURCE_SR               = 0x00000100u;
        inline constexpr ULONG FILE_RENAME_FORCE_RESIZE_SR                      = 0x00000180u;

    }

}
