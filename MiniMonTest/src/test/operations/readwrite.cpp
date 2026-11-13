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

using namespace mimo;

namespace {

    struct Operation {
        test::report::Description description;
        int64_t offset;
        uint32_t size;
    };

    constexpr Operation OPERATIONS[]{
        { { L"write",           STATUS_SUCCESS }, 0,  16u },
        { { L"write-oversized", STATUS_SUCCESS }, 16, 64u },
    };

    bool OpenTarget(test::Target& target) {
        const NTSTATUS status = target.Open(FILE_WRITE_DATA);

        if (!NT_SUCCESS(status)) {
            std::cerr << "Failed to open the target file\n";
            error::Display(RtlNtStatusToDosError(status));

            return false;
        }

        return true;
    }


    NTSTATUS Write(
        const test::Target& target,
        int64_t offset,
        std::span<const std::byte> data
    ) {
        LARGE_INTEGER byteOffset{};
        byteOffset.QuadPart = static_cast<LONGLONG>(offset);

        IO_STATUS_BLOCK ioStatus{};

        return NtWriteFile(target.Get(), nullptr, nullptr, nullptr, &ioStatus, const_cast<std::byte*>(data.data()), static_cast<ULONG>(data.size()), &byteOffset, nullptr);
    }


    constexpr std::string_view MAJOR = "Write";

}

namespace mimo {

    namespace test {

        namespace operations {

            namespace readwrite {

                bool Run(std::wstring_view ntDirectory, report::Counts& counts) {
                    constexpr std::byte FILL{ 0x4D };
                    Target target{ ntDirectory };
                    const bool opened = OpenTarget(target);

                    for (const Operation& operation : OPERATIONS) {

                        if (!opened) {
                            report::Add(counts, MAJOR, operation.description, std::nullopt);

                            continue;
                        }

                        const std::vector<std::byte> data(operation.size, FILL);
                        const NTSTATUS status = Write(target, operation.offset, data);
                        report::Add(counts, MAJOR, operation.description, status);
                    }

                    return true;
                }

            }

        }

    }

}
