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
#include <iostream>
#include <optional>
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
        { { L"filesystem-get-ntfs-volume-data",    STATUS_SUCCESS }, FSCTL_GET_NTFS_VOLUME_DATA    },
        { { L"filesystem-create-or-get-object-id", STATUS_SUCCESS }, FSCTL_CREATE_OR_GET_OBJECT_ID },
    };

    NTSTATUS Control(const test::Target& target, uint32_t fsControlCode) {
        constexpr size_t BUFFER_SIZE = 1024u;
        std::vector<std::byte> buffer(BUFFER_SIZE);
        IO_STATUS_BLOCK ioStatus{};

        return NtFsControlFile(target.Get(), nullptr, nullptr, nullptr, &ioStatus, static_cast<ULONG>(fsControlCode), nullptr, 0u, buffer.data(), static_cast<ULONG>(buffer.size()));
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
                        const NTSTATUS createStatus = target.Create(FILE_GENERIC_READ);

                        if (!NT_SUCCESS(createStatus)) {
                            std::cerr << "Failed to create the target file\n";
                            error::Display(RtlNtStatusToDosError(createStatus));
                            report::Add(counts, MAJOR, operation.description, std::nullopt);

                            continue;
                        }

                        const NTSTATUS controlStatus = Control(target, operation.fsControlCode);
                        report::Add(counts, MAJOR, operation.description, controlStatus);
                    }

                    return true;
                }

            }

        }

    }

}
