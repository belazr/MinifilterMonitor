#include "filesystem.h"

#include "..\report.h"
#include "..\target.h"

#include "..\..\error.h"

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
        test::report::Description description;
        uint32_t fsControlCode;
    };

    constexpr Operation OPERATIONS[]{
        { { L"filesystem-get-compression",         STATUS_SUCCESS }, FSCTL_GET_COMPRESSION         },
        { { L"filesystem-set-compression",         STATUS_SUCCESS }, FSCTL_SET_COMPRESSION         },
        { { L"filesystem-get-ntfs-volume-data",    STATUS_SUCCESS }, FSCTL_GET_NTFS_VOLUME_DATA    },
        { { L"filesystem-create-or-get-object-id", STATUS_SUCCESS }, FSCTL_CREATE_OR_GET_OBJECT_ID },
        { { L"filesystem-set-sparse",              STATUS_SUCCESS }, FSCTL_SET_SPARSE              },
        { { L"filesystem-set-zero-data",           STATUS_SUCCESS }, FSCTL_SET_ZERO_DATA           },
        { { L"filesystem-query-allocated-ranges",  STATUS_SUCCESS }, FSCTL_QUERY_ALLOCATED_RANGES  },
    };

    template <typename Input>
    std::vector<std::byte> MakeBuffer(const Input& payload) {
        std::vector<std::byte> buffer(sizeof(payload));
        std::memcpy(buffer.data(), &payload, sizeof(payload));

        return buffer;
    }


    std::optional<std::vector<std::byte>> MakeInputBuffer(uint32_t fsControlCode) {

        switch (fsControlCode) {

            case FSCTL_GET_COMPRESSION:
            case FSCTL_GET_NTFS_VOLUME_DATA:
            case FSCTL_CREATE_OR_GET_OBJECT_ID:

                return std::vector<std::byte>{};

            case FSCTL_SET_COMPRESSION: {
                constexpr USHORT COMPRESSION_FORMAT = COMPRESSION_FORMAT_DEFAULT;

                return MakeBuffer(COMPRESSION_FORMAT);
            }

            case FSCTL_SET_SPARSE: {
                FILE_SET_SPARSE_BUFFER sparse{};
                sparse.SetSparse = TRUE;

                return MakeBuffer(sparse);
            }

            case FSCTL_SET_ZERO_DATA: {
                constexpr LONGLONG BEYOND_FINAL_ZERO = 16;
                FILE_ZERO_DATA_INFORMATION zeroData{};
                zeroData.BeyondFinalZero.QuadPart = BEYOND_FINAL_ZERO;

                return MakeBuffer(zeroData);
            }

            case FSCTL_QUERY_ALLOCATED_RANGES: {
                constexpr LONGLONG RANGE_SIZE = 16;
                FILE_ALLOCATED_RANGE_BUFFER allocatedRange{};
                allocatedRange.Length.QuadPart = RANGE_SIZE;

                return MakeBuffer(allocatedRange);
            }

        }

        return std::nullopt;
    }


    NTSTATUS Control(
        const test::Target& target,
        uint32_t fsControlCode,
        std::span<const std::byte> inputData
    ) {
        constexpr size_t OUTPUT_BUFFER_SIZE = 1024u;
        std::vector<std::byte> inputBuffer(inputData.begin(), inputData.end());
        std::vector<std::byte> outputBuffer(OUTPUT_BUFFER_SIZE);
        IO_STATUS_BLOCK ioStatus{};

        return NtFsControlFile(target.Get(), nullptr, nullptr, nullptr, &ioStatus, static_cast<ULONG>(fsControlCode), inputBuffer.data(), static_cast<ULONG>(inputBuffer.size()), outputBuffer.data(), static_cast<ULONG>(outputBuffer.size()));
    }


    constexpr std::string_view MAJOR = "File system control";

}

namespace mimo {

    namespace test {

        namespace operations {

            namespace filesystem {

                bool Run(std::wstring_view ntDirectory, report::Counts& counts) {

                    for (const Operation& operation : OPERATIONS) {
                        Target target{ ntDirectory, operation.description.name };
                        const NTSTATUS createStatus = target.Create(FILE_GENERIC_READ | FILE_GENERIC_WRITE);

                        if (!NT_SUCCESS(createStatus)) {
                            std::cerr << "Failed to create the target file\n";
                            error::Display(RtlNtStatusToDosError(createStatus));
                            report::Add(counts, MAJOR, operation.description, std::nullopt);

                            continue;
                        }

                        const std::optional<std::vector<std::byte>> inputData = MakeInputBuffer(operation.fsControlCode);

                        if (!inputData.has_value()) {
                            std::cerr << "No payload for the control code\n";
                            report::Add(counts, MAJOR, operation.description, std::nullopt);

                            continue;
                        }

                        const NTSTATUS controlStatus = Control(target, operation.fsControlCode, *inputData);
                        report::Add(counts, MAJOR, operation.description, controlStatus);
                    }

                    return true;
                }

            }

        }

    }

}
