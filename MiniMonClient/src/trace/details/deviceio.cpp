#include "deviceio.h"

#include "payload.h"

#include "..\kernel.h"
#include "..\names.h"
#include "..\values.h"

#include "..\..\text.h"

#include "..\..\..\..\inc\protocol.h"

#include <Windows.h>
#include <winioctl.h>
#include <ntddvol.h>    // after winioctl.h, whose volume section it repeats under an IOCTL_VOLUME_BASE guard

#include <cstddef>
#include <cstdint>
#include <format>
#include <span>
#include <string>
#include <string_view>

using namespace mimo;

namespace {

    std::span<const uint8_t> ExtractInput(const protocol::DeviceIoControlSupplement& supplement) {

        if (!(supplement.captured & protocol::DEVICE_IO_CONTROL_CAPTURED_INPUT)) return {};

        return { supplement.inputPayload, supplement.capturedInputSize };
    }


    std::wstring RenderPropertyQueryPayload(const trace::kernel::STORAGE_PROPERTY_QUERY& payload) {

        return std::format(L"PropertyId: {}, QueryType: {}", trace::names::RenderStoragePropertyId(payload.PropertyId), trace::names::RenderStorageQueryType(payload.QueryType));
    }


    std::wstring RenderDeviceNamePayload(std::span<const uint8_t> payload) {
        constexpr size_t NAME_OFFSET = offsetof(trace::kernel::MOUNTDEV_NAME, Name);
        trace::kernel::MOUNTDEV_NAME deviceName;

        if (!trace::details::payload::ReadHeader(payload, deviceName, NAME_OFFSET)) return {};

        return std::format(L"Name: {}", trace::details::payload::RenderName(payload.subspan(NAME_OFFSET), deviceName.NameLength));
    }


    std::wstring RenderInput(uint32_t ioControlCode, const protocol::DeviceIoControlSupplement& supplement) {
        const std::span<const uint8_t> input = ExtractInput(supplement);
        const bool truncated = supplement.captured & protocol::DEVICE_IO_CONTROL_TRUNCATED_INPUT;

        switch (ioControlCode) {

            case IOCTL_STORAGE_QUERY_PROPERTY: {
                constexpr size_t PARAMETERS_OFFSET = offsetof(trace::kernel::STORAGE_PROPERTY_QUERY, AdditionalParameters);
                trace::kernel::STORAGE_PROPERTY_QUERY propertyQuery;

                if (trace::details::payload::ReadHeader(input, propertyQuery, PARAMETERS_OFFSET)) return RenderPropertyQueryPayload(propertyQuery);

                break;
            }

            case trace::kernel::IOCTL_MOUNTDEV_LINK_CREATED:
            case trace::kernel::IOCTL_MOUNTDEV_LINK_DELETED:

                return RenderDeviceNamePayload(input);

            default:

                if (!input.empty()) return std::format(L"InputBuffer: {}", trace::values::RenderBytes(input, truncated));

                break;

        }

        return {};
    }


    std::span<const uint8_t> ExtractOutput(const protocol::DeviceIoControlSupplement& supplement) {

        if (!(supplement.captured & protocol::DEVICE_IO_CONTROL_CAPTURED_OUTPUT)) return {};

        return { supplement.outputPayload, supplement.capturedOutputSize };
    }


    std::wstring RenderGeometryPayload(const trace::kernel::DISK_GEOMETRY& payload) {

        return std::format(L"Cylinders: {}, MediaType: {}, TracksPerCylinder: {}, SectorsPerTrack: {}, BytesPerSector: {}", payload.Cylinders, trace::names::RenderMediaType(payload.MediaType), payload.TracksPerCylinder, payload.SectorsPerTrack, payload.BytesPerSector);
    }


    std::wstring RenderGeometryExPayload(const trace::kernel::DISK_GEOMETRY_EX& payload) {

        return std::format(L"{}, DiskSize: {}", RenderGeometryPayload(payload.Geometry), payload.DiskSize);
    }


    std::wstring RenderLengthPayload(const trace::kernel::GET_LENGTH_INFORMATION& payload) {

        return std::format(L"Length: {}", payload.Length);
    }


    std::wstring RenderMbr(const trace::kernel::PARTITION_INFORMATION_MBR& payload) {

        return std::format(L"PartitionType: 0x{:X}, BootIndicator: {}, RecognizedPartition: {}, HiddenSectors: {}, PartitionId: {}", payload.PartitionType, trace::values::RenderBoolean(payload.BootIndicator), trace::values::RenderBoolean(payload.RecognizedPartition), payload.HiddenSectors, trace::values::RenderGuid(payload.PartitionId));
    }


    std::wstring RenderGpt(const trace::kernel::PARTITION_INFORMATION_GPT& payload) {
        std::wstring result = std::format(L"PartitionType: {}, PartitionId: {}, Attributes: 0x{:X}", trace::values::RenderGuid(payload.PartitionType), trace::values::RenderGuid(payload.PartitionId), payload.Attributes);
        const std::wstring_view name = text::Extract(payload.Name);

        if (!name.empty()) {
            result += std::format(L", Name: {}", name);
        }

        return result;
    }


    std::wstring RenderPartitionExPayload(const trace::kernel::PARTITION_INFORMATION_EX& payload) {
        std::wstring result = std::format(L"PartitionStyle: {}", trace::names::RenderPartitionStyle(payload.PartitionStyle));

        if (payload.PartitionOrdinal) {
            result += std::format(L", PartitionOrdinal: {}", payload.PartitionOrdinal);
        }

        result += std::format(L", StartingOffset: {}, PartitionLength: {}, PartitionNumber: {}, RewritePartition: {}, IsServicePartition: {}", payload.StartingOffset, payload.PartitionLength, payload.PartitionNumber, trace::values::RenderBoolean(payload.RewritePartition), trace::values::RenderBoolean(payload.IsServicePartition));
        std::wstring styleText;

        switch (payload.PartitionStyle) {

            case PARTITION_STYLE_MBR:
                styleText = RenderMbr(payload.Mbr);

                break;

            case PARTITION_STYLE_GPT:
                styleText = RenderGpt(payload.Gpt);

                break;

        }

        if (!styleText.empty()) {
            result += L", ";
            result += styleText;
        }

        return result;
    }


    std::wstring RenderDiskAttributesPayload(const trace::kernel::GET_DISK_ATTRIBUTES& payload) {

        return std::format(L"Version: {}, Attributes: {}", payload.Version, trace::names::RenderDiskAttributes(static_cast<uint32_t>(payload.Attributes)));
    }


    std::wstring RenderHotplugPayload(const trace::kernel::STORAGE_HOTPLUG_INFO& payload) {

        return std::format(L"Size: {}, MediaRemovable: {}, MediaHotplug: {}, DeviceHotplug: {}, WriteCacheEnableOverride: {}", payload.Size, trace::values::RenderBoolean(payload.MediaRemovable), trace::values::RenderBoolean(payload.MediaHotplug), trace::values::RenderBoolean(payload.DeviceHotplug), trace::values::RenderBoolean(payload.WriteCacheEnableOverride));
    }


    std::wstring RenderDeviceNumberPayload(const trace::kernel::STORAGE_DEVICE_NUMBER& payload) {

        return std::format(L"DeviceType: {}, DeviceNumber: {}, PartitionNumber: {}", trace::names::RenderDeviceType(payload.DeviceType), payload.DeviceNumber, trace::values::RenderPartitionNumber(payload.PartitionNumber));
    }


    std::wstring RenderDeviceNumberExPayload(const trace::kernel::STORAGE_DEVICE_NUMBER_EX& payload) {

        return std::format(L"Version: {}, Size: {}, Flags: {}, DeviceType: {}, DeviceNumber: {}, DeviceGuid: {}, PartitionNumber: {}", payload.Version, payload.Size, trace::names::RenderStorageDeviceFlags(payload.Flags), trace::names::RenderDeviceType(payload.DeviceType), payload.DeviceNumber, trace::values::RenderGuid(payload.DeviceGuid), trace::values::RenderPartitionNumber(payload.PartitionNumber));
    }


    std::wstring RenderDescriptorHeaderPayload(const trace::kernel::STORAGE_DESCRIPTOR_HEADER& payload) {

        return std::format(L"Version: {}, Size: {}", payload.Version, payload.Size);
    }


    std::wstring RenderDiskExtentsPayload(std::span<const uint8_t> payload) {
        constexpr size_t EXTENTS_OFFSET = offsetof(trace::kernel::VOLUME_DISK_EXTENTS, Extents);
        trace::kernel::VOLUME_DISK_EXTENTS extents;

        if (!trace::details::payload::ReadHeader(payload, extents, EXTENTS_OFFSET)) return {};

        std::wstring result = std::format(L"NumberOfDiskExtents: {}, ", extents.NumberOfDiskExtents);
        size_t offset = EXTENTS_OFFSET;
        uint32_t index = 1u;

        while (index <= extents.NumberOfDiskExtents) {
            trace::kernel::DISK_EXTENT extent;

            if (!trace::details::payload::ReadValue(payload, extent, offset)) break;

            result += std::format(L"{}: DiskNumber: {}, StartingOffset: {}, ExtentLength: {}, ", index, extent.DiskNumber, extent.StartingOffset, extent.ExtentLength);
            offset += sizeof(extent);
            index++;
        }

        const bool marked = index <= extents.NumberOfDiskExtents;

        if (!marked) {
            result.resize(result.size() - 2u);
        }

        return text::MarkTruncated(result, marked);
    }


    std::wstring RenderOutput(uint32_t ioControlCode, const protocol::DeviceIoControlSupplement& supplement) {
        const std::span<const uint8_t> output = ExtractOutput(supplement);
        const bool truncated = supplement.captured & protocol::DEVICE_IO_CONTROL_TRUNCATED_OUTPUT;

        switch (ioControlCode) {

            case IOCTL_DISK_GET_DRIVE_GEOMETRY: {
                trace::kernel::DISK_GEOMETRY geometry;

                if (trace::details::payload::ReadValue(output, geometry)) return RenderGeometryPayload(geometry);

                break;
            }

            case IOCTL_DISK_GET_DRIVE_GEOMETRY_EX: {
                constexpr size_t DATA_OFFSET = offsetof(trace::kernel::DISK_GEOMETRY_EX, Data);
                trace::kernel::DISK_GEOMETRY_EX geometryEx;

                if (trace::details::payload::ReadHeader(output, geometryEx, DATA_OFFSET)) return RenderGeometryExPayload(geometryEx);

                break;
            }

            case IOCTL_DISK_GET_LENGTH_INFO: {
                trace::kernel::GET_LENGTH_INFORMATION length;

                if (trace::details::payload::ReadValue(output, length)) return RenderLengthPayload(length);

                break;
            }

            case IOCTL_DISK_GET_PARTITION_INFO_EX: {
                trace::kernel::PARTITION_INFORMATION_EX partition;

                if (trace::details::payload::ReadValue(output, partition)) return RenderPartitionExPayload(partition);

                break;
            }

            case IOCTL_DISK_GET_DISK_ATTRIBUTES: {
                trace::kernel::GET_DISK_ATTRIBUTES diskAttributes;

                if (trace::details::payload::ReadValue(output, diskAttributes)) return RenderDiskAttributesPayload(diskAttributes);

                break;
            }

            case IOCTL_STORAGE_GET_HOTPLUG_INFO: {
                trace::kernel::STORAGE_HOTPLUG_INFO hotplug;

                if (trace::details::payload::ReadValue(output, hotplug)) return RenderHotplugPayload(hotplug);

                break;
            }

            case IOCTL_STORAGE_GET_DEVICE_NUMBER: {
                trace::kernel::STORAGE_DEVICE_NUMBER deviceNumber;

                if (trace::details::payload::ReadValue(output, deviceNumber)) return RenderDeviceNumberPayload(deviceNumber);

                break;
            }

            case IOCTL_STORAGE_GET_DEVICE_NUMBER_EX: {
                trace::kernel::STORAGE_DEVICE_NUMBER_EX deviceNumberEx;

                if (trace::details::payload::ReadValue(output, deviceNumberEx)) return RenderDeviceNumberExPayload(deviceNumberEx);

                break;
            }

            case IOCTL_STORAGE_QUERY_PROPERTY: {
                trace::kernel::STORAGE_DESCRIPTOR_HEADER descriptorHeader;

                if (trace::details::payload::ReadValue(output, descriptorHeader)) return RenderDescriptorHeaderPayload(descriptorHeader);

                break;
            }

            case IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS:

                return RenderDiskExtentsPayload(output);

            case IOCTL_VOLUME_IS_DYNAMIC: {
                uint8_t dynamic;

                if (trace::details::payload::ReadValue(output, dynamic)) return std::format(L"IsDynamic: {}", trace::values::RenderBoolean(dynamic));

                break;
            }

            case IOCTL_VOLUME_IS_CSV: {
                uint8_t csv;

                if (trace::details::payload::ReadValue(output, csv)) return std::format(L"IsCsv: {}", trace::values::RenderBoolean(csv));

                break;
            }

            case trace::kernel::IOCTL_MOUNTDEV_QUERY_DEVICE_NAME:

                return RenderDeviceNamePayload(output);

            default:

                if (!output.empty()) return std::format(L"OutputBuffer: {}", trace::values::RenderBytes(output, truncated));

                break;

        }

        return {};
    }

}

namespace mimo {

    namespace trace {

        namespace details {

            namespace deviceio {

                std::wstring Render(const protocol::RecordData& data) {
                    const protocol::FltParameters& parameters = data.parameters;
                    std::wstring details = std::format(L"Control: {}, InputBufferLength: {}, OutputBufferLength: {}", names::RenderIoControlCode(parameters.deviceIoControl.ioControlCode), parameters.deviceIoControl.inputBufferLength, parameters.deviceIoControl.outputBufferLength);
                    const std::wstring flags = names::RenderDeviceIoFlags(data.operationFlags);

                    if (!flags.empty()) {
                        details += L", ";
                        details += flags;
                    }

                    const protocol::DeviceIoControlSupplement& deviceIoControlSupplement = data.supplement.deviceIoControl;
                    const std::wstring inputText = RenderInput(parameters.deviceIoControl.ioControlCode, deviceIoControlSupplement);

                    if (!inputText.empty()) {
                        details += L", ";
                        details += inputText;
                    }

                    const std::wstring outputText = RenderOutput(parameters.deviceIoControl.ioControlCode, deviceIoControlSupplement);

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
