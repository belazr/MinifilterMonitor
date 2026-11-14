#include "readwrite.h"

#include "..\report.h"
#include "..\target.h"

#include "..\..\error.h"

#include <Windows.h>
#undef WIN32_NO_STATUS
#include <ntstatus.h>
#include <winternl.h>

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <optional>
#include <span>
#include <string_view>
#include <vector>

extern "C" NTSYSAPI NTSTATUS NTAPI NtWriteFile(
    HANDLE hFile,
    HANDLE hEvent,
    PIO_APC_ROUTINE pApcRoutine,
    void* pApcContext,
    IO_STATUS_BLOCK* pIoStatus,
    void* pBuffer,
    ULONG bufferSize,
    LARGE_INTEGER* pByteOffset,
    ULONG* pKey
);

extern "C" NTSYSAPI NTSTATUS NTAPI NtReadFile(
    HANDLE hFile,
    HANDLE hEvent,
    PIO_APC_ROUTINE pApcRoutine,
    void* pApcContext,
    IO_STATUS_BLOCK* pIoStatus,
    void* pBuffer,
    ULONG bufferSize,
    LARGE_INTEGER* pByteOffset,
    ULONG* pKey
);

using namespace mimo;

namespace {

    struct ReadOperation {
        test::report::Description description;
        uint32_t fileSize;
        uint32_t size;
        uint32_t key;
    };

    constexpr ReadOperation READ_OPERATIONS[]{
        { { L"read",             STATUS_SUCCESS     }, 16u, 16u, 0u      },
        { { L"read-oversized",   STATUS_SUCCESS     }, 64u, 64u, 0u      },
        { { L"read-short",       STATUS_SUCCESS     }, 16u, 64u, 0u      },
        { { L"read-end-of-file", STATUS_END_OF_FILE }, 0u,  16u, 0u      },
        { { L"read-key",         STATUS_SUCCESS     }, 16u, 16u, 0x4D4Du },
    };

    constexpr std::byte FILL{ 0x4D };

    NTSTATUS Write(const test::Target& target, std::span<const std::byte> data) {
        LARGE_INTEGER byteOffset{};
        IO_STATUS_BLOCK ioStatus{};

        return NtWriteFile(target.Get(), nullptr, nullptr, nullptr, &ioStatus, const_cast<std::byte*>(data.data()), static_cast<ULONG>(data.size()), &byteOffset, nullptr);
    }


    NTSTATUS Read(
        const test::Target& target,
        std::span<std::byte> data,
        ULONG* pKey
    ) {
        LARGE_INTEGER byteOffset{};
        IO_STATUS_BLOCK ioStatus{};

        return NtReadFile(target.Get(), nullptr, nullptr, nullptr, &ioStatus, data.data(), static_cast<ULONG>(data.size()), &byteOffset, pKey);
    }


    constexpr std::string_view READ_MAJOR = "Read";

    void RunReadOperations(std::wstring_view ntDirectory, test::report::Counts& counts) {

        for (const ReadOperation& operation : READ_OPERATIONS) {
            test::Target target{ ntDirectory, operation.description.name };
            const NTSTATUS createStatus = target.Create(FILE_READ_DATA | FILE_WRITE_DATA);

            if (!NT_SUCCESS(createStatus)) {
                std::cerr << "Failed to create the target file\n";
                error::Display(RtlNtStatusToDosError(createStatus));
                test::report::Add(counts, READ_MAJOR, operation.description, std::nullopt);

                continue;
            }

            const std::vector<std::byte> fileData(operation.fileSize, FILL);
            const NTSTATUS writeStatus = Write(target, fileData);

            if (!NT_SUCCESS(writeStatus)) {
                std::cerr << "Failed to write the target file\n";
                error::Display(RtlNtStatusToDosError(writeStatus));
                test::report::Add(counts, READ_MAJOR, operation.description, std::nullopt);

                continue;
            }

            std::vector<std::byte> data(operation.size);
            ULONG key = static_cast<ULONG>(operation.key);
            const NTSTATUS readStatus = Read(target, data, operation.key ? &key : nullptr);
            test::report::Add(counts, READ_MAJOR, operation.description, readStatus);
        }

        return;
    }


    struct WriteOperation {
        test::report::Description description;
        uint32_t size;
    };

    constexpr WriteOperation WRITE_OPERATIONS[]{
        { { L"write",           STATUS_SUCCESS }, 16u },
        { { L"write-oversized", STATUS_SUCCESS }, 64u },
    };

    constexpr std::string_view WRITE_MAJOR = "Write";

    void RunWriteOperations(std::wstring_view ntDirectory, test::report::Counts& counts) {

        for (const WriteOperation& operation : WRITE_OPERATIONS) {
            test::Target target{ ntDirectory, operation.description.name };
            const NTSTATUS createStatus = target.Create(FILE_WRITE_DATA);

            if (!NT_SUCCESS(createStatus)) {
                std::cerr << "Failed to create the target file\n";
                error::Display(RtlNtStatusToDosError(createStatus));
                test::report::Add(counts, WRITE_MAJOR, operation.description, std::nullopt);

                continue;
            }

            const std::vector<std::byte> data(operation.size, FILL);
            const NTSTATUS writeStatus = Write(target, data);
            test::report::Add(counts, WRITE_MAJOR, operation.description, writeStatus);
        }

        return;
    }

}

namespace mimo {

    namespace test {

        namespace operations {

            namespace readwrite {

                bool Run(std::wstring_view ntDirectory, report::Counts& counts) {
                    RunReadOperations(ntDirectory, counts);
                    RunWriteOperations(ntDirectory, counts);

                    return true;
                }

            }

        }

    }

}
