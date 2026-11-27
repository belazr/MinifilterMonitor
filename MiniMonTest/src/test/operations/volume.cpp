#include "volume.h"

#include "..\report.h"
#include "..\target.h"

#include "..\..\error.h"

#include "..\..\..\..\inc\kernel.h"

#include <Windows.h>
#undef WIN32_NO_STATUS
#include <ntstatus.h>
#include <winternl.h>

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <optional>
#include <string_view>
#include <vector>

extern "C" NTSYSAPI NTSTATUS NTAPI NtQueryVolumeInformationFile(
    HANDLE hFile,
    IO_STATUS_BLOCK* pIoStatus,
    void* pBuffer,
    ULONG bufferSize,
    ULONG infoClass
);

extern "C" NTSYSAPI NTSTATUS NTAPI NtSetVolumeInformationFile(
    HANDLE hFile,
    IO_STATUS_BLOCK* pIoStatus,
    void* pBuffer,
    ULONG bufferSize,
    ULONG infoClass
);

using namespace mimo;

namespace {

    struct QueryOperation {
        test::report::Description description;
        uint32_t infoClass;
    };

    constexpr QueryOperation QUERY_OPERATIONS[]{
        { { L"query-volume-info-volume",       STATUS_SUCCESS }, kernel::FileFsVolumeInformation     },
        { { L"query-volume-info-size",         STATUS_SUCCESS }, kernel::FileFsSizeInformation       },
        { { L"query-volume-info-attribute",    STATUS_SUCCESS }, kernel::FileFsAttributeInformation  },
        { { L"query-volume-info-control",      STATUS_SUCCESS }, kernel::FileFsControlInformation    },
        { { L"query-volume-info-full-size",    STATUS_SUCCESS }, kernel::FileFsFullSizeInformation   },
        { { L"query-volume-info-object-id",    STATUS_SUCCESS }, kernel::FileFsObjectIdInformation   },
        { { L"query-volume-info-sector-size",  STATUS_SUCCESS }, kernel::FileFsSectorSizeInformation },
        { { L"query-volume-info-data-copy",    STATUS_SUCCESS }, kernel::FileFsDataCopyInformation   },
        { { L"query-volume-info-full-size-ex", STATUS_SUCCESS }, kernel::FileFsFullSizeInformationEx },
    };

    NTSTATUS Query(const test::Target& target, uint32_t infoClass) {
        constexpr uint32_t BUFFER_SIZE = 1024u;
        std::array<std::byte, BUFFER_SIZE> buffer{};
        IO_STATUS_BLOCK ioStatus{};

        return NtQueryVolumeInformationFile(target.Get(), &ioStatus, buffer.data(), static_cast<ULONG>(buffer.size()), static_cast<ULONG>(infoClass));
    }


    constexpr std::string_view QUERY_MAJOR = "Query volume information";

    void RunQueryOperations(std::wstring_view ntDirectory, test::report::Counts& counts) {

        for (const QueryOperation& operation : QUERY_OPERATIONS) {
            test::Target target{ ntDirectory, operation.description.name };
            const NTSTATUS createStatus = target.Create(FILE_GENERIC_READ);

            if (!NT_SUCCESS(createStatus)) {
                std::cerr << "Failed to create the target file\n";
                error::Display(RtlNtStatusToDosError(createStatus));
                test::report::Add(counts, QUERY_MAJOR, operation.description, std::nullopt);

                continue;
            }

            const NTSTATUS queryStatus = Query(target, operation.infoClass);
            test::report::Add(counts, QUERY_MAJOR, operation.description, queryStatus);
        }

        return;
    }


    // to make no persistent changes on the machine
    // sent on a file handle so the file system refuses it and the volume label never changes
    constexpr test::report::Description SET_DESCRIPTIONS[]{
        { L"set-volume-info-label", STATUS_ACCESS_DENIED },
    };

    struct FILE_FS_LABEL_INFORMATION {
        ULONG VolumeLabelLength;
        WCHAR VolumeLabel[1u];
    };

    std::vector<std::byte> MakeLabelBuffer() {
        constexpr std::wstring_view LABEL = L"MINIMON";
        constexpr size_t LABEL_OFFSET = offsetof(FILE_FS_LABEL_INFORMATION, VolumeLabel);
        constexpr size_t LABEL_SIZE = LABEL.size() * sizeof(wchar_t);
        FILE_FS_LABEL_INFORMATION header{};
        header.VolumeLabelLength = static_cast<ULONG>(LABEL_SIZE);

        std::vector<std::byte> buffer(LABEL_OFFSET + LABEL_SIZE);
        std::memcpy(buffer.data(), &header, LABEL_OFFSET);
        std::memcpy(buffer.data() + LABEL_OFFSET, LABEL.data(), LABEL_SIZE);

        return buffer;
    }


    NTSTATUS SetLabel(const test::Target& target) {
        std::vector<std::byte> buffer = MakeLabelBuffer();
        IO_STATUS_BLOCK ioStatus{};

        return NtSetVolumeInformationFile(target.Get(), &ioStatus, buffer.data(), static_cast<ULONG>(buffer.size()), static_cast<ULONG>(kernel::FileFsLabelInformation));
    }


    constexpr std::string_view SET_MAJOR = "Set volume information";

    void RunSetOperations(std::wstring_view ntDirectory, test::report::Counts& counts) {

        for (const test::report::Description& description : SET_DESCRIPTIONS) {
            test::Target target{ ntDirectory, description.name };
            const NTSTATUS createStatus = target.Create(FILE_WRITE_DATA);

            if (!NT_SUCCESS(createStatus)) {
                std::cerr << "Failed to create the target file\n";
                error::Display(RtlNtStatusToDosError(createStatus));
                test::report::Add(counts, SET_MAJOR, description, std::nullopt);

                continue;
            }

            const NTSTATUS setStatus = SetLabel(target);
            test::report::Add(counts, SET_MAJOR, description, setStatus);
        }

        return;
    }

}

namespace mimo {

    namespace test {

        namespace operations {

            namespace volume {

                bool Run(std::wstring_view ntDirectory, report::Counts& counts) {
                    RunQueryOperations(ntDirectory, counts);
                    RunSetOperations(ntDirectory, counts);

                    return true;
                }

            }

        }

    }

}
