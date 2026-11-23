#include "lock.h"

#include "..\report.h"
#include "..\target.h"

#include "..\..\error.h"

#include <Windows.h>
#undef WIN32_NO_STATUS
#include <ntstatus.h>
#include <winternl.h>

#include <cstdint>
#include <iostream>
#include <optional>
#include <string_view>

extern "C" NTSYSAPI NTSTATUS NTAPI NtLockFile(
    HANDLE hFile,
    HANDLE hEvent,
    PIO_APC_ROUTINE pApcRoutine,
    void* pApcContext,
    IO_STATUS_BLOCK* pIoStatus,
    LARGE_INTEGER* pByteOffset,
    LARGE_INTEGER* pRangeSize,
    ULONG key,
    BOOLEAN failImmediately,
    BOOLEAN exclusiveLock
);

using namespace mimo;

namespace {

    struct LockOperation {
        test::report::Description description;
        uint32_t key;
    };

    constexpr LockOperation LOCK_OPERATIONS[]{
        { { L"lock",     STATUS_SUCCESS }, 0u      },
        { { L"lock-key", STATUS_SUCCESS }, 0x4D4Du },
    };

    NTSTATUS Lock(const test::Target& target, uint32_t key) {
        constexpr LONGLONG RANGE_SIZE = 16;
        LARGE_INTEGER byteOffset{};
        LARGE_INTEGER rangeSize{};
        rangeSize.QuadPart = RANGE_SIZE;
        IO_STATUS_BLOCK ioStatus{};

        return NtLockFile(target.Get(), nullptr, nullptr, nullptr, &ioStatus, &byteOffset, &rangeSize, static_cast<ULONG>(key), TRUE, TRUE);
    }


    constexpr std::string_view MAJOR = "Lock control";

}

namespace mimo {

    namespace test {

        namespace operations {

            namespace lock {

                bool Run(std::wstring_view ntDirectory, report::Counts& counts) {

                    for (const LockOperation& operation : LOCK_OPERATIONS) {
                        Target target{ ntDirectory, operation.description.name };
                        const NTSTATUS createStatus = target.Create(FILE_READ_DATA);

                        if (!NT_SUCCESS(createStatus)) {
                            std::cerr << "Failed to create the target file\n";
                            error::Display(RtlNtStatusToDosError(createStatus));
                            report::Add(counts, MAJOR, operation.description, std::nullopt);

                            continue;
                        }

                        const NTSTATUS lockStatus = Lock(target, operation.key);
                        report::Add(counts, MAJOR, operation.description, lockStatus);
                    }

                    return true;
                }

            }

        }

    }

}
