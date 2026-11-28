#include "filesystem.h"

#include "..\buffer.h"
#include "..\report.h"
#include "..\target.h"

#include "..\..\error.h"
#include "..\..\test.h"

#include <Windows.h>
#undef WIN32_NO_STATUS
#include <ntstatus.h>
#include <winioctl.h>
#include <winternl.h>

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <optional>
#include <span>
#include <string_view>
#include <vector>

extern "C" NTSYSAPI NTSTATUS NTAPI NtFsControlFile(
    HANDLE hFile,
    HANDLE hEvent,
    PIO_APC_ROUTINE pApcRoutine,
    void* pApcContext,
    IO_STATUS_BLOCK* pIoStatus,
    ULONG fsControlCode,
    void* pInputBuffer,
    ULONG inputSize,
    void* pOutputBuffer,
    ULONG outputSize
);

using namespace mimo;

namespace {

    struct Operation {
        test::Description description;
        uint32_t fsControlCode;
        uint32_t outputSize;
        bool directory;
    };

    constexpr uint32_t OUTPUT_BUFFER_SIZE = 1024u;

    constexpr std::string_view MAJOR = "File system control";

    constexpr Operation OPERATIONS[]{
        { { MAJOR, L"filesystem-get-compression",         STATUS_SUCCESS }, FSCTL_GET_COMPRESSION,         OUTPUT_BUFFER_SIZE, false },
        { { MAJOR, L"filesystem-set-compression",         STATUS_SUCCESS }, FSCTL_SET_COMPRESSION,         OUTPUT_BUFFER_SIZE, false },
        { { MAJOR, L"filesystem-get-ntfs-volume-data",    STATUS_SUCCESS }, FSCTL_GET_NTFS_VOLUME_DATA,    OUTPUT_BUFFER_SIZE, false },
        // the output size is 0 because NTFS answers FSCTL_SET_REPARSE_POINT with STATUS_INVALID_PARAMETER when an output buffer is passed
        { { MAJOR, L"filesystem-set-reparse-point",       STATUS_SUCCESS }, FSCTL_SET_REPARSE_POINT,       0u,                 true  },
        { { MAJOR, L"filesystem-get-reparse-point",       STATUS_SUCCESS }, FSCTL_GET_REPARSE_POINT,       OUTPUT_BUFFER_SIZE, true  },
        // the output size is 0 because NTFS answers FSCTL_DELETE_REPARSE_POINT with STATUS_INVALID_PARAMETER when an output buffer is passed
        { { MAJOR, L"filesystem-delete-reparse-point",    STATUS_SUCCESS }, FSCTL_DELETE_REPARSE_POINT,    0u,                 true  },
        { { MAJOR, L"filesystem-create-or-get-object-id", STATUS_SUCCESS }, FSCTL_CREATE_OR_GET_OBJECT_ID, OUTPUT_BUFFER_SIZE, false },
        { { MAJOR, L"filesystem-set-sparse",              STATUS_SUCCESS }, FSCTL_SET_SPARSE,              OUTPUT_BUFFER_SIZE, false },
        { { MAJOR, L"filesystem-set-zero-data",           STATUS_SUCCESS }, FSCTL_SET_ZERO_DATA,           OUTPUT_BUFFER_SIZE, false },
        { { MAJOR, L"filesystem-query-allocated-ranges",  STATUS_SUCCESS }, FSCTL_QUERY_ALLOCATED_RANGES,  OUTPUT_BUFFER_SIZE, false },
    };

    struct REPARSE_DATA_BUFFER {
        ULONG ReparseTag;
        USHORT ReparseDataLength;
        USHORT Reserved;

        struct {
            USHORT SubstituteNameOffset;
            USHORT SubstituteNameLength;
            USHORT PrintNameOffset;
            USHORT PrintNameLength;
            WCHAR PathBuffer[1u];
        } MountPointReparseBuffer;
    };

    static_assert(offsetof(REPARSE_DATA_BUFFER, MountPointReparseBuffer.PathBuffer) == 16u, "REPARSE_DATA_BUFFER layout drift");

    std::vector<std::byte> MakeMountPointBuffer() {
        constexpr std::wstring_view SUBSTITUTE_NAME = L"\\??\\C:\\MiniMonTest.missing";
        constexpr std::wstring_view PRINT_NAME = L"C:\\MiniMonTest.missing";
        constexpr size_t DATA_OFFSET = offsetof(REPARSE_DATA_BUFFER, MountPointReparseBuffer);
        constexpr size_t NAME_OFFSET = offsetof(REPARSE_DATA_BUFFER, MountPointReparseBuffer.PathBuffer);
        constexpr size_t SUBSTITUTE_NAME_SIZE = SUBSTITUTE_NAME.size() * sizeof(wchar_t);
        constexpr size_t PRINT_NAME_OFFSET = SUBSTITUTE_NAME_SIZE + sizeof(wchar_t);
        constexpr size_t PRINT_NAME_SIZE = PRINT_NAME.size() * sizeof(wchar_t);
        constexpr size_t NAMES_SIZE = PRINT_NAME_OFFSET + PRINT_NAME_SIZE + sizeof(wchar_t);
        REPARSE_DATA_BUFFER header{};
        header.ReparseTag = IO_REPARSE_TAG_MOUNT_POINT;
        header.ReparseDataLength = static_cast<USHORT>(NAME_OFFSET - DATA_OFFSET + NAMES_SIZE);
        header.MountPointReparseBuffer.SubstituteNameLength = static_cast<USHORT>(SUBSTITUTE_NAME_SIZE);
        header.MountPointReparseBuffer.PrintNameOffset = static_cast<USHORT>(PRINT_NAME_OFFSET);
        header.MountPointReparseBuffer.PrintNameLength = static_cast<USHORT>(PRINT_NAME_SIZE);

        std::vector<std::byte> buffer(NAME_OFFSET + NAMES_SIZE);
        std::memcpy(buffer.data(), &header, NAME_OFFSET);
        std::memcpy(buffer.data() + NAME_OFFSET, SUBSTITUTE_NAME.data(), SUBSTITUTE_NAME_SIZE);
        std::memcpy(buffer.data() + NAME_OFFSET + PRINT_NAME_OFFSET, PRINT_NAME.data(), PRINT_NAME_SIZE);

        return buffer;
    }


    NTSTATUS Control(
        const test::Target& target,
        uint32_t fsControlCode,
        std::span<const std::byte> inputData,
        uint32_t outputSize
    ) {
        std::vector<std::byte> inputBuffer(inputData.begin(), inputData.end());
        std::vector<std::byte> outputBuffer(outputSize);
        IO_STATUS_BLOCK ioStatus{};

        return NtFsControlFile(target.Get(), nullptr, nullptr, nullptr, &ioStatus, static_cast<ULONG>(fsControlCode), inputBuffer.data(), static_cast<ULONG>(inputBuffer.size()), outputBuffer.data(), static_cast<ULONG>(outputBuffer.size()));
    }


    NTSTATUS PrepareTarget(const test::Target& target, uint32_t fsControlCode) {

        switch (fsControlCode) {

            case FSCTL_GET_REPARSE_POINT:
            case FSCTL_DELETE_REPARSE_POINT: {
                const std::vector<std::byte> mountPoint = MakeMountPointBuffer();

                return Control(target, FSCTL_SET_REPARSE_POINT, mountPoint, 0u);
            }

        }

        return STATUS_SUCCESS;
    }


    std::vector<std::byte> MakeMountPointTagBuffer() {
        constexpr size_t DATA_OFFSET = offsetof(REPARSE_DATA_BUFFER, MountPointReparseBuffer);
        REPARSE_DATA_BUFFER header{};
        header.ReparseTag = IO_REPARSE_TAG_MOUNT_POINT;

        std::vector<std::byte> buffer(DATA_OFFSET);
        std::memcpy(buffer.data(), &header, DATA_OFFSET);

        return buffer;
    }


    std::optional<std::vector<std::byte>> MakeInputBuffer(uint32_t fsControlCode) {

        switch (fsControlCode) {

            case FSCTL_GET_COMPRESSION:
            case FSCTL_GET_NTFS_VOLUME_DATA:
            case FSCTL_GET_REPARSE_POINT:
            case FSCTL_CREATE_OR_GET_OBJECT_ID:

                return std::vector<std::byte>{};

            case FSCTL_SET_COMPRESSION: {
                constexpr USHORT COMPRESSION_FORMAT = COMPRESSION_FORMAT_DEFAULT;

                return test::MakeBuffer(COMPRESSION_FORMAT);
            }

            case FSCTL_SET_REPARSE_POINT:

                return MakeMountPointBuffer();

            case FSCTL_DELETE_REPARSE_POINT:

                return MakeMountPointTagBuffer();

            case FSCTL_SET_SPARSE: {
                FILE_SET_SPARSE_BUFFER sparse{};
                sparse.SetSparse = TRUE;

                return test::MakeBuffer(sparse);
            }

            case FSCTL_SET_ZERO_DATA: {
                constexpr LONGLONG BEYOND_FINAL_ZERO = 16;
                FILE_ZERO_DATA_INFORMATION zeroData{};
                zeroData.BeyondFinalZero.QuadPart = BEYOND_FINAL_ZERO;

                return test::MakeBuffer(zeroData);
            }

            case FSCTL_QUERY_ALLOCATED_RANGES: {
                constexpr LONGLONG RANGE_SIZE = 16;
                FILE_ALLOCATED_RANGE_BUFFER allocatedRange{};
                allocatedRange.Length.QuadPart = RANGE_SIZE;

                return test::MakeBuffer(allocatedRange);
            }

        }

        return std::nullopt;
    }

}

namespace mimo {

    namespace test {

        namespace operations {

            namespace filesystem {

                void Run(std::wstring_view ntDirectory, Report& report) {

                    for (const Operation& operation : OPERATIONS) {
                        Target target{ ntDirectory, operation.description.name };
                        const NTSTATUS createStatus = target.Create(FILE_GENERIC_READ | FILE_GENERIC_WRITE, false, operation.directory);

                        if (!NT_SUCCESS(createStatus)) {
                            std::cerr << "Failed to create the target\n";
                            error::Display(RtlNtStatusToDosError(createStatus));
                            report.Add(operation.description, std::nullopt);

                            continue;
                        }

                        const NTSTATUS prepareStatus = PrepareTarget(target, operation.fsControlCode);

                        if (!NT_SUCCESS(prepareStatus)) {
                            std::cerr << "Failed to prepare the target\n";
                            error::Display(RtlNtStatusToDosError(prepareStatus));
                            report.Add(operation.description, std::nullopt);

                            continue;
                        }

                        const std::optional<std::vector<std::byte>> inputData = MakeInputBuffer(operation.fsControlCode);

                        if (!inputData.has_value()) {
                            std::cerr << "No payload for the control code\n";
                            report.Add(operation.description, std::nullopt);

                            continue;
                        }

                        const NTSTATUS controlStatus = Control(target, operation.fsControlCode, *inputData, operation.outputSize);
                        report.Add(operation.description, controlStatus);
                    }

                    return;
                }

            }

        }

    }

}
