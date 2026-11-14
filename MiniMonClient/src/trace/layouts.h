#pragma once

#include <cstddef>
#include <cstdint>

namespace mimo {

    namespace trace {

        namespace layouts {

            // FILE_INFORMATION_CLASS payload layouts

            struct FILE_BASIC_INFORMATION {
                int64_t CreationTime;
                int64_t LastAccessTime;
                int64_t LastWriteTime;
                int64_t ChangeTime;
                uint32_t FileAttributes;
            };

            static_assert(sizeof(FILE_BASIC_INFORMATION) == 40u, "layouts::FILE_BASIC_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_BASIC_INFORMATION, FileAttributes) == 32u, "layouts::FILE_BASIC_INFORMATION x64 layout drift");

            struct FILE_STANDARD_INFORMATION {
                int64_t AllocationSize;
                int64_t EndOfFile;
                uint32_t NumberOfLinks;
                uint8_t DeletePending;
                uint8_t Directory;
            };

            static_assert(sizeof(FILE_STANDARD_INFORMATION) == 24u, "layouts::FILE_STANDARD_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_STANDARD_INFORMATION, DeletePending) == 20u, "layouts::FILE_STANDARD_INFORMATION x64 layout drift");

            struct FILE_INTERNAL_INFORMATION {
                int64_t IndexNumber;
            };

            static_assert(sizeof(FILE_INTERNAL_INFORMATION) == 8u, "layouts::FILE_INTERNAL_INFORMATION x64 layout drift");

            struct FILE_EA_INFORMATION {
                uint32_t EaSize;
            };

            static_assert(sizeof(FILE_EA_INFORMATION) == 4u, "layouts::FILE_EA_INFORMATION x64 layout drift");

            struct FILE_ACCESS_INFORMATION {
                uint32_t AccessFlags;
            };

            static_assert(sizeof(FILE_ACCESS_INFORMATION) == 4u, "layouts::FILE_ACCESS_INFORMATION x64 layout drift");

            struct FILE_NAME_INFORMATION {
                uint32_t FileNameLength;
                wchar_t FileName[1u];
            };

            static_assert(sizeof(FILE_NAME_INFORMATION) == 8u, "layouts::FILE_NAME_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_NAME_INFORMATION, FileName) == 4u, "layouts::FILE_NAME_INFORMATION x64 layout drift");

            struct FILE_POSITION_INFORMATION {
                int64_t CurrentByteOffset;
            };

            static_assert(sizeof(FILE_POSITION_INFORMATION) == 8u, "layouts::FILE_POSITION_INFORMATION x64 layout drift");

            struct FILE_MODE_INFORMATION {
                uint32_t Mode;
            };

            static_assert(sizeof(FILE_MODE_INFORMATION) == 4u, "layouts::FILE_MODE_INFORMATION x64 layout drift");

            struct FILE_ALIGNMENT_INFORMATION {
                uint32_t AlignmentRequirement;
            };

            static_assert(sizeof(FILE_ALIGNMENT_INFORMATION) == 4u, "layouts::FILE_ALIGNMENT_INFORMATION x64 layout drift");

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

            static_assert(sizeof(FILE_ALL_INFORMATION) == 104u, "layouts::FILE_ALL_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_ALL_INFORMATION, StandardInformation) == 40u, "layouts::FILE_ALL_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_ALL_INFORMATION, PositionInformation) == 80u, "layouts::FILE_ALL_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_ALL_INFORMATION, NameInformation) == 96u, "layouts::FILE_ALL_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_ALL_INFORMATION, NameInformation.FileName) == 100u, "layouts::FILE_ALL_INFORMATION x64 layout drift");

            struct FILE_STREAM_INFORMATION {
                uint32_t NextEntryOffset;
                uint32_t StreamNameLength;
                int64_t StreamSize;
                int64_t StreamAllocationSize;
                wchar_t StreamName[1u];
            };

            static_assert(sizeof(FILE_STREAM_INFORMATION) == 32u, "layouts::FILE_STREAM_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_STREAM_INFORMATION, StreamName) == 24u, "layouts::FILE_STREAM_INFORMATION x64 layout drift");

            struct FILE_COMPRESSION_INFORMATION {
                int64_t CompressedFileSize;
                uint16_t CompressionFormat;
                uint8_t CompressionUnitShift;
                uint8_t ChunkShift;
                uint8_t ClusterShift;
                uint8_t Reserved[3u];
            };

            static_assert(sizeof(FILE_COMPRESSION_INFORMATION) == 16u, "layouts::FILE_COMPRESSION_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_COMPRESSION_INFORMATION, CompressionFormat) == 8u, "layouts::FILE_COMPRESSION_INFORMATION x64 layout drift");

            struct FILE_NETWORK_OPEN_INFORMATION {
                int64_t CreationTime;
                int64_t LastAccessTime;
                int64_t LastWriteTime;
                int64_t ChangeTime;
                int64_t AllocationSize;
                int64_t EndOfFile;
                uint32_t FileAttributes;
            };

            static_assert(sizeof(FILE_NETWORK_OPEN_INFORMATION) == 56u, "layouts::FILE_NETWORK_OPEN_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_NETWORK_OPEN_INFORMATION, FileAttributes) == 48u, "layouts::FILE_NETWORK_OPEN_INFORMATION x64 layout drift");

            struct FILE_ATTRIBUTE_TAG_INFORMATION {
                uint32_t FileAttributes;
                uint32_t ReparseTag;
            };

            static_assert(sizeof(FILE_ATTRIBUTE_TAG_INFORMATION) == 8u, "layouts::FILE_ATTRIBUTE_TAG_INFORMATION x64 layout drift");

            struct FILE_SFIO_RESERVE_INFORMATION {
                uint32_t RequestsPerPeriod;
                uint32_t Period;
                uint8_t RetryFailures;
                uint8_t Discardable;
                uint32_t RequestSize;
                uint32_t NumOutstandingRequests;
            };

            static_assert(sizeof(FILE_SFIO_RESERVE_INFORMATION) == 20u, "layouts::FILE_SFIO_RESERVE_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_SFIO_RESERVE_INFORMATION, Discardable) == 9u, "layouts::FILE_SFIO_RESERVE_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_SFIO_RESERVE_INFORMATION, RequestSize) == 12u, "layouts::FILE_SFIO_RESERVE_INFORMATION x64 layout drift");

            struct FILE_SFIO_VOLUME_INFORMATION {
                uint32_t MaximumRequestsPerPeriod;
                uint32_t MinimumPeriod;
                uint32_t MinimumTransferSize;
            };

            static_assert(sizeof(FILE_SFIO_VOLUME_INFORMATION) == 12u, "layouts::FILE_SFIO_VOLUME_INFORMATION x64 layout drift");

            struct FILE_LINK_ENTRY_INFORMATION {
                uint32_t NextEntryOffset;
                int64_t ParentFileId;
                uint32_t FileNameLength;    // character count
                wchar_t FileName[1u];
            };

            static_assert(sizeof(FILE_LINK_ENTRY_INFORMATION) == 24u, "layouts::FILE_LINK_ENTRY_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_LINK_ENTRY_INFORMATION, ParentFileId) == 8u, "layouts::FILE_LINK_ENTRY_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_LINK_ENTRY_INFORMATION, FileName) == 20u, "layouts::FILE_LINK_ENTRY_INFORMATION x64 layout drift");

            struct FILE_LINKS_INFORMATION {
                uint32_t BytesNeeded;
                uint32_t EntriesReturned;
                FILE_LINK_ENTRY_INFORMATION Entry;
            };

            static_assert(sizeof(FILE_LINKS_INFORMATION) == 32u, "layouts::FILE_LINKS_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_LINKS_INFORMATION, Entry) == 8u, "layouts::FILE_LINKS_INFORMATION x64 layout drift");

            struct FILE_PROCESS_IDS_USING_FILE_INFORMATION {
                uint32_t NumberOfProcessIdsInList;
                uint64_t ProcessIdList[1u];    // ULONG_PTR in the kernel
            };

            static_assert(sizeof(FILE_PROCESS_IDS_USING_FILE_INFORMATION) == 16u, "layouts::FILE_PROCESS_IDS_USING_FILE_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_PROCESS_IDS_USING_FILE_INFORMATION, ProcessIdList) == 8u, "layouts::FILE_PROCESS_IDS_USING_FILE_INFORMATION x64 layout drift");

            struct FILE_STANDARD_LINK_INFORMATION {
                uint32_t NumberOfAccessibleLinks;
                uint32_t TotalNumberOfLinks;
                uint8_t DeletePending;
                uint8_t Directory;
            };

            static_assert(sizeof(FILE_STANDARD_LINK_INFORMATION) == 12u, "layouts::FILE_STANDARD_LINK_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_STANDARD_LINK_INFORMATION, DeletePending) == 8u, "layouts::FILE_STANDARD_LINK_INFORMATION x64 layout drift");

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

            static_assert(sizeof(FILE_REMOTE_PROTOCOL_INFORMATION) == 116u, "layouts::FILE_REMOTE_PROTOCOL_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_REMOTE_PROTOCOL_INFORMATION, Flags) == 16u, "layouts::FILE_REMOTE_PROTOCOL_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_REMOTE_PROTOCOL_INFORMATION, GenericReserved) == 20u, "layouts::FILE_REMOTE_PROTOCOL_INFORMATION x64 layout drift");

            struct FILE_VOLUME_NAME_INFORMATION {
                uint32_t DeviceNameLength;
                wchar_t DeviceName[1u];
            };

            static_assert(sizeof(FILE_VOLUME_NAME_INFORMATION) == 8u, "layouts::FILE_VOLUME_NAME_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_VOLUME_NAME_INFORMATION, DeviceName) == 4u, "layouts::FILE_VOLUME_NAME_INFORMATION x64 layout drift");

            struct FILE_ID_INFORMATION {
                uint64_t VolumeSerialNumber;
                uint8_t FileId[16u];
            };

            static_assert(sizeof(FILE_ID_INFORMATION) == 24u, "layouts::FILE_ID_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_ID_INFORMATION, FileId) == 8u, "layouts::FILE_ID_INFORMATION x64 layout drift");

            struct FILE_LINK_ENTRY_FULL_ID_INFORMATION {
                uint32_t NextEntryOffset;
                uint8_t ParentFileId[16u];
                uint32_t FileNameLength;    // character count
                wchar_t FileName[1u];
            };

            static_assert(sizeof(FILE_LINK_ENTRY_FULL_ID_INFORMATION) == 28u, "layouts::FILE_LINK_ENTRY_FULL_ID_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_LINK_ENTRY_FULL_ID_INFORMATION, ParentFileId) == 4u, "layouts::FILE_LINK_ENTRY_FULL_ID_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_LINK_ENTRY_FULL_ID_INFORMATION, FileName) == 24u, "layouts::FILE_LINK_ENTRY_FULL_ID_INFORMATION x64 layout drift");

            struct FILE_LINKS_FULL_ID_INFORMATION {
                uint32_t BytesNeeded;
                uint32_t EntriesReturned;
                FILE_LINK_ENTRY_FULL_ID_INFORMATION Entry;
            };

            static_assert(sizeof(FILE_LINKS_FULL_ID_INFORMATION) == 36u, "layouts::FILE_LINKS_FULL_ID_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_LINKS_FULL_ID_INFORMATION, Entry) == 8u, "layouts::FILE_LINKS_FULL_ID_INFORMATION x64 layout drift");

            struct FILE_DESIRED_STORAGE_CLASS_INFORMATION {
                uint32_t Class;    // FILE_STORAGE_TIER_CLASS in the kernel
                uint32_t Flags;
            };

            static_assert(sizeof(FILE_DESIRED_STORAGE_CLASS_INFORMATION) == 8u, "layouts::FILE_DESIRED_STORAGE_CLASS_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_DESIRED_STORAGE_CLASS_INFORMATION, Flags) == 4u, "layouts::FILE_DESIRED_STORAGE_CLASS_INFORMATION x64 layout drift");

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

            static_assert(sizeof(FILE_STAT_INFORMATION) == 72u, "layouts::FILE_STAT_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_STAT_INFORMATION, FileAttributes) == 56u, "layouts::FILE_STAT_INFORMATION x64 layout drift");

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

            static_assert(sizeof(FILE_STAT_LX_INFORMATION) == 96u, "layouts::FILE_STAT_LX_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_STAT_LX_INFORMATION, LxFlags) == sizeof(FILE_STAT_INFORMATION), "layouts::FILE_STAT_LX_INFORMATION x64 layout drift");

            struct FILE_CASE_SENSITIVE_INFORMATION {
                uint32_t Flags;
            };

            static_assert(sizeof(FILE_CASE_SENSITIVE_INFORMATION) == 4u, "layouts::FILE_CASE_SENSITIVE_INFORMATION x64 layout drift");

            struct FILE_STORAGE_RESERVE_ID_INFORMATION {
                uint32_t StorageReserveId;
            };

            static_assert(sizeof(FILE_STORAGE_RESERVE_ID_INFORMATION) == 4u, "layouts::FILE_STORAGE_RESERVE_ID_INFORMATION x64 layout drift");

            struct FILE_KNOWN_FOLDER_INFORMATION {
                uint32_t Type;    // FILE_KNOWN_FOLDER_TYPE in the kernel
            };

            static_assert(sizeof(FILE_KNOWN_FOLDER_INFORMATION) == 4u, "layouts::FILE_KNOWN_FOLDER_INFORMATION x64 layout drift");

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

            static_assert(sizeof(FILE_STAT_BASIC_INFORMATION) == 104u, "layouts::FILE_STAT_BASIC_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_STAT_BASIC_INFORMATION, DeviceType) == 68u, "layouts::FILE_STAT_BASIC_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_STAT_BASIC_INFORMATION, FileId128) == 88u, "layouts::FILE_STAT_BASIC_INFORMATION x64 layout drift");

            struct FILE_STREAM_RESERVATION_INFORMATION {
                int64_t TrackedReservation;
                int64_t EnforcedReservation;
            };

            static_assert(sizeof(FILE_STREAM_RESERVATION_INFORMATION) == 16u, "layouts::FILE_STREAM_RESERVATION_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_STREAM_RESERVATION_INFORMATION, EnforcedReservation) == 8u, "layouts::FILE_STREAM_RESERVATION_INFORMATION x64 layout drift");

            // FILE_LINK_INFORMATION shares this layout
            struct FILE_RENAME_INFORMATION {
                uint8_t ReplaceIfExists;
                uint64_t RootDirectory;     // HANDLE in the kernel
                uint32_t FileNameLength;
                wchar_t FileName[1u];
            };

            static_assert(sizeof(FILE_RENAME_INFORMATION) == 24u, "layouts::FILE_RENAME_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_RENAME_INFORMATION, RootDirectory) == 8u, "layouts::FILE_RENAME_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_RENAME_INFORMATION, FileNameLength) == 16u, "layouts::FILE_RENAME_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_RENAME_INFORMATION, FileName) == 20u, "layouts::FILE_RENAME_INFORMATION x64 layout drift");

            struct FILE_DISPOSITION_INFORMATION {
                uint8_t DeletePending;      // MS-FSCC spelling, the ntifs.h member name DeleteFile is a winbase.h macro
            };

            static_assert(sizeof(FILE_DISPOSITION_INFORMATION) == 1u, "layouts::FILE_DISPOSITION_INFORMATION x64 layout drift");

            struct FILE_ALLOCATION_INFORMATION {
                int64_t AllocationSize;
            };

            static_assert(sizeof(FILE_ALLOCATION_INFORMATION) == 8u, "layouts::FILE_ALLOCATION_INFORMATION x64 layout drift");

            struct FILE_END_OF_FILE_INFORMATION {
                int64_t EndOfFile;
            };

            static_assert(sizeof(FILE_END_OF_FILE_INFORMATION) == 8u, "layouts::FILE_END_OF_FILE_INFORMATION x64 layout drift");

            struct FILE_TRACKING_INFORMATION {
                uint64_t DestinationFile;    // HANDLE in the kernel
                uint32_t ObjectInformationLength;
                char ObjectInformation[1u];
            };

            static_assert(sizeof(FILE_TRACKING_INFORMATION) == 16u, "layouts::FILE_TRACKING_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_TRACKING_INFORMATION, ObjectInformationLength) == 8u, "layouts::FILE_TRACKING_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_TRACKING_INFORMATION, ObjectInformation) == 12u, "layouts::FILE_TRACKING_INFORMATION x64 layout drift");

            struct FILE_VALID_DATA_LENGTH_INFORMATION {
                int64_t ValidDataLength;
            };

            static_assert(sizeof(FILE_VALID_DATA_LENGTH_INFORMATION) == 8u, "layouts::FILE_VALID_DATA_LENGTH_INFORMATION x64 layout drift");

            struct FILE_DISPOSITION_INFORMATION_EX {
                uint32_t Flags;
            };

            static_assert(sizeof(FILE_DISPOSITION_INFORMATION_EX) == 4u, "layouts::FILE_DISPOSITION_INFORMATION_EX x64 layout drift");

            // FILE_RENAME_INFORMATION under the Ex classes, FILE_LINK_INFORMATION shares this layout
            struct FILE_RENAME_INFORMATION_EX {
                uint32_t Flags;
                uint64_t RootDirectory;     // HANDLE in the kernel
                uint32_t FileNameLength;
                wchar_t FileName[1u];
            };

            static_assert(sizeof(FILE_RENAME_INFORMATION_EX) == 24u, "layouts::FILE_RENAME_INFORMATION_EX x64 layout drift");
            static_assert(offsetof(FILE_RENAME_INFORMATION_EX, RootDirectory) == 8u, "layouts::FILE_RENAME_INFORMATION_EX x64 layout drift");
            static_assert(offsetof(FILE_RENAME_INFORMATION_EX, FileNameLength) == 16u, "layouts::FILE_RENAME_INFORMATION_EX x64 layout drift");
            static_assert(offsetof(FILE_RENAME_INFORMATION_EX, FileName) == 20u, "layouts::FILE_RENAME_INFORMATION_EX x64 layout drift");

            // extended attribute entry layouts

            struct FILE_FULL_EA_INFORMATION {
                uint32_t NextEntryOffset;
                uint8_t Flags;
                uint8_t EaNameLength;
                uint16_t EaValueLength;
                char EaName[1u];
            };

            static_assert(sizeof(FILE_FULL_EA_INFORMATION) == 12u, "layouts::FILE_FULL_EA_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_FULL_EA_INFORMATION, EaValueLength) == 6u, "layouts::FILE_FULL_EA_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_FULL_EA_INFORMATION, EaName) == 8u, "layouts::FILE_FULL_EA_INFORMATION x64 layout drift");

            struct FILE_GET_EA_INFORMATION {
                uint32_t NextEntryOffset;
                uint8_t EaNameLength;
                char EaName[1u];
            };

            static_assert(sizeof(FILE_GET_EA_INFORMATION) == 8u, "layouts::FILE_GET_EA_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_GET_EA_INFORMATION, EaName) == 5u, "layouts::FILE_GET_EA_INFORMATION x64 layout drift");

            // FS_INFORMATION_CLASS payload layouts

            struct FILE_FS_VOLUME_INFORMATION {
                int64_t VolumeCreationTime;
                uint32_t VolumeSerialNumber;
                uint32_t VolumeLabelLength;
                uint8_t SupportsObjects;
                wchar_t VolumeLabel[1u];
            };

            static_assert(sizeof(FILE_FS_VOLUME_INFORMATION) == 24u, "layouts::FILE_FS_VOLUME_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_FS_VOLUME_INFORMATION, VolumeLabel) == 18u, "layouts::FILE_FS_VOLUME_INFORMATION x64 layout drift");

            struct FILE_FS_LABEL_INFORMATION {
                uint32_t VolumeLabelLength;
                wchar_t VolumeLabel[1u];
            };

            static_assert(sizeof(FILE_FS_LABEL_INFORMATION) == 8u, "layouts::FILE_FS_LABEL_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_FS_LABEL_INFORMATION, VolumeLabel) == 4u, "layouts::FILE_FS_LABEL_INFORMATION x64 layout drift");

            struct FILE_FS_SIZE_INFORMATION {
                int64_t TotalAllocationUnits;
                int64_t AvailableAllocationUnits;
                uint32_t SectorsPerAllocationUnit;
                uint32_t BytesPerSector;
            };

            static_assert(sizeof(FILE_FS_SIZE_INFORMATION) == 24u, "layouts::FILE_FS_SIZE_INFORMATION x64 layout drift");

            struct FILE_FS_DEVICE_INFORMATION {
                uint32_t DeviceType;
                uint32_t Characteristics;
            };

            static_assert(sizeof(FILE_FS_DEVICE_INFORMATION) == 8u, "layouts::FILE_FS_DEVICE_INFORMATION x64 layout drift");

            struct FILE_FS_ATTRIBUTE_INFORMATION {
                uint32_t FileSystemAttributes;
                int32_t MaximumComponentNameLength;
                uint32_t FileSystemNameLength;
                wchar_t FileSystemName[1u];
            };

            static_assert(sizeof(FILE_FS_ATTRIBUTE_INFORMATION) == 16u, "layouts::FILE_FS_ATTRIBUTE_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_FS_ATTRIBUTE_INFORMATION, FileSystemName) == 12u, "layouts::FILE_FS_ATTRIBUTE_INFORMATION x64 layout drift");

            struct FILE_FS_CONTROL_INFORMATION {
                int64_t FreeSpaceStartFiltering;
                int64_t FreeSpaceThreshold;
                int64_t FreeSpaceStopFiltering;
                int64_t DefaultQuotaThreshold;
                int64_t DefaultQuotaLimit;
                uint32_t FileSystemControlFlags;
            };

            static_assert(sizeof(FILE_FS_CONTROL_INFORMATION) == 48u, "layouts::FILE_FS_CONTROL_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_FS_CONTROL_INFORMATION, FileSystemControlFlags) == 40u, "layouts::FILE_FS_CONTROL_INFORMATION x64 layout drift");

            struct FILE_FS_FULL_SIZE_INFORMATION {
                int64_t TotalAllocationUnits;
                int64_t CallerAvailableAllocationUnits;
                int64_t ActualAvailableAllocationUnits;
                uint32_t SectorsPerAllocationUnit;
                uint32_t BytesPerSector;
            };

            static_assert(sizeof(FILE_FS_FULL_SIZE_INFORMATION) == 32u, "layouts::FILE_FS_FULL_SIZE_INFORMATION x64 layout drift");

            struct FILE_FS_OBJECTID_INFORMATION {
                uint8_t ObjectId[16u];
                uint8_t ExtendedInfo[48u];
            };

            static_assert(sizeof(FILE_FS_OBJECTID_INFORMATION) == 64u, "layouts::FILE_FS_OBJECTID_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_FS_OBJECTID_INFORMATION, ExtendedInfo) == 16u, "layouts::FILE_FS_OBJECTID_INFORMATION x64 layout drift");

            struct FILE_FS_VOLUME_FLAGS_INFORMATION {
                uint32_t Flags;
            };

            static_assert(sizeof(FILE_FS_VOLUME_FLAGS_INFORMATION) == 4u, "layouts::FILE_FS_VOLUME_FLAGS_INFORMATION x64 layout drift");

            struct FILE_FS_SECTOR_SIZE_INFORMATION {
                uint32_t LogicalBytesPerSector;
                uint32_t PhysicalBytesPerSectorForAtomicity;
                uint32_t PhysicalBytesPerSectorForPerformance;
                uint32_t FileSystemEffectivePhysicalBytesPerSectorForAtomicity;
                uint32_t Flags;
                uint32_t ByteOffsetForSectorAlignment;
                uint32_t ByteOffsetForPartitionAlignment;
            };

            static_assert(sizeof(FILE_FS_SECTOR_SIZE_INFORMATION) == 28u, "layouts::FILE_FS_SECTOR_SIZE_INFORMATION x64 layout drift");

            struct FILE_FS_DATA_COPY_INFORMATION {
                uint32_t NumberOfCopies;
            };

            static_assert(sizeof(FILE_FS_DATA_COPY_INFORMATION) == 4u, "layouts::FILE_FS_DATA_COPY_INFORMATION x64 layout drift");

            struct FILE_FS_METADATA_SIZE_INFORMATION {
                int64_t TotalMetadataAllocationUnits;
                uint32_t SectorsPerAllocationUnit;
                uint32_t BytesPerSector;
            };

            static_assert(sizeof(FILE_FS_METADATA_SIZE_INFORMATION) == 16u, "layouts::FILE_FS_METADATA_SIZE_INFORMATION x64 layout drift");

            struct FILE_FS_FULL_SIZE_INFORMATION_EX {
                uint64_t ActualTotalAllocationUnits;
                uint64_t ActualAvailableAllocationUnits;
                uint64_t ActualPoolUnavailableAllocationUnits;
                uint64_t CallerTotalAllocationUnits;
                uint64_t CallerAvailableAllocationUnits;
                uint64_t CallerPoolUnavailableAllocationUnits;
                uint64_t UsedAllocationUnits;
                uint64_t TotalReservedAllocationUnits;
                uint64_t VolumeStorageReserveAllocationUnits;
                uint64_t AvailableCommittedAllocationUnits;
                uint64_t PoolAvailableAllocationUnits;
                uint32_t SectorsPerAllocationUnit;
                uint32_t BytesPerSector;
            };

            static_assert(sizeof(FILE_FS_FULL_SIZE_INFORMATION_EX) == 96u, "layouts::FILE_FS_FULL_SIZE_INFORMATION_EX x64 layout drift");

            struct FILE_FS_GUID_INFORMATION {
                uint8_t FsGuid[16u];
            };

            static_assert(sizeof(FILE_FS_GUID_INFORMATION) == 16u, "layouts::FILE_FS_GUID_INFORMATION x64 layout drift");

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

            static_assert(sizeof(FILE_DIRECTORY_INFORMATION) == 72u, "layouts::FILE_DIRECTORY_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_DIRECTORY_INFORMATION, FileName) == 64u, "layouts::FILE_DIRECTORY_INFORMATION x64 layout drift");

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

            static_assert(sizeof(FILE_FULL_DIR_INFORMATION) == 72u, "layouts::FILE_FULL_DIR_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_FULL_DIR_INFORMATION, FileName) == 68u, "layouts::FILE_FULL_DIR_INFORMATION x64 layout drift");

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

            static_assert(sizeof(FILE_BOTH_DIR_INFORMATION) == 96u, "layouts::FILE_BOTH_DIR_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_BOTH_DIR_INFORMATION, FileName) == 94u, "layouts::FILE_BOTH_DIR_INFORMATION x64 layout drift");

            struct FILE_NAMES_INFORMATION {
                uint32_t NextEntryOffset;
                uint32_t FileIndex;
                uint32_t FileNameLength;
                wchar_t FileName[1u];
            };

            static_assert(sizeof(FILE_NAMES_INFORMATION) == 16u, "layouts::FILE_NAMES_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_NAMES_INFORMATION, FileName) == 12u, "layouts::FILE_NAMES_INFORMATION x64 layout drift");

            struct FILE_OBJECTID_INFORMATION {
                int64_t FileReference;
                uint8_t ObjectId[16u];
                uint8_t BirthVolumeId[16u];
                uint8_t BirthObjectId[16u];
                uint8_t DomainId[16u];
            };

            static_assert(sizeof(FILE_OBJECTID_INFORMATION) == 72u, "layouts::FILE_OBJECTID_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_OBJECTID_INFORMATION, ObjectId) == 8u, "layouts::FILE_OBJECTID_INFORMATION x64 layout drift");

            struct FILE_REPARSE_POINT_INFORMATION {
                int64_t FileReference;
                uint32_t Tag;
            };

            static_assert(sizeof(FILE_REPARSE_POINT_INFORMATION) == 16u, "layouts::FILE_REPARSE_POINT_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_REPARSE_POINT_INFORMATION, Tag) == 8u, "layouts::FILE_REPARSE_POINT_INFORMATION x64 layout drift");

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

            static_assert(sizeof(FILE_ID_BOTH_DIR_INFORMATION) == 112u, "layouts::FILE_ID_BOTH_DIR_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_ID_BOTH_DIR_INFORMATION, FileId) == 96u, "layouts::FILE_ID_BOTH_DIR_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_ID_BOTH_DIR_INFORMATION, FileName) == 104u, "layouts::FILE_ID_BOTH_DIR_INFORMATION x64 layout drift");

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

            static_assert(sizeof(FILE_ID_FULL_DIR_INFORMATION) == 88u, "layouts::FILE_ID_FULL_DIR_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_ID_FULL_DIR_INFORMATION, FileId) == 72u, "layouts::FILE_ID_FULL_DIR_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_ID_FULL_DIR_INFORMATION, FileName) == 80u, "layouts::FILE_ID_FULL_DIR_INFORMATION x64 layout drift");

            struct FILE_ID_GLOBAL_TX_DIR_INFORMATION {
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
                int64_t FileId;
                uint8_t LockingTransactionId[16u];
                uint32_t TxInfoFlags;
                wchar_t FileName[1u];
            };

            static_assert(sizeof(FILE_ID_GLOBAL_TX_DIR_INFORMATION) == 96u, "layouts::FILE_ID_GLOBAL_TX_DIR_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_ID_GLOBAL_TX_DIR_INFORMATION, FileId) == 64u, "layouts::FILE_ID_GLOBAL_TX_DIR_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_ID_GLOBAL_TX_DIR_INFORMATION, FileName) == 92u, "layouts::FILE_ID_GLOBAL_TX_DIR_INFORMATION x64 layout drift");

            struct FILE_ID_EXTD_DIR_INFORMATION {
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
                uint32_t ReparsePointTag;
                uint8_t FileId[16u];
                wchar_t FileName[1u];
            };

            static_assert(sizeof(FILE_ID_EXTD_DIR_INFORMATION) == 96u, "layouts::FILE_ID_EXTD_DIR_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_ID_EXTD_DIR_INFORMATION, FileId) == 72u, "layouts::FILE_ID_EXTD_DIR_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_ID_EXTD_DIR_INFORMATION, FileName) == 88u, "layouts::FILE_ID_EXTD_DIR_INFORMATION x64 layout drift");

            struct FILE_ID_EXTD_BOTH_DIR_INFORMATION {
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
                uint32_t ReparsePointTag;
                uint8_t FileId[16u];
                int8_t ShortNameLength;
                wchar_t ShortName[12u];
                wchar_t FileName[1u];
            };

            static_assert(sizeof(FILE_ID_EXTD_BOTH_DIR_INFORMATION) == 120u, "layouts::FILE_ID_EXTD_BOTH_DIR_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_ID_EXTD_BOTH_DIR_INFORMATION, FileId) == 72u, "layouts::FILE_ID_EXTD_BOTH_DIR_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_ID_EXTD_BOTH_DIR_INFORMATION, FileName) == 114u, "layouts::FILE_ID_EXTD_BOTH_DIR_INFORMATION x64 layout drift");

            struct FILE_ID_64_EXTD_DIR_INFORMATION {
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
                uint32_t ReparsePointTag;
                int64_t FileId;
                wchar_t FileName[1u];
            };

            static_assert(sizeof(FILE_ID_64_EXTD_DIR_INFORMATION) == 88u, "layouts::FILE_ID_64_EXTD_DIR_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_ID_64_EXTD_DIR_INFORMATION, FileId) == 72u, "layouts::FILE_ID_64_EXTD_DIR_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_ID_64_EXTD_DIR_INFORMATION, FileName) == 80u, "layouts::FILE_ID_64_EXTD_DIR_INFORMATION x64 layout drift");

            struct FILE_ID_64_EXTD_BOTH_DIR_INFORMATION {
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
                uint32_t ReparsePointTag;
                int64_t FileId;
                int8_t ShortNameLength;
                wchar_t ShortName[12u];
                wchar_t FileName[1u];
            };

            static_assert(sizeof(FILE_ID_64_EXTD_BOTH_DIR_INFORMATION) == 112u, "layouts::FILE_ID_64_EXTD_BOTH_DIR_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_ID_64_EXTD_BOTH_DIR_INFORMATION, FileId) == 72u, "layouts::FILE_ID_64_EXTD_BOTH_DIR_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_ID_64_EXTD_BOTH_DIR_INFORMATION, FileName) == 106u, "layouts::FILE_ID_64_EXTD_BOTH_DIR_INFORMATION x64 layout drift");

            struct FILE_ID_ALL_EXTD_DIR_INFORMATION {
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
                uint32_t ReparsePointTag;
                int64_t FileId;
                uint8_t FileId128[16u];
                wchar_t FileName[1u];
            };

            static_assert(sizeof(FILE_ID_ALL_EXTD_DIR_INFORMATION) == 104u, "layouts::FILE_ID_ALL_EXTD_DIR_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_ID_ALL_EXTD_DIR_INFORMATION, FileId) == 72u, "layouts::FILE_ID_ALL_EXTD_DIR_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_ID_ALL_EXTD_DIR_INFORMATION, FileName) == 96u, "layouts::FILE_ID_ALL_EXTD_DIR_INFORMATION x64 layout drift");

            struct FILE_ID_ALL_EXTD_BOTH_DIR_INFORMATION {
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
                uint32_t ReparsePointTag;
                int64_t FileId;
                uint8_t FileId128[16u];
                int8_t ShortNameLength;
                wchar_t ShortName[12u];
                wchar_t FileName[1u];
            };

            static_assert(sizeof(FILE_ID_ALL_EXTD_BOTH_DIR_INFORMATION) == 128u, "layouts::FILE_ID_ALL_EXTD_BOTH_DIR_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_ID_ALL_EXTD_BOTH_DIR_INFORMATION, FileId) == 72u, "layouts::FILE_ID_ALL_EXTD_BOTH_DIR_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_ID_ALL_EXTD_BOTH_DIR_INFORMATION, FileName) == 122u, "layouts::FILE_ID_ALL_EXTD_BOTH_DIR_INFORMATION x64 layout drift");

            // directory notify entry layouts

            struct FILE_NOTIFY_INFORMATION {
                uint32_t NextEntryOffset;
                uint32_t Action;
                uint32_t FileNameLength;
                wchar_t FileName[1u];
            };

            static_assert(sizeof(FILE_NOTIFY_INFORMATION) == 16u, "layouts::FILE_NOTIFY_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_NOTIFY_INFORMATION, FileName) == 12u, "layouts::FILE_NOTIFY_INFORMATION x64 layout drift");

            struct FILE_NOTIFY_EXTENDED_INFORMATION {
                uint32_t NextEntryOffset;
                uint32_t Action;
                int64_t CreationTime;
                int64_t LastModificationTime;
                int64_t LastChangeTime;
                int64_t LastAccessTime;
                int64_t AllocatedLength;
                int64_t FileSize;
                uint32_t FileAttributes;
                uint32_t ReparsePointTag;    // EaSize without a reparse point, the kernel's nameless union
                int64_t FileId;
                int64_t ParentFileId;
                uint32_t FileNameLength;
                wchar_t FileName[1u];
            };

            static_assert(sizeof(FILE_NOTIFY_EXTENDED_INFORMATION) == 88u, "layouts::FILE_NOTIFY_EXTENDED_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_NOTIFY_EXTENDED_INFORMATION, FileId) == 64u, "layouts::FILE_NOTIFY_EXTENDED_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_NOTIFY_EXTENDED_INFORMATION, FileName) == 84u, "layouts::FILE_NOTIFY_EXTENDED_INFORMATION x64 layout drift");

            struct FILE_NOTIFY_FULL_INFORMATION {
                uint32_t NextEntryOffset;
                uint32_t Action;
                int64_t CreationTime;
                int64_t LastModificationTime;
                int64_t LastChangeTime;
                int64_t LastAccessTime;
                int64_t AllocatedLength;
                int64_t FileSize;
                uint32_t FileAttributes;
                uint32_t ReparsePointTag;    // EaSize without a reparse point, the kernel's nameless union
                int64_t FileId;
                int64_t ParentFileId;
                uint16_t FileNameLength;
                uint8_t FileNameFlags;
                uint8_t Reserved;
                wchar_t FileName[1u];
            };

            static_assert(sizeof(FILE_NOTIFY_FULL_INFORMATION) == 88u, "layouts::FILE_NOTIFY_FULL_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_NOTIFY_FULL_INFORMATION, FileNameLength) == 80u, "layouts::FILE_NOTIFY_FULL_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_NOTIFY_FULL_INFORMATION, FileName) == 84u, "layouts::FILE_NOTIFY_FULL_INFORMATION x64 layout drift");

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

            static_assert(sizeof(FILESYSTEM_STATISTICS) == 56u, "layouts::FILESYSTEM_STATISTICS x64 layout drift");
            static_assert(offsetof(FILESYSTEM_STATISTICS, UserFileReads) == 8u, "layouts::FILESYSTEM_STATISTICS x64 layout drift");

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

            static_assert(sizeof(NTFS_VOLUME_DATA_BUFFER) == 96u, "layouts::NTFS_VOLUME_DATA_BUFFER x64 layout drift");
            static_assert(offsetof(NTFS_VOLUME_DATA_BUFFER, MftValidDataLength) == 56u, "layouts::NTFS_VOLUME_DATA_BUFFER x64 layout drift");

            struct MOVE_FILE_DATA {
                uint64_t FileHandle;    // HANDLE in the kernel
                int64_t StartingVcn;
                int64_t StartingLcn;
                uint32_t ClusterCount;
            };

            static_assert(sizeof(MOVE_FILE_DATA) == 32u, "layouts::MOVE_FILE_DATA x64 layout drift");
            static_assert(offsetof(MOVE_FILE_DATA, ClusterCount) == 24u, "layouts::MOVE_FILE_DATA x64 layout drift");

            struct FILE_OBJECTID_BUFFER {
                uint8_t ObjectId[16u];
                uint8_t BirthVolumeId[16u];
                uint8_t BirthObjectId[16u];
                uint8_t DomainId[16u];
            };

            static_assert(sizeof(FILE_OBJECTID_BUFFER) == 64u, "layouts::FILE_OBJECTID_BUFFER x64 layout drift");
            static_assert(offsetof(FILE_OBJECTID_BUFFER, BirthVolumeId) == 16u, "layouts::FILE_OBJECTID_BUFFER x64 layout drift");

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

            static_assert(sizeof(REPARSE_DATA_BUFFER) == 24u, "layouts::REPARSE_DATA_BUFFER x64 layout drift");
            static_assert(offsetof(REPARSE_DATA_BUFFER, SymbolicLinkReparseBuffer) == 8u, "layouts::REPARSE_DATA_BUFFER x64 layout drift");
            static_assert(offsetof(REPARSE_DATA_BUFFER, SymbolicLinkReparseBuffer.PathBuffer) == 20u, "layouts::REPARSE_DATA_BUFFER x64 layout drift");
            static_assert(offsetof(REPARSE_DATA_BUFFER, MountPointReparseBuffer.PathBuffer) == 16u, "layouts::REPARSE_DATA_BUFFER x64 layout drift");

            struct READ_USN_JOURNAL_DATA_V0 {
                int64_t StartUsn;
                uint32_t ReasonMask;
                uint32_t ReturnOnlyOnClose;
                uint64_t Timeout;
                uint64_t BytesToWaitFor;
                uint64_t UsnJournalID;
            };

            static_assert(sizeof(READ_USN_JOURNAL_DATA_V0) == 40u, "layouts::READ_USN_JOURNAL_DATA_V0 x64 layout drift");
            static_assert(offsetof(READ_USN_JOURNAL_DATA_V0, UsnJournalID) == 32u, "layouts::READ_USN_JOURNAL_DATA_V0 x64 layout drift");

            struct READ_USN_JOURNAL_DATA_V1 {
                int64_t StartUsn;
                uint32_t ReasonMask;
                uint32_t ReturnOnlyOnClose;
                uint64_t Timeout;
                uint64_t BytesToWaitFor;
                uint64_t UsnJournalID;
                uint16_t MinMajorVersion;
                uint16_t MaxMajorVersion;
            };

            static_assert(sizeof(READ_USN_JOURNAL_DATA_V1) == 48u, "layouts::READ_USN_JOURNAL_DATA_V1 x64 layout drift");
            static_assert(offsetof(READ_USN_JOURNAL_DATA_V1, MinMajorVersion) == 40u, "layouts::READ_USN_JOURNAL_DATA_V1 x64 layout drift");

            struct USN_RECORD_COMMON_HEADER {
                uint32_t RecordLength;
                uint16_t MajorVersion;
                uint16_t MinorVersion;
            };

            static_assert(sizeof(USN_RECORD_COMMON_HEADER) == 8u, "layouts::USN_RECORD_COMMON_HEADER x64 layout drift");

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

            static_assert(sizeof(USN_RECORD_V2) == 64u, "layouts::USN_RECORD_V2 x64 layout drift");
            static_assert(offsetof(USN_RECORD_V2, FileName) == 60u, "layouts::USN_RECORD_V2 x64 layout drift");

            struct USN_RECORD_V3 {
                uint32_t RecordLength;
                uint16_t MajorVersion;
                uint16_t MinorVersion;
                uint8_t FileReferenceNumber[16u];
                uint8_t ParentFileReferenceNumber[16u];
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

            static_assert(sizeof(USN_RECORD_V3) == 80u, "layouts::USN_RECORD_V3 x64 layout drift");
            static_assert(offsetof(USN_RECORD_V3, Usn) == 40u, "layouts::USN_RECORD_V3 x64 layout drift");
            static_assert(offsetof(USN_RECORD_V3, FileName) == 76u, "layouts::USN_RECORD_V3 x64 layout drift");

            struct FILE_SET_SPARSE_BUFFER {
                uint8_t SetSparse;
            };

            static_assert(sizeof(FILE_SET_SPARSE_BUFFER) == 1u, "layouts::FILE_SET_SPARSE_BUFFER x64 layout drift");

            struct FILE_ZERO_DATA_INFORMATION {
                int64_t FileOffset;
                int64_t BeyondFinalZero;
            };

            static_assert(sizeof(FILE_ZERO_DATA_INFORMATION) == 16u, "layouts::FILE_ZERO_DATA_INFORMATION x64 layout drift");

            struct FILE_ALLOCATED_RANGE_BUFFER {
                int64_t FileOffset;
                int64_t Length;
            };

            static_assert(sizeof(FILE_ALLOCATED_RANGE_BUFFER) == 16u, "layouts::FILE_ALLOCATED_RANGE_BUFFER x64 layout drift");

            struct CREATE_USN_JOURNAL_DATA {
                uint64_t MaximumSize;
                uint64_t AllocationDelta;
            };

            static_assert(sizeof(CREATE_USN_JOURNAL_DATA) == 16u, "layouts::CREATE_USN_JOURNAL_DATA x64 layout drift");

            struct READ_FILE_USN_DATA {
                uint16_t MinMajorVersion;
                uint16_t MaxMajorVersion;
            };

            static_assert(sizeof(READ_FILE_USN_DATA) == 4u, "layouts::READ_FILE_USN_DATA x64 layout drift");

            struct USN_JOURNAL_DATA_V0 {
                uint64_t UsnJournalID;
                int64_t FirstUsn;
                int64_t NextUsn;
                int64_t LowestValidUsn;
                int64_t MaxUsn;
                uint64_t MaximumSize;
                uint64_t AllocationDelta;
            };

            static_assert(sizeof(USN_JOURNAL_DATA_V0) == 56u, "layouts::USN_JOURNAL_DATA_V0 x64 layout drift");
            static_assert(offsetof(USN_JOURNAL_DATA_V0, AllocationDelta) == 48u, "layouts::USN_JOURNAL_DATA_V0 x64 layout drift");

            struct USN_JOURNAL_DATA_V1 {
                uint64_t UsnJournalID;
                int64_t FirstUsn;
                int64_t NextUsn;
                int64_t LowestValidUsn;
                int64_t MaxUsn;
                uint64_t MaximumSize;
                uint64_t AllocationDelta;
                uint16_t MinSupportedMajorVersion;
                uint16_t MaxSupportedMajorVersion;
            };

            static_assert(sizeof(USN_JOURNAL_DATA_V1) == 64u, "layouts::USN_JOURNAL_DATA_V1 x64 layout drift");
            static_assert(offsetof(USN_JOURNAL_DATA_V1, MinSupportedMajorVersion) == 56u, "layouts::USN_JOURNAL_DATA_V1 x64 layout drift");

            struct USN_JOURNAL_DATA_V2 {
                uint64_t UsnJournalID;
                int64_t FirstUsn;
                int64_t NextUsn;
                int64_t LowestValidUsn;
                int64_t MaxUsn;
                uint64_t MaximumSize;
                uint64_t AllocationDelta;
                uint16_t MinSupportedMajorVersion;
                uint16_t MaxSupportedMajorVersion;
                uint32_t Flags;
                uint64_t RangeTrackChunkSize;
                int64_t RangeTrackFileSizeThreshold;
            };

            static_assert(sizeof(USN_JOURNAL_DATA_V2) == 80u, "layouts::USN_JOURNAL_DATA_V2 x64 layout drift");
            static_assert(offsetof(USN_JOURNAL_DATA_V2, Flags) == 60u, "layouts::USN_JOURNAL_DATA_V2 x64 layout drift");
            static_assert(offsetof(USN_JOURNAL_DATA_V2, RangeTrackChunkSize) == 64u, "layouts::USN_JOURNAL_DATA_V2 x64 layout drift");

            struct MARK_HANDLE_INFO {
                uint32_t UsnSourceInfo;     // CopyNumber under MARK_HANDLE_READ_COPY, the kernel's nameless union
                uint64_t VolumeHandle;      // HANDLE in the kernel
                uint32_t HandleInfo;
            };

            static_assert(sizeof(MARK_HANDLE_INFO) == 24u, "layouts::MARK_HANDLE_INFO x64 layout drift");
            static_assert(offsetof(MARK_HANDLE_INFO, VolumeHandle) == 8u, "layouts::MARK_HANDLE_INFO x64 layout drift");
            static_assert(offsetof(MARK_HANDLE_INFO, HandleInfo) == 16u, "layouts::MARK_HANDLE_INFO x64 layout drift");

            struct FILE_PREFETCH {
                uint32_t Type;
                uint32_t Count;
                uint64_t Prefetch[1u];
            };

            static_assert(sizeof(FILE_PREFETCH) == 16u, "layouts::FILE_PREFETCH x64 layout drift");
            static_assert(offsetof(FILE_PREFETCH, Prefetch) == 8u, "layouts::FILE_PREFETCH x64 layout drift");

            struct FILE_PREFETCH_EX {
                uint32_t Type;
                uint32_t Count;
                uint64_t Context;       // PVOID in the kernel
                uint64_t Prefetch[1u];
            };

            static_assert(sizeof(FILE_PREFETCH_EX) == 24u, "layouts::FILE_PREFETCH_EX x64 layout drift");
            static_assert(offsetof(FILE_PREFETCH_EX, Prefetch) == 16u, "layouts::FILE_PREFETCH_EX x64 layout drift");

            struct TXFS_QUERY_RM_INFORMATION {
                uint32_t BytesRequired;
                uint64_t TailLsn;
                uint64_t CurrentLsn;
                uint64_t ArchiveTailLsn;
                uint64_t LogContainerSize;
                int64_t HighestVirtualClock;
                uint32_t LogContainerCount;
                uint32_t LogContainerCountMax;
                uint32_t LogContainerCountMin;
                uint32_t LogGrowthIncrement;
                uint32_t LogAutoShrinkPercentage;
                uint32_t Flags;
                uint16_t LoggingMode;
                uint16_t Reserved;
                uint32_t RmState;
                uint64_t LogCapacity;
                uint64_t LogFree;
                uint64_t TopsSize;
                uint64_t TopsUsed;
                uint64_t TransactionCount;
                uint64_t OnePCCount;
                uint64_t TwoPCCount;
                uint64_t NumberLogFileFull;
                uint64_t OldestTransactionAge;
                uint8_t RMName[16u];
                uint32_t TmLogPathOffset;
            };

            static_assert(sizeof(TXFS_QUERY_RM_INFORMATION) == 176u, "layouts::TXFS_QUERY_RM_INFORMATION x64 layout drift");
            static_assert(offsetof(TXFS_QUERY_RM_INFORMATION, TailLsn) == 8u, "layouts::TXFS_QUERY_RM_INFORMATION x64 layout drift");
            static_assert(offsetof(TXFS_QUERY_RM_INFORMATION, LogContainerCount) == 48u, "layouts::TXFS_QUERY_RM_INFORMATION x64 layout drift");
            static_assert(offsetof(TXFS_QUERY_RM_INFORMATION, LoggingMode) == 72u, "layouts::TXFS_QUERY_RM_INFORMATION x64 layout drift");
            static_assert(offsetof(TXFS_QUERY_RM_INFORMATION, LogCapacity) == 80u, "layouts::TXFS_QUERY_RM_INFORMATION x64 layout drift");
            static_assert(offsetof(TXFS_QUERY_RM_INFORMATION, RMName) == 152u, "layouts::TXFS_QUERY_RM_INFORMATION x64 layout drift");
            static_assert(offsetof(TXFS_QUERY_RM_INFORMATION, TmLogPathOffset) == 168u, "layouts::TXFS_QUERY_RM_INFORMATION x64 layout drift");

            struct FILE_LEVEL_TRIM_RANGE {
                uint64_t Offset;
                uint64_t Length;
            };

            static_assert(sizeof(FILE_LEVEL_TRIM_RANGE) == 16u, "layouts::FILE_LEVEL_TRIM_RANGE x64 layout drift");

            struct FILE_LEVEL_TRIM {
                uint32_t Key;
                uint32_t NumRanges;
                FILE_LEVEL_TRIM_RANGE Ranges[1u];
            };

            static_assert(sizeof(FILE_LEVEL_TRIM) == 24u, "layouts::FILE_LEVEL_TRIM x64 layout drift");
            static_assert(offsetof(FILE_LEVEL_TRIM, Ranges) == 8u, "layouts::FILE_LEVEL_TRIM x64 layout drift");

            struct FILE_FS_PERSISTENT_VOLUME_INFORMATION {
                uint32_t VolumeFlags;
                uint32_t FlagMask;
                uint32_t Version;
                uint32_t Reserved;
            };

            static_assert(sizeof(FILE_FS_PERSISTENT_VOLUME_INFORMATION) == 16u, "layouts::FILE_FS_PERSISTENT_VOLUME_INFORMATION x64 layout drift");

            struct REQUEST_OPLOCK_INPUT_BUFFER {
                uint16_t StructureVersion;
                uint16_t StructureLength;
                uint32_t RequestedOplockLevel;
                uint32_t Flags;
            };

            static_assert(sizeof(REQUEST_OPLOCK_INPUT_BUFFER) == 12u, "layouts::REQUEST_OPLOCK_INPUT_BUFFER x64 layout drift");
            static_assert(offsetof(REQUEST_OPLOCK_INPUT_BUFFER, RequestedOplockLevel) == 4u, "layouts::REQUEST_OPLOCK_INPUT_BUFFER x64 layout drift");

            struct REQUEST_OPLOCK_OUTPUT_BUFFER {
                uint16_t StructureVersion;
                uint16_t StructureLength;
                uint32_t OriginalOplockLevel;
                uint32_t NewOplockLevel;
                uint32_t Flags;
                uint32_t AccessMode;    // ACCESS_MASK in the kernel
                uint16_t ShareMode;
            };

            static_assert(sizeof(REQUEST_OPLOCK_OUTPUT_BUFFER) == 24u, "layouts::REQUEST_OPLOCK_OUTPUT_BUFFER x64 layout drift");
            static_assert(offsetof(REQUEST_OPLOCK_OUTPUT_BUFFER, AccessMode) == 16u, "layouts::REQUEST_OPLOCK_OUTPUT_BUFFER x64 layout drift");

            struct FILE_REGION_INPUT {
                int64_t FileOffset;
                int64_t Length;
                uint32_t DesiredUsage;
            };

            static_assert(sizeof(FILE_REGION_INPUT) == 24u, "layouts::FILE_REGION_INPUT x64 layout drift");
            static_assert(offsetof(FILE_REGION_INPUT, DesiredUsage) == 16u, "layouts::FILE_REGION_INPUT x64 layout drift");

            struct FILE_REGION_INFO {
                int64_t FileOffset;
                int64_t Length;
                uint32_t Usage;
                uint32_t Reserved;
            };

            static_assert(sizeof(FILE_REGION_INFO) == 24u, "layouts::FILE_REGION_INFO x64 layout drift");

            struct FILE_REGION_OUTPUT {
                uint32_t Flags;
                uint32_t TotalRegionEntryCount;
                uint32_t RegionEntryCount;
                uint32_t Reserved;
                FILE_REGION_INFO Region[1u];
            };

            static_assert(sizeof(FILE_REGION_OUTPUT) == 40u, "layouts::FILE_REGION_OUTPUT x64 layout drift");
            static_assert(offsetof(FILE_REGION_OUTPUT, Region) == 16u, "layouts::FILE_REGION_OUTPUT x64 layout drift");

            struct DUPLICATE_EXTENTS_DATA {
                uint64_t FileHandle;    // HANDLE in the kernel
                int64_t SourceFileOffset;
                int64_t TargetFileOffset;
                int64_t ByteCount;
            };

            static_assert(sizeof(DUPLICATE_EXTENTS_DATA) == 32u, "layouts::DUPLICATE_EXTENTS_DATA x64 layout drift");
            static_assert(offsetof(DUPLICATE_EXTENTS_DATA, SourceFileOffset) == 8u, "layouts::DUPLICATE_EXTENTS_DATA x64 layout drift");

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

            static_assert(sizeof(FILESYSTEM_STATISTICS_EX) == 104u, "layouts::FILESYSTEM_STATISTICS_EX x64 layout drift");
            static_assert(offsetof(FILESYSTEM_STATISTICS_EX, UserFileReads) == 8u, "layouts::FILESYSTEM_STATISTICS_EX x64 layout drift");

            // IOCTL payload layouts

            struct DISK_GEOMETRY {
                int64_t Cylinders;
                uint32_t MediaType;
                uint32_t TracksPerCylinder;
                uint32_t SectorsPerTrack;
                uint32_t BytesPerSector;
            };

            static_assert(sizeof(DISK_GEOMETRY) == 24u, "layouts::DISK_GEOMETRY x64 layout drift");
            static_assert(offsetof(DISK_GEOMETRY, MediaType) == 8u, "layouts::DISK_GEOMETRY x64 layout drift");

            struct DISK_GEOMETRY_EX {
                DISK_GEOMETRY Geometry;
                int64_t DiskSize;
                uint8_t Data[1u];
            };

            static_assert(sizeof(DISK_GEOMETRY_EX) == 40u, "layouts::DISK_GEOMETRY_EX x64 layout drift");
            static_assert(offsetof(DISK_GEOMETRY_EX, DiskSize) == 24u, "layouts::DISK_GEOMETRY_EX x64 layout drift");
            static_assert(offsetof(DISK_GEOMETRY_EX, Data) == 32u, "layouts::DISK_GEOMETRY_EX x64 layout drift");

            struct GET_LENGTH_INFORMATION {
                int64_t Length;
            };

            static_assert(sizeof(GET_LENGTH_INFORMATION) == 8u, "layouts::GET_LENGTH_INFORMATION x64 layout drift");

            struct PARTITION_INFORMATION_MBR {
                uint8_t PartitionType;
                uint8_t BootIndicator;
                uint8_t RecognizedPartition;
                uint32_t HiddenSectors;
                uint8_t PartitionId[16u];
            };

            static_assert(sizeof(PARTITION_INFORMATION_MBR) == 24u, "layouts::PARTITION_INFORMATION_MBR x64 layout drift");
            static_assert(offsetof(PARTITION_INFORMATION_MBR, HiddenSectors) == 4u, "layouts::PARTITION_INFORMATION_MBR x64 layout drift");
            static_assert(offsetof(PARTITION_INFORMATION_MBR, PartitionId) == 8u, "layouts::PARTITION_INFORMATION_MBR x64 layout drift");

            struct PARTITION_INFORMATION_GPT {
                uint8_t PartitionType[16u];
                uint8_t PartitionId[16u];
                uint64_t Attributes;
                wchar_t Name[36u];
            };

            static_assert(sizeof(PARTITION_INFORMATION_GPT) == 112u, "layouts::PARTITION_INFORMATION_GPT x64 layout drift");
            static_assert(offsetof(PARTITION_INFORMATION_GPT, Attributes) == 32u, "layouts::PARTITION_INFORMATION_GPT x64 layout drift");
            static_assert(offsetof(PARTITION_INFORMATION_GPT, Name) == 40u, "layouts::PARTITION_INFORMATION_GPT x64 layout drift");

            struct PARTITION_INFORMATION_EX {
                uint32_t PartitionStyle;
                uint16_t PartitionOrdinal;
                int64_t StartingOffset;
                int64_t PartitionLength;
                uint32_t PartitionNumber;
                uint8_t RewritePartition;
                uint8_t IsServicePartition;
                union {
                    PARTITION_INFORMATION_MBR Mbr;
                    PARTITION_INFORMATION_GPT Gpt;
                };
            };

            static_assert(sizeof(PARTITION_INFORMATION_EX) == 144u, "layouts::PARTITION_INFORMATION_EX x64 layout drift");
            static_assert(offsetof(PARTITION_INFORMATION_EX, StartingOffset) == 8u, "layouts::PARTITION_INFORMATION_EX x64 layout drift");
            static_assert(offsetof(PARTITION_INFORMATION_EX, PartitionNumber) == 24u, "layouts::PARTITION_INFORMATION_EX x64 layout drift");
            static_assert(offsetof(PARTITION_INFORMATION_EX, IsServicePartition) == 29u, "layouts::PARTITION_INFORMATION_EX x64 layout drift");
            static_assert(offsetof(PARTITION_INFORMATION_EX, Mbr) == 32u, "layouts::PARTITION_INFORMATION_EX x64 layout drift");
            static_assert(offsetof(PARTITION_INFORMATION_EX, Gpt) == 32u, "layouts::PARTITION_INFORMATION_EX x64 layout drift");

            struct GET_DISK_ATTRIBUTES {
                uint32_t Version;
                uint32_t Reserved1;
                uint64_t Attributes;
            };

            static_assert(sizeof(GET_DISK_ATTRIBUTES) == 16u, "layouts::GET_DISK_ATTRIBUTES x64 layout drift");
            static_assert(offsetof(GET_DISK_ATTRIBUTES, Attributes) == 8u, "layouts::GET_DISK_ATTRIBUTES x64 layout drift");

            struct STORAGE_HOTPLUG_INFO {
                uint32_t Size;
                uint8_t MediaRemovable;
                uint8_t MediaHotplug;
                uint8_t DeviceHotplug;
                uint8_t WriteCacheEnableOverride;
            };

            static_assert(sizeof(STORAGE_HOTPLUG_INFO) == 8u, "layouts::STORAGE_HOTPLUG_INFO x64 layout drift");
            static_assert(offsetof(STORAGE_HOTPLUG_INFO, MediaRemovable) == 4u, "layouts::STORAGE_HOTPLUG_INFO x64 layout drift");

            struct STORAGE_DEVICE_NUMBER {
                uint32_t DeviceType;
                uint32_t DeviceNumber;
                uint32_t PartitionNumber;
            };

            static_assert(sizeof(STORAGE_DEVICE_NUMBER) == 12u, "layouts::STORAGE_DEVICE_NUMBER x64 layout drift");

            struct STORAGE_DEVICE_NUMBER_EX {
                uint32_t Version;
                uint32_t Size;
                uint32_t Flags;
                uint32_t DeviceType;
                uint32_t DeviceNumber;
                uint8_t DeviceGuid[16u];
                uint32_t PartitionNumber;
            };

            static_assert(sizeof(STORAGE_DEVICE_NUMBER_EX) == 40u, "layouts::STORAGE_DEVICE_NUMBER_EX x64 layout drift");
            static_assert(offsetof(STORAGE_DEVICE_NUMBER_EX, DeviceGuid) == 20u, "layouts::STORAGE_DEVICE_NUMBER_EX x64 layout drift");
            static_assert(offsetof(STORAGE_DEVICE_NUMBER_EX, PartitionNumber) == 36u, "layouts::STORAGE_DEVICE_NUMBER_EX x64 layout drift");

            struct STORAGE_PROPERTY_QUERY {
                uint32_t PropertyId;
                uint32_t QueryType;
                uint8_t AdditionalParameters[1u];
            };

            static_assert(sizeof(STORAGE_PROPERTY_QUERY) == 12u, "layouts::STORAGE_PROPERTY_QUERY x64 layout drift");
            static_assert(offsetof(STORAGE_PROPERTY_QUERY, AdditionalParameters) == 8u, "layouts::STORAGE_PROPERTY_QUERY x64 layout drift");

            struct STORAGE_DESCRIPTOR_HEADER {
                uint32_t Version;
                uint32_t Size;
            };

            static_assert(sizeof(STORAGE_DESCRIPTOR_HEADER) == 8u, "layouts::STORAGE_DESCRIPTOR_HEADER x64 layout drift");

            struct DISK_EXTENT {
                uint32_t DiskNumber;
                int64_t StartingOffset;
                int64_t ExtentLength;
            };

            static_assert(sizeof(DISK_EXTENT) == 24u, "layouts::DISK_EXTENT x64 layout drift");
            static_assert(offsetof(DISK_EXTENT, StartingOffset) == 8u, "layouts::DISK_EXTENT x64 layout drift");

            struct VOLUME_DISK_EXTENTS {
                uint32_t NumberOfDiskExtents;
                DISK_EXTENT Extents[1u];
            };

            static_assert(sizeof(VOLUME_DISK_EXTENTS) == 32u, "layouts::VOLUME_DISK_EXTENTS x64 layout drift");
            static_assert(offsetof(VOLUME_DISK_EXTENTS, Extents) == 8u, "layouts::VOLUME_DISK_EXTENTS x64 layout drift");

            struct MOUNTDEV_NAME {
                uint16_t NameLength;
                wchar_t Name[1u];
            };

            static_assert(sizeof(MOUNTDEV_NAME) == 4u, "layouts::MOUNTDEV_NAME x64 layout drift");
            static_assert(offsetof(MOUNTDEV_NAME, Name) == 2u, "layouts::MOUNTDEV_NAME x64 layout drift");

            // quota entry layouts

            struct SID {
                uint8_t Revision;
                uint8_t SubAuthorityCount;
                uint8_t IdentifierAuthority[6u];
                uint32_t SubAuthority[1u];
            };

            static_assert(sizeof(SID) == 12u, "layouts::SID x64 layout drift");
            static_assert(offsetof(SID, SubAuthority) == 8u, "layouts::SID x64 layout drift");

            struct FILE_GET_QUOTA_INFORMATION {
                uint32_t NextEntryOffset;
                uint32_t SidLength;
                SID Sid;
            };

            static_assert(sizeof(FILE_GET_QUOTA_INFORMATION) == 20u, "layouts::FILE_GET_QUOTA_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_GET_QUOTA_INFORMATION, Sid) == 8u, "layouts::FILE_GET_QUOTA_INFORMATION x64 layout drift");

            struct FILE_QUOTA_INFORMATION {
                uint32_t NextEntryOffset;
                uint32_t SidLength;
                int64_t ChangeTime;
                int64_t QuotaUsed;
                int64_t QuotaThreshold;
                int64_t QuotaLimit;
                SID Sid;
            };

            static_assert(sizeof(FILE_QUOTA_INFORMATION) == 56u, "layouts::FILE_QUOTA_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_QUOTA_INFORMATION, ChangeTime) == 8u, "layouts::FILE_QUOTA_INFORMATION x64 layout drift");
            static_assert(offsetof(FILE_QUOTA_INFORMATION, Sid) == 40u, "layouts::FILE_QUOTA_INFORMATION x64 layout drift");

        }

    }

}
