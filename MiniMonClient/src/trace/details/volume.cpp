#include "volume.h"

#include "..\names.h"
#include "..\payload.h"
#include "..\values.h"

#include "..\..\..\..\inc\kernel.h"
#include "..\..\..\..\inc\protocol.h"

#include <cstddef>
#include <cstdint>
#include <format>
#include <span>
#include <string>

using namespace mimo;

namespace {

    std::wstring RenderParameters(uint32_t infoClass, uint32_t size) {

        return std::format(L"Class: {}, Length: {}", trace::names::RenderFsInfoClass(infoClass), size);
    }


    std::span<const uint8_t> ExtractPayload(const protocol::VolumeInfoSupplement& supplement) {

        if (!(supplement.captured & protocol::VOLUME_INFO_CAPTURED_PAYLOAD)) return {};

        return { supplement.payload, supplement.capturedSize };
    }


    std::wstring RenderVolumePayload(const kernel::FILE_FS_VOLUME_INFORMATION& payload, std::span<const uint8_t> labelData) {
        std::wstring result;

        if (payload.VolumeCreationTime) {
            result += std::format(L"VolumeCreationTime: {}, ", trace::values::RenderTime(payload.VolumeCreationTime));
        }

        result += std::format(L"VolumeSerialNumber: {:04X}-{:04X}, SupportsObjects: {}", payload.VolumeSerialNumber >> 16, payload.VolumeSerialNumber & 0xFFFFu, trace::values::RenderBoolean(payload.SupportsObjects));

        if (payload.VolumeLabelLength) {
            result += std::format(L", VolumeLabel: {}", trace::payload::RenderName(labelData, payload.VolumeLabelLength));
        }

        return result;
    }


    std::wstring RenderLabelPayload(const kernel::FILE_FS_LABEL_INFORMATION& payload, std::span<const uint8_t> labelData) {

        if (!payload.VolumeLabelLength) return {};

        return std::format(L"VolumeLabel: {}", trace::payload::RenderName(labelData, payload.VolumeLabelLength));
    }


    std::wstring RenderSizePayload(const kernel::FILE_FS_SIZE_INFORMATION& payload) {

        return std::format(L"TotalAllocationUnits: {}, AvailableAllocationUnits: {}, SectorsPerAllocationUnit: {}, BytesPerSector: {}", payload.TotalAllocationUnits, payload.AvailableAllocationUnits, payload.SectorsPerAllocationUnit, payload.BytesPerSector);
    }


    std::wstring RenderDevicePayload(const kernel::FILE_FS_DEVICE_INFORMATION& payload) {

        return std::format(L"DeviceType: {}, Characteristics: 0x{:X}", trace::names::RenderDeviceType(payload.DeviceType), payload.Characteristics);
    }


    std::wstring RenderAttributePayload(const kernel::FILE_FS_ATTRIBUTE_INFORMATION& payload, std::span<const uint8_t> nameData) {
        std::wstring result = std::format(L"FileSystemAttributes: {}, MaximumComponentNameLength: {}", trace::names::RenderFileSystemAttributes(payload.FileSystemAttributes), payload.MaximumComponentNameLength);

        if (payload.FileSystemNameLength) {
            result += std::format(L", FileSystemName: {}", trace::payload::RenderName(nameData, payload.FileSystemNameLength));
        }

        return result;
    }


    std::wstring RenderControlPayload(const kernel::FILE_FS_CONTROL_INFORMATION& payload) {

        return std::format(L"FreeSpaceStartFiltering: {}, FreeSpaceThreshold: {}, FreeSpaceStopFiltering: {}, DefaultQuotaThreshold: {}, DefaultQuotaLimit: {}, FileSystemControlFlags: {}", payload.FreeSpaceStartFiltering, payload.FreeSpaceThreshold, payload.FreeSpaceStopFiltering, payload.DefaultQuotaThreshold, payload.DefaultQuotaLimit, trace::names::RenderFileSystemControlFlags(payload.FileSystemControlFlags));
    }


    std::wstring RenderFullSizePayload(const kernel::FILE_FS_FULL_SIZE_INFORMATION& payload) {

        return std::format(L"TotalAllocationUnits: {}, CallerAvailableAllocationUnits: {}, ActualAvailableAllocationUnits: {}, SectorsPerAllocationUnit: {}, BytesPerSector: {}", payload.TotalAllocationUnits, payload.CallerAvailableAllocationUnits, payload.ActualAvailableAllocationUnits, payload.SectorsPerAllocationUnit, payload.BytesPerSector);
    }


    std::wstring RenderObjectIdPayload(const kernel::FILE_FS_OBJECTID_INFORMATION& payload) {

        return std::format(L"ObjectId: {}, ExtendedInfo: {}", trace::values::RenderGuid(payload.ObjectId), trace::values::RenderBytes(payload.ExtendedInfo, false));
    }


    std::wstring RenderVolumeFlagsPayload(const kernel::FILE_FS_VOLUME_FLAGS_INFORMATION& payload) {

        return std::format(L"Flags: 0x{:X}", payload.Flags);
    }


    std::wstring RenderSectorSizePayload(const kernel::FILE_FS_SECTOR_SIZE_INFORMATION& payload) {

        return std::format(L"LogicalBytesPerSector: {}, PhysicalBytesPerSectorForAtomicity: {}, PhysicalBytesPerSectorForPerformance: {}, FileSystemEffectivePhysicalBytesPerSectorForAtomicity: {}, Flags: 0x{:X}, ByteOffsetForSectorAlignment: {}, ByteOffsetForPartitionAlignment: {}", payload.LogicalBytesPerSector, payload.PhysicalBytesPerSectorForAtomicity, payload.PhysicalBytesPerSectorForPerformance, payload.FileSystemEffectivePhysicalBytesPerSectorForAtomicity, payload.Flags, payload.ByteOffsetForSectorAlignment, payload.ByteOffsetForPartitionAlignment);
    }


    std::wstring RenderDataCopyPayload(const kernel::FILE_FS_DATA_COPY_INFORMATION& payload) {

        return std::format(L"NumberOfCopies: {}", payload.NumberOfCopies);
    }


    std::wstring RenderMetadataSizePayload(const kernel::FILE_FS_METADATA_SIZE_INFORMATION& payload) {

        return std::format(L"TotalMetadataAllocationUnits: {}, SectorsPerAllocationUnit: {}, BytesPerSector: {}", payload.TotalMetadataAllocationUnits, payload.SectorsPerAllocationUnit, payload.BytesPerSector);
    }


    std::wstring RenderFullSizeExPayload(const kernel::FILE_FS_FULL_SIZE_INFORMATION_EX& payload) {

        return std::format(L"ActualTotalAllocationUnits: {}, ActualAvailableAllocationUnits: {}, ActualPoolUnavailableAllocationUnits: {}, CallerTotalAllocationUnits: {}, CallerAvailableAllocationUnits: {}, CallerPoolUnavailableAllocationUnits: {}, UsedAllocationUnits: {}, TotalReservedAllocationUnits: {}, VolumeStorageReserveAllocationUnits: {}, AvailableCommittedAllocationUnits: {}, PoolAvailableAllocationUnits: {}, SectorsPerAllocationUnit: {}, BytesPerSector: {}", payload.ActualTotalAllocationUnits, payload.ActualAvailableAllocationUnits, payload.ActualPoolUnavailableAllocationUnits, payload.CallerTotalAllocationUnits, payload.CallerAvailableAllocationUnits, payload.CallerPoolUnavailableAllocationUnits, payload.UsedAllocationUnits, payload.TotalReservedAllocationUnits, payload.VolumeStorageReserveAllocationUnits, payload.AvailableCommittedAllocationUnits, payload.PoolAvailableAllocationUnits, payload.SectorsPerAllocationUnit, payload.BytesPerSector);
    }


    std::wstring RenderGuidPayload(const kernel::FILE_FS_GUID_INFORMATION& payload) {

        return std::format(L"FsGuid: {}", trace::values::RenderGuid(payload.FsGuid));
    }


    std::wstring RenderPayload(uint32_t infoClass, const protocol::VolumeInfoSupplement& supplement) {
        const std::span<const uint8_t> payload = ExtractPayload(supplement);

        switch (infoClass) {

            case kernel::FileFsVolumeInformation: {
                constexpr size_t LABEL_OFFSET = offsetof(kernel::FILE_FS_VOLUME_INFORMATION, VolumeLabel);
                kernel::FILE_FS_VOLUME_INFORMATION volume{};

                if (trace::payload::ReadHeader(payload, volume, LABEL_OFFSET)) return RenderVolumePayload(volume, payload.subspan(LABEL_OFFSET));

                break;
            }

            case kernel::FileFsLabelInformation: {
                constexpr size_t LABEL_OFFSET = offsetof(kernel::FILE_FS_LABEL_INFORMATION, VolumeLabel);
                kernel::FILE_FS_LABEL_INFORMATION label{};

                if (trace::payload::ReadHeader(payload, label, LABEL_OFFSET)) return RenderLabelPayload(label, payload.subspan(LABEL_OFFSET));

                break;
            }

            case kernel::FileFsSizeInformation: {
                kernel::FILE_FS_SIZE_INFORMATION size{};

                if (trace::payload::ReadValue(payload, size)) return RenderSizePayload(size);

                break;
            }

            case kernel::FileFsDeviceInformation: {
                kernel::FILE_FS_DEVICE_INFORMATION device{};

                if (trace::payload::ReadValue(payload, device)) return RenderDevicePayload(device);

                break;
            }

            case kernel::FileFsAttributeInformation: {
                constexpr size_t NAME_OFFSET = offsetof(kernel::FILE_FS_ATTRIBUTE_INFORMATION, FileSystemName);
                kernel::FILE_FS_ATTRIBUTE_INFORMATION attribute{};

                if (trace::payload::ReadHeader(payload, attribute, NAME_OFFSET)) return RenderAttributePayload(attribute, payload.subspan(NAME_OFFSET));

                break;
            }

            case kernel::FileFsControlInformation: {
                kernel::FILE_FS_CONTROL_INFORMATION control{};

                if (trace::payload::ReadValue(payload, control)) return RenderControlPayload(control);

                break;
            }

            case kernel::FileFsFullSizeInformation: {
                kernel::FILE_FS_FULL_SIZE_INFORMATION fullSize{};

                if (trace::payload::ReadValue(payload, fullSize)) return RenderFullSizePayload(fullSize);

                break;
            }

            case kernel::FileFsObjectIdInformation: {
                kernel::FILE_FS_OBJECTID_INFORMATION objectId{};

                if (trace::payload::ReadValue(payload, objectId)) return RenderObjectIdPayload(objectId);

                break;
            }

            case kernel::FileFsVolumeFlagsInformation: {
                kernel::FILE_FS_VOLUME_FLAGS_INFORMATION volumeFlags{};

                if (trace::payload::ReadValue(payload, volumeFlags)) return RenderVolumeFlagsPayload(volumeFlags);

                break;
            }

            case kernel::FileFsSectorSizeInformation: {
                kernel::FILE_FS_SECTOR_SIZE_INFORMATION sectorSize{};

                if (trace::payload::ReadValue(payload, sectorSize)) return RenderSectorSizePayload(sectorSize);

                break;
            }

            case kernel::FileFsDataCopyInformation: {
                kernel::FILE_FS_DATA_COPY_INFORMATION dataCopy{};

                if (trace::payload::ReadValue(payload, dataCopy)) return RenderDataCopyPayload(dataCopy);

                break;
            }

            case kernel::FileFsMetadataSizeInformation: {
                kernel::FILE_FS_METADATA_SIZE_INFORMATION metadataSize{};

                if (trace::payload::ReadValue(payload, metadataSize)) return RenderMetadataSizePayload(metadataSize);

                break;
            }

            case kernel::FileFsFullSizeInformationEx: {
                kernel::FILE_FS_FULL_SIZE_INFORMATION_EX fullSizeEx{};

                if (trace::payload::ReadValue(payload, fullSizeEx)) return RenderFullSizeExPayload(fullSizeEx);

                break;
            }

            case kernel::FileFsGuidInformation: {
                kernel::FILE_FS_GUID_INFORMATION guid{};

                if (trace::payload::ReadValue(payload, guid)) return RenderGuidPayload(guid);

                break;
            }

        }

        return {};
    }

}

namespace mimo {

    namespace trace {

        namespace details {

            namespace volume {

                std::wstring Render(const protocol::RecordData& data) {
                    const protocol::FltParameters& parameters = data.parameters;
                    std::wstring details = RenderParameters(parameters.volumeInformation.fsInformationClass, parameters.volumeInformation.length);

                    const std::wstring payloadText = RenderPayload(parameters.volumeInformation.fsInformationClass, data.supplement.volumeInfo);

                    if (!payloadText.empty()) {
                        details += L", ";
                        details += payloadText;
                    }

                    return details;
                }

            }

        }

    }

}
