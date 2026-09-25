#include "volume.h"

#include "payload.h"

#include "..\kernel.h"
#include "..\names.h"
#include "..\values.h"

#include "..\..\..\..\inc\protocol.h"

#include <cstddef>
#include <cstdint>
#include <format>
#include <span>
#include <string>

using namespace mimo;

namespace {

    std::wstring RenderInformationParameters(uint32_t fsInformationClass, uint32_t length) {

        return std::format(L"Class: {}, Length: {}", trace::names::RenderFsInformationClass(fsInformationClass), length);
    }


    std::span<const uint8_t> ExtractPayload(const protocol::VolumeInfoSupplement& supplement) {

        if (!(supplement.captured & protocol::VOLUME_INFO_CAPTURED_PAYLOAD)) return {};

        return { supplement.payload, supplement.capturedSize };
    }


    std::wstring RenderVolumePayload(const trace::kernel::FILE_FS_VOLUME_INFORMATION& payload, std::span<const uint8_t> labelData) {
        std::wstring result;

        if (payload.VolumeCreationTime) {
            result += std::format(L"VolumeCreationTime: {}, ", trace::values::RenderTime(payload.VolumeCreationTime));
        }

        result += std::format(L"VolumeSerialNumber: {:04X}-{:04X}, SupportsObjects: {}", payload.VolumeSerialNumber >> 16, payload.VolumeSerialNumber & 0xFFFFu, trace::values::RenderBoolean(payload.SupportsObjects));

        if (payload.VolumeLabelLength) {
            result += std::format(L", VolumeLabel: {}", trace::details::payload::RenderName(labelData, payload.VolumeLabelLength));
        }

        return result;
    }


    std::wstring RenderLabelPayload(const trace::kernel::FILE_FS_LABEL_INFORMATION& payload, std::span<const uint8_t> labelData) {

        if (!payload.VolumeLabelLength) return {};

        return std::format(L"VolumeLabel: {}", trace::details::payload::RenderName(labelData, payload.VolumeLabelLength));
    }


    std::wstring RenderSizePayload(const trace::kernel::FILE_FS_SIZE_INFORMATION& payload) {

        return std::format(L"TotalAllocationUnits: {}, AvailableAllocationUnits: {}, SectorsPerAllocationUnit: {}, BytesPerSector: {}", payload.TotalAllocationUnits, payload.AvailableAllocationUnits, payload.SectorsPerAllocationUnit, payload.BytesPerSector);
    }


    std::wstring RenderDevicePayload(const trace::kernel::FILE_FS_DEVICE_INFORMATION& payload) {

        return std::format(L"DeviceType: {}, Characteristics: 0x{:X}", trace::names::RenderDeviceType(payload.DeviceType), payload.Characteristics);
    }


    std::wstring RenderAttributePayload(const trace::kernel::FILE_FS_ATTRIBUTE_INFORMATION& payload, std::span<const uint8_t> nameData) {
        std::wstring result = std::format(L"FileSystemAttributes: {}, MaximumComponentNameLength: {}", trace::names::RenderFileSystemAttributes(payload.FileSystemAttributes), payload.MaximumComponentNameLength);

        if (payload.FileSystemNameLength) {
            result += std::format(L", FileSystemName: {}", trace::details::payload::RenderName(nameData, payload.FileSystemNameLength));
        }

        return result;
    }


    std::wstring RenderControlPayload(const trace::kernel::FILE_FS_CONTROL_INFORMATION& payload) {

        return std::format(L"FreeSpaceStartFiltering: {}, FreeSpaceThreshold: {}, FreeSpaceStopFiltering: {}, DefaultQuotaThreshold: {}, DefaultQuotaLimit: {}, FileSystemControlFlags: {}", payload.FreeSpaceStartFiltering, payload.FreeSpaceThreshold, payload.FreeSpaceStopFiltering, payload.DefaultQuotaThreshold, payload.DefaultQuotaLimit, trace::names::RenderFileSystemControlFlags(payload.FileSystemControlFlags));
    }


    std::wstring RenderFullSizePayload(const trace::kernel::FILE_FS_FULL_SIZE_INFORMATION& payload) {

        return std::format(L"TotalAllocationUnits: {}, CallerAvailableAllocationUnits: {}, ActualAvailableAllocationUnits: {}, SectorsPerAllocationUnit: {}, BytesPerSector: {}", payload.TotalAllocationUnits, payload.CallerAvailableAllocationUnits, payload.ActualAvailableAllocationUnits, payload.SectorsPerAllocationUnit, payload.BytesPerSector);
    }


    std::wstring RenderObjectIdPayload(const trace::kernel::FILE_FS_OBJECTID_INFORMATION& payload) {

        return std::format(L"ObjectId: {}, ExtendedInfo: {}", trace::values::RenderGuid(payload.ObjectId), trace::values::RenderBytes(payload.ExtendedInfo, false));
    }


    std::wstring RenderSectorSizePayload(const trace::kernel::FILE_FS_SECTOR_SIZE_INFORMATION& payload) {

        return std::format(L"LogicalBytesPerSector: {}, PhysicalBytesPerSectorForAtomicity: {}, PhysicalBytesPerSectorForPerformance: {}, FileSystemEffectivePhysicalBytesPerSectorForAtomicity: {}, Flags: 0x{:X}, ByteOffsetForSectorAlignment: {}, ByteOffsetForPartitionAlignment: {}", payload.LogicalBytesPerSector, payload.PhysicalBytesPerSectorForAtomicity, payload.PhysicalBytesPerSectorForPerformance, payload.FileSystemEffectivePhysicalBytesPerSectorForAtomicity, payload.Flags, payload.ByteOffsetForSectorAlignment, payload.ByteOffsetForPartitionAlignment);
    }


    std::wstring RenderMetadataSizePayload(const trace::kernel::FILE_FS_METADATA_SIZE_INFORMATION& payload) {

        return std::format(L"TotalMetadataAllocationUnits: {}, SectorsPerAllocationUnit: {}, BytesPerSector: {}", payload.TotalMetadataAllocationUnits, payload.SectorsPerAllocationUnit, payload.BytesPerSector);
    }


    std::wstring RenderFullSizeExPayload(const trace::kernel::FILE_FS_FULL_SIZE_INFORMATION_EX& payload) {

        return std::format(L"ActualTotalAllocationUnits: {}, ActualAvailableAllocationUnits: {}, ActualPoolUnavailableAllocationUnits: {}, CallerTotalAllocationUnits: {}, CallerAvailableAllocationUnits: {}, CallerPoolUnavailableAllocationUnits: {}, UsedAllocationUnits: {}, TotalReservedAllocationUnits: {}, VolumeStorageReserveAllocationUnits: {}, AvailableCommittedAllocationUnits: {}, PoolAvailableAllocationUnits: {}, SectorsPerAllocationUnit: {}, BytesPerSector: {}", payload.ActualTotalAllocationUnits, payload.ActualAvailableAllocationUnits, payload.ActualPoolUnavailableAllocationUnits, payload.CallerTotalAllocationUnits, payload.CallerAvailableAllocationUnits, payload.CallerPoolUnavailableAllocationUnits, payload.UsedAllocationUnits, payload.TotalReservedAllocationUnits, payload.VolumeStorageReserveAllocationUnits, payload.AvailableCommittedAllocationUnits, payload.PoolAvailableAllocationUnits, payload.SectorsPerAllocationUnit, payload.BytesPerSector);
    }

}

namespace mimo {

    namespace trace {

        namespace details {

            namespace volume {

                std::wstring Render(const protocol::RecordData& data) {
                    const protocol::FltParameters& parameters = data.parameters;
                    std::wstring details = RenderInformationParameters(parameters.volumeInformation.fsInformationClass, parameters.volumeInformation.length);

                    const protocol::VolumeInfoSupplement& volumeInfoSupplement = data.supplement.volumeInfo;
                    const std::span<const uint8_t> payload = ExtractPayload(volumeInfoSupplement);
                    std::wstring payloadText;

                    switch (parameters.volumeInformation.fsInformationClass) {

                        case kernel::FileFsVolumeInformation: {
                            constexpr size_t LABEL_OFFSET = offsetof(kernel::FILE_FS_VOLUME_INFORMATION, VolumeLabel);
                            kernel::FILE_FS_VOLUME_INFORMATION volume;

                            if (payload::ReadHeader(payload, volume, LABEL_OFFSET)) {
                                payloadText = RenderVolumePayload(volume, payload.subspan(LABEL_OFFSET));
                            }

                            break;
                        }

                        case kernel::FileFsLabelInformation: {
                            constexpr size_t LABEL_OFFSET = offsetof(kernel::FILE_FS_LABEL_INFORMATION, VolumeLabel);
                            kernel::FILE_FS_LABEL_INFORMATION label;

                            if (payload::ReadHeader(payload, label, LABEL_OFFSET)) {
                                payloadText = RenderLabelPayload(label, payload.subspan(LABEL_OFFSET));
                            }

                            break;
                        }

                        case kernel::FileFsSizeInformation: {
                            kernel::FILE_FS_SIZE_INFORMATION size;

                            if (payload::ReadValue(payload, size)) {
                                payloadText = RenderSizePayload(size);
                            }

                            break;
                        }

                        case kernel::FileFsDeviceInformation: {
                            kernel::FILE_FS_DEVICE_INFORMATION device;

                            if (payload::ReadValue(payload, device)) {
                                payloadText = RenderDevicePayload(device);
                            }

                            break;
                        }

                        case kernel::FileFsAttributeInformation: {
                            constexpr size_t NAME_OFFSET = offsetof(kernel::FILE_FS_ATTRIBUTE_INFORMATION, FileSystemName);
                            kernel::FILE_FS_ATTRIBUTE_INFORMATION attribute;

                            if (payload::ReadHeader(payload, attribute, NAME_OFFSET)) {
                                payloadText = RenderAttributePayload(attribute, payload.subspan(NAME_OFFSET));
                            }

                            break;
                        }

                        case kernel::FileFsControlInformation: {
                            kernel::FILE_FS_CONTROL_INFORMATION control;

                            if (payload::ReadValue(payload, control)) {
                                payloadText = RenderControlPayload(control);
                            }

                            break;
                        }

                        case kernel::FileFsFullSizeInformation: {
                            kernel::FILE_FS_FULL_SIZE_INFORMATION fullSize;

                            if (payload::ReadValue(payload, fullSize)) {
                                payloadText = RenderFullSizePayload(fullSize);
                            }

                            break;
                        }

                        case kernel::FileFsObjectIdInformation: {
                            kernel::FILE_FS_OBJECTID_INFORMATION objectId;

                            if (payload::ReadValue(payload, objectId)) {
                                payloadText = RenderObjectIdPayload(objectId);
                            }

                            break;
                        }

                        case kernel::FileFsSectorSizeInformation: {
                            kernel::FILE_FS_SECTOR_SIZE_INFORMATION sectorSize;

                            if (payload::ReadValue(payload, sectorSize)) {
                                payloadText = RenderSectorSizePayload(sectorSize);
                            }

                            break;
                        }

                        case kernel::FileFsMetadataSizeInformation: {
                            kernel::FILE_FS_METADATA_SIZE_INFORMATION metadataSize;

                            if (payload::ReadValue(payload, metadataSize)) {
                                payloadText = RenderMetadataSizePayload(metadataSize);
                            }

                            break;
                        }

                        case kernel::FileFsFullSizeInformationEx: {
                            kernel::FILE_FS_FULL_SIZE_INFORMATION_EX fullSizeEx;

                            if (payload::ReadValue(payload, fullSizeEx)) {
                                payloadText = RenderFullSizeExPayload(fullSizeEx);
                            }

                            break;
                        }

                    }

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
