#include "deviceio.h"

#include "..\buffer.h"
#include "..\report.h"

#include "..\..\error.h"
#include "..\..\handle.h"

#include "..\..\..\..\inc\kernel.h"

#include <Windows.h>
#undef WIN32_NO_STATUS
#include <ntstatus.h>
#include <winioctl.h>
#include <ntddvol.h>    // after winioctl.h, whose volume section it repeats under an IOCTL_VOLUME_BASE guard
#include <winternl.h>

#include <array>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>

using namespace mimo;

namespace {

    struct Operation {
        test::report::Description description;
        uint32_t ioControlCode;
    };

    constexpr Operation OPERATIONS[]{
        { { L"deviceio-get-drive-geometry",      STATUS_SUCCESS }, IOCTL_DISK_GET_DRIVE_GEOMETRY            },
        { { L"deviceio-get-drive-geometry-ex",   STATUS_SUCCESS }, IOCTL_DISK_GET_DRIVE_GEOMETRY_EX         },
        { { L"deviceio-get-partition-info-ex",   STATUS_SUCCESS }, IOCTL_DISK_GET_PARTITION_INFO_EX         },
        { { L"deviceio-get-disk-attributes",     STATUS_SUCCESS }, IOCTL_DISK_GET_DISK_ATTRIBUTES           },
        { { L"deviceio-get-hotplug-info",        STATUS_SUCCESS }, IOCTL_STORAGE_GET_HOTPLUG_INFO           },
        { { L"deviceio-get-device-number",       STATUS_SUCCESS }, IOCTL_STORAGE_GET_DEVICE_NUMBER          },
        { { L"deviceio-get-device-number-ex",    STATUS_SUCCESS }, IOCTL_STORAGE_GET_DEVICE_NUMBER_EX       },
        { { L"deviceio-query-property",          STATUS_SUCCESS }, IOCTL_STORAGE_QUERY_PROPERTY             },
        { { L"deviceio-get-volume-disk-extents", STATUS_SUCCESS }, IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS     },
        { { L"deviceio-is-dynamic",              STATUS_SUCCESS }, IOCTL_VOLUME_IS_DYNAMIC                  },
        { { L"deviceio-is-csv",                  STATUS_SUCCESS }, IOCTL_VOLUME_IS_CSV                      },
        { { L"deviceio-query-device-name",       STATUS_SUCCESS }, kernel::IOCTL_MOUNTDEV_QUERY_DEVICE_NAME },
    };

    // the operations are sent on a volume handle because NTFS answers these device control codes on a file handle with STATUS_INVALID_PARAMETER
    NTSTATUS OpenVolume(std::wstring_view ntDirectory, NtHandle& volume) {
        constexpr std::wstring_view NT_PREFIX = L"\\??\\";
        // a volume open that asks for no more than FILE_READ_ATTRIBUTES bypasses the file system
        // FILE_EXECUTE is the access a caller without elevation gets that does not
        constexpr ACCESS_MASK DESIRED_ACCESS = FILE_EXECUTE | SYNCHRONIZE;
        constexpr ULONG SHARE_ALL = FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE;
        std::wstring path{ ntDirectory.substr(0u, ntDirectory.find(L'\\', NT_PREFIX.size())) };
        UNICODE_STRING objectName{};
        objectName.Length = static_cast<USHORT>(path.size() * sizeof(wchar_t));
        objectName.MaximumLength = objectName.Length;
        objectName.Buffer = path.data();

        OBJECT_ATTRIBUTES objectAttributes{};
        InitializeObjectAttributes(&objectAttributes, &objectName, OBJ_CASE_INSENSITIVE, nullptr, nullptr);

        IO_STATUS_BLOCK ioStatus{};

        return NtCreateFile(volume.Put(), DESIRED_ACCESS, &objectAttributes, &ioStatus, nullptr, FILE_ATTRIBUTE_NORMAL, SHARE_ALL, FILE_OPEN, FILE_SYNCHRONOUS_IO_NONALERT, nullptr, 0u);
    }


    std::optional<std::vector<std::byte>> MakeInputBuffer(uint32_t ioControlCode) {

        switch (ioControlCode) {

            case IOCTL_DISK_GET_DRIVE_GEOMETRY:
            case IOCTL_DISK_GET_DRIVE_GEOMETRY_EX:
            case IOCTL_DISK_GET_PARTITION_INFO_EX:
            case IOCTL_DISK_GET_DISK_ATTRIBUTES:
            case IOCTL_STORAGE_GET_HOTPLUG_INFO:
            case IOCTL_STORAGE_GET_DEVICE_NUMBER:
            case IOCTL_STORAGE_GET_DEVICE_NUMBER_EX:
            case IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS:
            case IOCTL_VOLUME_IS_DYNAMIC:
            case IOCTL_VOLUME_IS_CSV:
            case kernel::IOCTL_MOUNTDEV_QUERY_DEVICE_NAME:

                return std::vector<std::byte>{};

            case IOCTL_STORAGE_QUERY_PROPERTY: {
                STORAGE_PROPERTY_QUERY query{};
                query.PropertyId = StorageDeviceProperty;
                query.QueryType = PropertyStandardQuery;

                return test::MakeBuffer(query);
            }

        }

        return std::nullopt;
    }


    NTSTATUS Control(
        HANDLE hVolume,
        uint32_t ioControlCode,
        std::span<const std::byte> inputData
    ) {
        constexpr uint32_t OUTPUT_BUFFER_SIZE = 1024u;
        std::vector<std::byte> inputBuffer(inputData.begin(), inputData.end());
        std::array<std::byte, OUTPUT_BUFFER_SIZE> outputBuffer{};
        IO_STATUS_BLOCK ioStatus{};

        return NtDeviceIoControlFile(hVolume, nullptr, nullptr, nullptr, &ioStatus, static_cast<ULONG>(ioControlCode), inputBuffer.data(), static_cast<ULONG>(inputBuffer.size()), outputBuffer.data(), static_cast<ULONG>(outputBuffer.size()));
    }


    constexpr std::string_view MAJOR = "Device control";

}

namespace mimo {

    namespace test {

        namespace operations {

            namespace deviceio {

                void Run(std::wstring_view ntDirectory, report::Counts& counts) {

                    for (const Operation& operation : OPERATIONS) {
                        NtHandle volume;
                        const NTSTATUS openStatus = OpenVolume(ntDirectory, volume);

                        if (!NT_SUCCESS(openStatus)) {
                            std::cerr << "Failed to open the volume\n";
                            error::Display(RtlNtStatusToDosError(openStatus));
                            report::Add(counts, MAJOR, operation.description, std::nullopt);

                            continue;
                        }

                        const std::optional<std::vector<std::byte>> inputData = MakeInputBuffer(operation.ioControlCode);

                        if (!inputData.has_value()) {
                            std::cerr << "No payload for the control code\n";
                            report::Add(counts, MAJOR, operation.description, std::nullopt);

                            continue;
                        }

                        const NTSTATUS controlStatus = Control(volume.Get(), operation.ioControlCode, *inputData);
                        report::Add(counts, MAJOR, operation.description, controlStatus);
                    }

                    return;
                }

            }

        }

    }

}
