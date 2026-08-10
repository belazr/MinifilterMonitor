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

    std::span<const uint8_t> ExtractPayload(const protocol::VolumeInfoSupplement& supplement) {

        if (!(supplement.captured & protocol::VOLUME_INFO_CAPTURED_PAYLOAD)) return {};

        return { supplement.payload, supplement.capturedBytes };
    }


    std::wstring RenderVolumePayload(const trace::kernel::FILE_FS_VOLUME_INFORMATION& payload, std::span<const uint8_t> labelData) {
        std::wstring result;

        if (payload.VolumeCreationTime) {
            result += std::format(L"VolumeCreationTime: {}, ", trace::values::RenderFileTime(payload.VolumeCreationTime));
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

        return std::format(L"DeviceType: {}, Characteristics: 0x{:X}", payload.DeviceType, payload.Characteristics);
    }


    std::wstring RenderAttributePayload(const trace::kernel::FILE_FS_ATTRIBUTE_INFORMATION& payload, std::span<const uint8_t> nameData) {
        std::wstring result = std::format(L"FileSystemAttributes: {}, MaximumComponentNameLength: {}", trace::names::RenderFileSystemAttributes(payload.FileSystemAttributes), payload.MaximumComponentNameLength);

        if (payload.FileSystemNameLength) {
            result += std::format(L", FileSystemName: {}", trace::details::payload::RenderName(nameData, payload.FileSystemNameLength));
        }

        return result;
    }


    std::wstring RenderFullSizePayload(const trace::kernel::FILE_FS_FULL_SIZE_INFORMATION& payload) {

        return std::format(L"TotalAllocationUnits: {}, CallerAvailableAllocationUnits: {}, ActualAvailableAllocationUnits: {}, SectorsPerAllocationUnit: {}, BytesPerSector: {}", payload.TotalAllocationUnits, payload.CallerAvailableAllocationUnits, payload.ActualAvailableAllocationUnits, payload.SectorsPerAllocationUnit, payload.BytesPerSector);
    }


    std::wstring RenderSectorSizePayload(const trace::kernel::FILE_FS_SECTOR_SIZE_INFORMATION& payload) {

        return std::format(L"LogicalBytesPerSector: {}, PhysicalBytesPerSectorForAtomicity: {}, PhysicalBytesPerSectorForPerformance: {}, FileSystemEffectivePhysicalBytesPerSectorForAtomicity: {}, Flags: 0x{:X}, ByteOffsetForSectorAlignment: {}, ByteOffsetForPartitionAlignment: {}", payload.LogicalBytesPerSector, payload.PhysicalBytesPerSectorForAtomicity, payload.PhysicalBytesPerSectorForPerformance, payload.FileSystemEffectivePhysicalBytesPerSectorForAtomicity, payload.Flags, payload.ByteOffsetForSectorAlignment, payload.ByteOffsetForPartitionAlignment);
    }

}

namespace mimo {

    namespace trace {

        namespace details {

            namespace volume {

                std::wstring Render(const protocol::RecordData& data) {
                    const protocol::FltParameters& parameters = data.parameters;
                    std::wstring details = std::format(L"Class: {}, Length: {}", names::RenderFsInformationClass(parameters.volumeInformation.fsInformationClass), parameters.volumeInformation.length);
                    const std::span<const uint8_t> payload = ExtractPayload(data.supplement.volumeInfo);
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

                        case kernel::FileFsFullSizeInformation: {
                            kernel::FILE_FS_FULL_SIZE_INFORMATION fullSize;

                            if (payload::ReadValue(payload, fullSize)) {
                                payloadText = RenderFullSizePayload(fullSize);
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
