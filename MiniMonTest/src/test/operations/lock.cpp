#include "lock.h"

#include "..\report.h"
#include "..\target.h"

#include "..\..\error.h"
#include "..\..\test.h"

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

extern "C" NTSYSAPI NTSTATUS NTAPI NtUnlockFile(
    HANDLE hFile,
    IO_STATUS_BLOCK* pIoStatus,
    LARGE_INTEGER* pByteOffset,
    LARGE_INTEGER* pRangeSize,
    ULONG key
);

using namespace mimo;

namespace {

    struct LockOperation {
        test::Description description;
        uint32_t key;
    };

    constexpr uint32_t NO_KEY = 0u;
    constexpr uint32_t KEY    = 0x4D4Du;

    constexpr std::string_view MAJOR = "Lock control";

    constexpr LockOperation LOCK_OPERATIONS[]{
        { { MAJOR, L"lock",     STATUS_SUCCESS }, NO_KEY },
        { { MAJOR, L"lock-key", STATUS_SUCCESS }, KEY    },
    };

    constexpr LONGLONG RANGE_SIZE = 16;

    NTSTATUS Lock(const test::Target& target, uint32_t key) {
        LARGE_INTEGER byteOffset{};
        LARGE_INTEGER rangeSize{};
        rangeSize.QuadPart = RANGE_SIZE;
        IO_STATUS_BLOCK ioStatus{};

        return NtLockFile(target.Get(), nullptr, nullptr, nullptr, &ioStatus, &byteOffset, &rangeSize, static_cast<ULONG>(key), TRUE, TRUE);
    }


    void RunLockOperations(std::wstring_view ntDirectory, test::Report& report) {

        for (const LockOperation& operation : LOCK_OPERATIONS) {
            test::Target target{ ntDirectory, operation.description.name };
            const NTSTATUS createStatus = target.Create(FILE_READ_DATA);

            if (!NT_SUCCESS(createStatus)) {
                std::cerr << "Failed to create the target file\n";
                error::Display(RtlNtStatusToDosError(createStatus));
                report.Add(operation.description, std::nullopt);

                continue;
            }

            const NTSTATUS lockStatus = Lock(target, operation.key);
            report.Add(operation.description, lockStatus);
        }

        return;
    }


    constexpr test::Description UNLOCK_DESCRIPTIONS[]{
        { MAJOR, L"unlock", STATUS_SUCCESS },
    };

    NTSTATUS Unlock(const test::Target& target) {
        LARGE_INTEGER byteOffset{};
        LARGE_INTEGER rangeSize{};
        rangeSize.QuadPart = RANGE_SIZE;
        IO_STATUS_BLOCK ioStatus{};

        return NtUnlockFile(target.Get(), &ioStatus, &byteOffset, &rangeSize, static_cast<ULONG>(NO_KEY));
    }


    void RunUnlockOperations(std::wstring_view ntDirectory, test::Report& report) {

        for (const test::Description& description : UNLOCK_DESCRIPTIONS) {
            test::Target target{ ntDirectory, description.name };
            const NTSTATUS createStatus = target.Create(FILE_READ_DATA);

            if (!NT_SUCCESS(createStatus)) {
                std::cerr << "Failed to create the target file\n";
                error::Display(RtlNtStatusToDosError(createStatus));
                report.Add(description, std::nullopt);

                continue;
            }

            const NTSTATUS lockStatus = Lock(target, NO_KEY);

            if (!NT_SUCCESS(lockStatus)) {
                std::cerr << "Failed to lock the target file\n";
                error::Display(RtlNtStatusToDosError(lockStatus));
                report.Add(description, std::nullopt);

                continue;
            }

            const NTSTATUS unlockStatus = Unlock(target);
            report.Add(description, unlockStatus);
        }

        return;
    }

}

namespace mimo {

    namespace test {

        namespace operations {

            namespace lock {

                void Run(std::wstring_view ntDirectory, Report& report) {
                    RunLockOperations(ntDirectory, report);
                    RunUnlockOperations(ntDirectory, report);

                    return;
                }

            }

        }

    }

}
