#include "deviceio.h"

#include "payload.h"

#include "..\kernel.h"
#include "..\names.h"

#include "..\..\text.h"

#include "..\..\..\..\inc\protocol.h"

#include <Windows.h>
#include <winioctl.h>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <format>
#include <span>
#include <string>

using namespace mimo;

namespace {

    std::span<const uint8_t> ExtractInput(const protocol::DeviceIoControlSupplement& supplement) {

        if (!(supplement.captured & protocol::DEVICE_IO_CONTROL_CAPTURED_INPUT)) return {};

        return { supplement.inputPayload, supplement.capturedInputBytes };
    }


    std::wstring RenderPropertyQueryPayload(const trace::kernel::STORAGE_PROPERTY_QUERY& payload) {

        return std::format(L"PropertyId: {}, QueryType: {}", trace::names::RenderStoragePropertyId(payload.PropertyId), trace::names::RenderStorageQueryType(payload.QueryType));
    }


    std::wstring RenderInput(uint32_t ioControlCode, std::span<const uint8_t> input) {

        switch (ioControlCode) {

            case IOCTL_STORAGE_QUERY_PROPERTY: {
                constexpr size_t PARAMETERS_OFFSET = offsetof(trace::kernel::STORAGE_PROPERTY_QUERY, AdditionalParameters);
                trace::kernel::STORAGE_PROPERTY_QUERY propertyQuery;

                if (trace::details::payload::ReadHeader(input, propertyQuery, PARAMETERS_OFFSET)) return RenderPropertyQueryPayload(propertyQuery);

                break;
            }

        }

        return {};
    }


    std::span<const uint8_t> ExtractOutput(const protocol::DeviceIoControlSupplement& supplement) {

        if (!(supplement.captured & protocol::DEVICE_IO_CONTROL_CAPTURED_OUTPUT)) return {};

        return { supplement.outputPayload, supplement.capturedOutputBytes };
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


    std::wstring RenderDeviceNumberPayload(const trace::kernel::STORAGE_DEVICE_NUMBER& payload) {

        return std::format(L"DeviceType: {}, DeviceNumber: {}, PartitionNumber: {}", trace::names::RenderDeviceType(payload.DeviceType), payload.DeviceNumber, payload.PartitionNumber);
    }


    std::wstring RenderDescriptorHeaderPayload(const trace::kernel::STORAGE_DESCRIPTOR_HEADER& payload) {

        return std::format(L"Version: {}, Size: {}", payload.Version, payload.Size);
    }


    std::wstring RenderDiskExtentsPayload(std::span<const uint8_t> payload) {
        constexpr size_t EXTENTS_OFFSET = offsetof(trace::kernel::VOLUME_DISK_EXTENTS, Extents);
        trace::kernel::VOLUME_DISK_EXTENTS extents;

        if (!trace::details::payload::ReadHeader(payload, extents, EXTENTS_OFFSET)) return {};

        std::wstring result = std::format(L"NumberOfDiskExtents: {}", extents.NumberOfDiskExtents);
        size_t offset = EXTENTS_OFFSET;
        uint32_t index = 1u;

        while (index <= extents.NumberOfDiskExtents) {
            trace::kernel::DISK_EXTENT extent;

            if (!trace::details::payload::ReadValue(payload, extent, offset)) break;

            result += std::format(L", {}: DiskNumber: {}, StartingOffset: {}, ExtentLength: {}", index, extent.DiskNumber, extent.StartingOffset, extent.ExtentLength);
            offset += sizeof(extent);
            index++;
        }

        return result;
    }


    std::wstring RenderOutput(uint32_t ioControlCode, std::span<const uint8_t> output) {

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

            case IOCTL_STORAGE_GET_DEVICE_NUMBER: {
                trace::kernel::STORAGE_DEVICE_NUMBER deviceNumber;

                if (trace::details::payload::ReadValue(output, deviceNumber)) return RenderDeviceNumberPayload(deviceNumber);

                break;
            }

            case IOCTL_STORAGE_QUERY_PROPERTY: {
                trace::kernel::STORAGE_DESCRIPTOR_HEADER descriptorHeader;

                if (trace::details::payload::ReadValue(output, descriptorHeader)) return RenderDescriptorHeaderPayload(descriptorHeader);

                break;
            }

            case IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS:

                return RenderDiskExtentsPayload(output);

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
                    const protocol::DeviceIoControlSupplement& deviceIoControlSupplement = data.supplement.deviceIoControl;
                    const std::wstring inputText = RenderInput(parameters.deviceIoControl.ioControlCode, ExtractInput(deviceIoControlSupplement));

                    if (!inputText.empty()) {
                        details += L", ";
                        details += text::MarkTruncated(inputText, deviceIoControlSupplement.capturedInputBytes < parameters.deviceIoControl.inputBufferLength);
                    }

                    const std::wstring outputText = RenderOutput(parameters.deviceIoControl.ioControlCode, ExtractOutput(deviceIoControlSupplement));

                    if (!outputText.empty()) {
                        const uint64_t writtenBytes = std::min<uint64_t>(data.information, parameters.deviceIoControl.outputBufferLength);

                        details += L", ";
                        details += text::MarkTruncated(outputText, deviceIoControlSupplement.capturedOutputBytes < writtenBytes);
                    }

                    return details;
                }

            }

        }

    }

}
