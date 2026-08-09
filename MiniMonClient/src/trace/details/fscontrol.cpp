#include "fscontrol.h"

#include "payload.h"

#include "..\kernel.h"
#include "..\names.h"

#include "..\..\..\..\inc\protocol.h"

#include <Windows.h>
#include <winioctl.h>

#include <cstddef>
#include <cstring>
#include <format>
#include <span>
#include <string>
#include <string_view>

using namespace mimo;

namespace {

    std::span<const uint8_t> ExtractInput(const protocol::FsControlSupplement& supplement) {

        if (!(supplement.captured & protocol::FS_CONTROL_CAPTURED_INPUT)) return {};

        return { supplement.inputPayload, supplement.capturedInputBytes };
    }


    std::wstring RenderCompressionPayload(uint16_t compressionFormat) {

        return std::format(L"CompressionFormat: {}", trace::names::RenderCompressionFormat(compressionFormat));
    }


    std::wstring RenderMoveFilePayload(const trace::kernel::MOVE_FILE_DATA& payload) {

        return std::format(L"FileHandle: 0x{:X}, StartingVcn: {}, StartingLcn: {}, ClusterCount: {}", static_cast<uint32_t>(payload.FileHandle), payload.StartingVcn, payload.StartingLcn, payload.ClusterCount);
    }


    std::wstring RenderGuid(std::span<const uint8_t, 16u> guid) {
        uint32_t data1 = 0u;
        std::memcpy(&data1, guid.data(), sizeof(data1));

        uint16_t data2 = 0u;
        std::memcpy(&data2, guid.data() + 4u, sizeof(data2));

        uint16_t data3 = 0u;
        std::memcpy(&data3, guid.data() + 6u, sizeof(data3));

        return std::format(L"{:08X}-{:04X}-{:04X}-{:02X}{:02X}-{:02X}{:02X}{:02X}{:02X}{:02X}{:02X}", data1, data2, data3, guid[8u], guid[9u], guid[10u], guid[11u], guid[12u], guid[13u], guid[14u], guid[15u]);
    }


    std::wstring RenderObjectIdPayload(const trace::kernel::FILE_OBJECTID_BUFFER& payload) {

        return std::format(L"ObjectId: {}, BirthVolumeId: {}, BirthObjectId: {}, DomainId: {}", RenderGuid(payload.ObjectId), RenderGuid(payload.BirthVolumeId), RenderGuid(payload.BirthObjectId), RenderGuid(payload.DomainId));
    }


    std::wstring RenderPathName(std::wstring_view label, std::span<const uint8_t> nameData, uint16_t nameOffset, uint16_t nameBytes) {

        if (!nameBytes) return {};

        const size_t start = nameOffset < nameData.size() ? nameOffset : nameData.size();

        return std::format(L"{}: {}", label, trace::details::payload::RenderName(nameData.subspan(start), nameBytes));
    }


    std::wstring RenderSymlinkPayload(const trace::kernel::REPARSE_DATA_BUFFER& payload, std::span<const uint8_t> nameData) {
        std::wstring result;

        const std::wstring substituteText = RenderPathName(L"SubstituteName", nameData, payload.SymbolicLinkReparseBuffer.SubstituteNameOffset, payload.SymbolicLinkReparseBuffer.SubstituteNameLength);

        if (!substituteText.empty()) {
            result += substituteText;
            result += L", ";
        }

        const std::wstring printText = RenderPathName(L"PrintName", nameData, payload.SymbolicLinkReparseBuffer.PrintNameOffset, payload.SymbolicLinkReparseBuffer.PrintNameLength);

        if (!printText.empty()) {
            result += printText;
            result += L", ";
        }

        const std::wstring flags = trace::names::RenderSymlinkFlags(payload.SymbolicLinkReparseBuffer.Flags);

        if (!flags.empty()) {
            result += std::format(L"Flags: {}, ", flags);
        }

        if (!result.empty()) {
            result.resize(result.size() - 2u);
        }

        return result;
    }


    std::wstring RenderMountPointPayload(const trace::kernel::REPARSE_DATA_BUFFER& payload, std::span<const uint8_t> nameData) {
        std::wstring result;

        const std::wstring substituteText = RenderPathName(L"SubstituteName", nameData, payload.MountPointReparseBuffer.SubstituteNameOffset, payload.MountPointReparseBuffer.SubstituteNameLength);

        if (!substituteText.empty()) {
            result += substituteText;
            result += L", ";
        }

        const std::wstring printText = RenderPathName(L"PrintName", nameData, payload.MountPointReparseBuffer.PrintNameOffset, payload.MountPointReparseBuffer.PrintNameLength);

        if (!printText.empty()) {
            result += printText;
            result += L", ";
        }

        if (!result.empty()) {
            result.resize(result.size() - 2u);
        }

        return result;
    }


    std::wstring RenderReparsePayload(std::span<const uint8_t> payload) {
        constexpr ULONG HEADER_BYTES = static_cast<ULONG>(offsetof(trace::kernel::REPARSE_DATA_BUFFER, SymbolicLinkReparseBuffer));
        trace::kernel::REPARSE_DATA_BUFFER reparse;

        if (!trace::details::payload::ReadHeader(payload, reparse, HEADER_BYTES)) return {};

        if (!reparse.ReparseTag) return {};

        std::wstring result = std::format(L"ReparseTag: {}", trace::names::RenderReparseTag(reparse.ReparseTag));
        std::wstring tagText;

        switch (reparse.ReparseTag) {

            case IO_REPARSE_TAG_SYMLINK: {
                constexpr ULONG NAME_OFFSET = static_cast<ULONG>(offsetof(trace::kernel::REPARSE_DATA_BUFFER, SymbolicLinkReparseBuffer.PathBuffer));

                if (trace::details::payload::ReadHeader(payload, reparse, NAME_OFFSET)) {
                    tagText = RenderSymlinkPayload(reparse, payload.subspan(NAME_OFFSET));
                }

                break;
            }

            case IO_REPARSE_TAG_MOUNT_POINT: {
                constexpr ULONG NAME_OFFSET = static_cast<ULONG>(offsetof(trace::kernel::REPARSE_DATA_BUFFER, MountPointReparseBuffer.PathBuffer));

                if (trace::details::payload::ReadHeader(payload, reparse, NAME_OFFSET)) {
                    tagText = RenderMountPointPayload(reparse, payload.subspan(NAME_OFFSET));
                }

                break;
            }

            default:
                tagText = std::format(L"ReparseDataLength: {}", reparse.ReparseDataLength);

                break;

        }

        if (!tagText.empty()) {
            result += L", ";
            result += tagText;
        }

        return result;
    }


    std::wstring RenderSparsePayload(const trace::kernel::FILE_SET_SPARSE_BUFFER& payload) {

        return std::format(L"SetSparse: {}", payload.SetSparse ? L"True" : L"False");
    }


    std::wstring RenderZeroDataPayload(const trace::kernel::FILE_ZERO_DATA_INFORMATION& payload) {

        return std::format(L"FileOffset: {}, BeyondFinalZero: {}", payload.FileOffset, payload.BeyondFinalZero);
    }


    std::wstring RenderAllocatedRangePayload(const trace::kernel::FILE_ALLOCATED_RANGE_BUFFER& payload) {

        return std::format(L"FileOffset: {}, Length: {}", payload.FileOffset, payload.Length);
    }


    std::wstring RenderTrimPayload(std::span<const uint8_t> payload) {
        constexpr ULONG RANGES_OFFSET = static_cast<ULONG>(offsetof(trace::kernel::FILE_LEVEL_TRIM, Ranges));
        trace::kernel::FILE_LEVEL_TRIM trim;

        if (!trace::details::payload::ReadHeader(payload, trim, RANGES_OFFSET)) return {};

        std::wstring result = std::format(L"Key: {}, NumRanges: {}", trim.Key, trim.NumRanges);
        ULONG offset = RANGES_OFFSET;
        ULONG index = 1u;

        while (index <= trim.NumRanges) {
            trace::kernel::FILE_LEVEL_TRIM_RANGE range;

            if (!trace::details::payload::ReadValue(payload, range, offset)) break;

            result += std::format(L", {}: Offset: {}, Length: {}", index, range.Offset, range.Length);
            offset += static_cast<ULONG>(sizeof(range));
            index++;
        }

        return result;
    }


    std::wstring RenderOplockInputPayload(const trace::kernel::REQUEST_OPLOCK_INPUT_BUFFER& payload) {

        return std::format(L"RequestedOplockLevel: {}, Flags: {}", trace::names::RenderOplockLevel(payload.RequestedOplockLevel), trace::names::RenderOplockInputFlags(payload.Flags));
    }


    std::wstring RenderFileRegionInputPayload(const trace::kernel::FILE_REGION_INPUT& payload) {

        return std::format(L"FileOffset: {}, Length: {}, DesiredUsage: {}", payload.FileOffset, payload.Length, trace::names::RenderFileRegionUsage(payload.DesiredUsage));
    }


    std::wstring RenderDuplicateExtentsPayload(const trace::kernel::DUPLICATE_EXTENTS_DATA& payload) {

        return std::format(L"FileHandle: 0x{:X}, SourceFileOffset: {}, TargetFileOffset: {}, ByteCount: {}", static_cast<uint32_t>(payload.FileHandle), payload.SourceFileOffset, payload.TargetFileOffset, payload.ByteCount);
    }


    std::wstring RenderInput(const protocol::FltParameters& parameters, std::span<const uint8_t> input) {

        switch (parameters.fileSystemControl.fsControlCode) {

            case FSCTL_SET_COMPRESSION: {
                uint16_t compressionFormat;

                if (trace::details::payload::ReadValue(input, compressionFormat)) return RenderCompressionPayload(compressionFormat);

                break;
            }

            case FSCTL_MOVE_FILE: {
                trace::kernel::MOVE_FILE_DATA moveFile;

                if (trace::details::payload::ReadValue(input, moveFile)) return RenderMoveFilePayload(moveFile);

                break;
            }

            case FSCTL_SET_OBJECT_ID: {
                trace::kernel::FILE_OBJECTID_BUFFER objectId;

                if (trace::details::payload::ReadValue(input, objectId)) return RenderObjectIdPayload(objectId);

                break;
            }

            case FSCTL_SET_REPARSE_POINT:
            case FSCTL_DELETE_REPARSE_POINT:
                return RenderReparsePayload(input);

            case FSCTL_SET_SPARSE: {
                // a missing/zero-length input buffer means SetSparse = TRUE (documented default)
                trace::kernel::FILE_SET_SPARSE_BUFFER sparse{ 1u };

                if (!parameters.fileSystemControl.inputBufferLength || trace::details::payload::ReadValue(input, sparse)) return RenderSparsePayload(sparse);

                break;
            }

            case FSCTL_SET_ZERO_DATA: {
                trace::kernel::FILE_ZERO_DATA_INFORMATION zeroData;

                if (trace::details::payload::ReadValue(input, zeroData)) return RenderZeroDataPayload(zeroData);

                break;
            }

            case FSCTL_QUERY_ALLOCATED_RANGES: {
                trace::kernel::FILE_ALLOCATED_RANGE_BUFFER range;

                if (trace::details::payload::ReadValue(input, range)) return RenderAllocatedRangePayload(range);

                break;
            }

            case FSCTL_FILE_LEVEL_TRIM:
                return RenderTrimPayload(input);

            case FSCTL_REQUEST_OPLOCK: {
                trace::kernel::REQUEST_OPLOCK_INPUT_BUFFER oplock;

                if (trace::details::payload::ReadValue(input, oplock)) return RenderOplockInputPayload(oplock);

                break;
            }

            case FSCTL_QUERY_FILE_REGIONS: {
                trace::kernel::FILE_REGION_INPUT region;

                if (trace::details::payload::ReadValue(input, region)) return RenderFileRegionInputPayload(region);

                break;
            }

            case FSCTL_DUPLICATE_EXTENTS_TO_FILE: {
                trace::kernel::DUPLICATE_EXTENTS_DATA duplicateExtents;

                if (trace::details::payload::ReadValue(input, duplicateExtents)) return RenderDuplicateExtentsPayload(duplicateExtents);

                break;
            }

        }

        return {};
    }


    std::span<const uint8_t> ExtractOutput(const protocol::FsControlSupplement& supplement) {

        if (!(supplement.captured & protocol::FS_CONTROL_CAPTURED_OUTPUT)) return {};

        return { supplement.outputPayload, supplement.capturedOutputBytes };
    }


    template <typename Statistics>
    std::wstring RenderStatisticsPayload(const Statistics& payload) {

        return std::format(L"FileSystemType: {}, Version: {}, SizeOfCompleteStructure: {}, UserFileReads: {}, UserFileReadBytes: {}, UserDiskReads: {}, UserFileWrites: {}, UserFileWriteBytes: {}, UserDiskWrites: {}, MetaDataReads: {}, MetaDataReadBytes: {}, MetaDataDiskReads: {}, MetaDataWrites: {}, MetaDataWriteBytes: {}, MetaDataDiskWrites: {}", trace::names::RenderFileSystemStatisticsType(payload.FileSystemType), payload.Version, payload.SizeOfCompleteStructure, payload.UserFileReads, payload.UserFileReadBytes, payload.UserDiskReads, payload.UserFileWrites, payload.UserFileWriteBytes, payload.UserDiskWrites, payload.MetaDataReads, payload.MetaDataReadBytes, payload.MetaDataDiskReads, payload.MetaDataWrites, payload.MetaDataWriteBytes, payload.MetaDataDiskWrites);
    }


    std::wstring RenderNtfsVolumeDataPayload(const trace::kernel::NTFS_VOLUME_DATA_BUFFER& payload) {

        return std::format(L"VolumeSerialNumber: 0x{:X}, NumberSectors: {}, TotalClusters: {}, FreeClusters: {}, TotalReserved: {}, BytesPerSector: {}, BytesPerCluster: {}, BytesPerFileRecordSegment: {}, ClustersPerFileRecordSegment: {}, MftValidDataLength: {}, MftStartLcn: {}, Mft2StartLcn: {}, MftZoneStart: {}, MftZoneEnd: {}", payload.VolumeSerialNumber, payload.NumberSectors, payload.TotalClusters, payload.FreeClusters, payload.TotalReserved, payload.BytesPerSector, payload.BytesPerCluster, payload.BytesPerFileRecordSegment, payload.ClustersPerFileRecordSegment, payload.MftValidDataLength, payload.MftStartLcn, payload.Mft2StartLcn, payload.MftZoneStart, payload.MftZoneEnd);
    }


    std::wstring RenderOplockOutputPayload(const trace::kernel::REQUEST_OPLOCK_OUTPUT_BUFFER& payload) {
        std::wstring result = std::format(L"OriginalOplockLevel: {}, NewOplockLevel: {}, Flags: {}", trace::names::RenderOplockLevel(payload.OriginalOplockLevel), trace::names::RenderOplockLevel(payload.NewOplockLevel), trace::names::RenderOplockOutputFlags(payload.Flags));

        if (payload.Flags & REQUEST_OPLOCK_OUTPUT_FLAG_MODES_PROVIDED) {
            result += std::format(L", AccessMode: {}, ShareMode: {}", trace::names::RenderDesiredAccess(payload.AccessMode), trace::names::RenderShareAccess(payload.ShareMode));
        }

        return result;
    }


    std::wstring RenderFileRegionOutputPayload(std::span<const uint8_t> payload) {
        constexpr ULONG REGION_OFFSET = static_cast<ULONG>(offsetof(trace::kernel::FILE_REGION_OUTPUT, Region));
        trace::kernel::FILE_REGION_OUTPUT regions;

        if (!trace::details::payload::ReadHeader(payload, regions, REGION_OFFSET)) return {};

        std::wstring result = std::format(L"TotalRegionEntryCount: {}, RegionEntryCount: {}", regions.TotalRegionEntryCount, regions.RegionEntryCount);
        ULONG offset = REGION_OFFSET;
        ULONG index = 1u;

        while (index <= regions.RegionEntryCount) {
            trace::kernel::FILE_REGION_INFO region;

            if (!trace::details::payload::ReadValue(payload, region, offset)) break;

            result += std::format(L", {}: FileOffset: {}, Length: {}, Usage: {}", index, region.FileOffset, region.Length, trace::names::RenderFileRegionUsage(region.Usage));
            offset += static_cast<ULONG>(sizeof(region));
            index++;
        }

        return result;
    }


    std::wstring RenderOutput(uint32_t fsControlCode, std::span<const uint8_t> output) {

        switch (fsControlCode) {

            case FSCTL_GET_COMPRESSION: {
                uint16_t compressionFormat;

                if (trace::details::payload::ReadValue(output, compressionFormat)) return RenderCompressionPayload(compressionFormat);

                break;
            }

            case FSCTL_FILESYSTEM_GET_STATISTICS: {
                trace::kernel::FILESYSTEM_STATISTICS statistics;

                if (trace::details::payload::ReadValue(output, statistics)) return RenderStatisticsPayload(statistics);

                break;
            }

            case FSCTL_GET_NTFS_VOLUME_DATA: {
                trace::kernel::NTFS_VOLUME_DATA_BUFFER volumeData;

                if (trace::details::payload::ReadValue(output, volumeData)) return RenderNtfsVolumeDataPayload(volumeData);

                break;
            }

            case FSCTL_GET_OBJECT_ID:
            case FSCTL_CREATE_OR_GET_OBJECT_ID: {
                trace::kernel::FILE_OBJECTID_BUFFER objectId;

                if (trace::details::payload::ReadValue(output, objectId)) return RenderObjectIdPayload(objectId);

                break;
            }

            case FSCTL_GET_REPARSE_POINT:
                return RenderReparsePayload(output);

            case FSCTL_REQUEST_OPLOCK: {
                trace::kernel::REQUEST_OPLOCK_OUTPUT_BUFFER oplock;

                if (trace::details::payload::ReadValue(output, oplock)) return RenderOplockOutputPayload(oplock);

                break;
            }

            case FSCTL_QUERY_FILE_REGIONS:
                return RenderFileRegionOutputPayload(output);

            case FSCTL_FILESYSTEM_GET_STATISTICS_EX: {
                trace::kernel::FILESYSTEM_STATISTICS_EX statistics;

                if (trace::details::payload::ReadValue(output, statistics)) return RenderStatisticsPayload(statistics);

                break;
            }

        }

        return {};
    }

}

namespace mimo {

    namespace trace {

        namespace details {

            namespace fscontrol {

                std::wstring Render(const protocol::RecordData& data) {

                    if (data.callbackMinorId != kernel::IRP_MN_USER_FS_REQUEST && data.callbackMinorId != kernel::IRP_MN_KERNEL_CALL) return {};

                    const protocol::FltParameters& parameters = data.parameters;
                    std::wstring details = std::format(L"Control: {}, InputBufferLength: {}, OutputBufferLength: {}", names::RenderFsControlCode(parameters.fileSystemControl.fsControlCode), parameters.fileSystemControl.inputBufferLength, parameters.fileSystemControl.outputBufferLength);
                    const protocol::FsControlSupplement& fsControlSupplement = data.supplement.fsControl;
                    const std::wstring inputText = RenderInput(parameters, ExtractInput(fsControlSupplement));

                    if (!inputText.empty()) {
                        details += L", ";
                        details += inputText;
                    }

                    const std::wstring outputText = RenderOutput(parameters.fileSystemControl.fsControlCode, ExtractOutput(fsControlSupplement));

                    if (!outputText.empty()) {
                        details += L", ";
                        details += outputText;
                    }

                    return details;
                }

            }

        }

    }

}
