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
        test::report::Description description;
        uint32_t key;
    };

    constexpr uint32_t NO_KEY = 0u;
    constexpr uint32_t KEY    = 0x4D4Du;

    constexpr LockOperation LOCK_OPERATIONS[]{
        { { L"lock",     STATUS_SUCCESS }, NO_KEY },
        { { L"lock-key", STATUS_SUCCESS }, KEY    },
    };

    constexpr LONGLONG RANGE_SIZE = 16;

    NTSTATUS Lock(const test::Target& target, uint32_t key) {
        LARGE_INTEGER byteOffset{};
        LARGE_INTEGER rangeSize{};
        rangeSize.QuadPart = RANGE_SIZE;
        IO_STATUS_BLOCK ioStatus{};

        return NtLockFile(target.Get(), nullptr, nullptr, nullptr, &ioStatus, &byteOffset, &rangeSize, static_cast<ULONG>(key), TRUE, TRUE);
    }


    constexpr std::string_view MAJOR = "Lock control";

    void RunLockOperations(std::wstring_view ntDirectory, test::report::Counts& counts) {

        for (const LockOperation& operation : LOCK_OPERATIONS) {
            test::Target target{ ntDirectory, operation.description.name };
            const NTSTATUS createStatus = target.Create(FILE_READ_DATA);

            if (!NT_SUCCESS(createStatus)) {
                std::cerr << "Failed to create the target file\n";
                error::Display(RtlNtStatusToDosError(createStatus));
                test::report::Add(counts, MAJOR, operation.description, std::nullopt);

                continue;
            }

            const NTSTATUS lockStatus = Lock(target, operation.key);
            test::report::Add(counts, MAJOR, operation.description, lockStatus);
        }

        return;
    }


    constexpr test::report::Description UNLOCK_DESCRIPTIONS[]{
        { L"unlock", STATUS_SUCCESS },
    };

    NTSTATUS Unlock(const test::Target& target) {
        LARGE_INTEGER byteOffset{};
        LARGE_INTEGER rangeSize{};
        rangeSize.QuadPart = RANGE_SIZE;
        IO_STATUS_BLOCK ioStatus{};

        return NtUnlockFile(target.Get(), &ioStatus, &byteOffset, &rangeSize, static_cast<ULONG>(NO_KEY));
    }


    void RunUnlockOperations(std::wstring_view ntDirectory, test::report::Counts& counts) {

        for (const test::report::Description& description : UNLOCK_DESCRIPTIONS) {
            test::Target target{ ntDirectory, description.name };
            const NTSTATUS createStatus = target.Create(FILE_READ_DATA);

            if (!NT_SUCCESS(createStatus)) {
                std::cerr << "Failed to create the target file\n";
                error::Display(RtlNtStatusToDosError(createStatus));
                test::report::Add(counts, MAJOR, description, std::nullopt);

                continue;
            }

            const NTSTATUS lockStatus = Lock(target, NO_KEY);

            if (!NT_SUCCESS(lockStatus)) {
                std::cerr << "Failed to lock the target file\n";
                error::Display(RtlNtStatusToDosError(lockStatus));
                test::report::Add(counts, MAJOR, description, std::nullopt);

                continue;
            }

            const NTSTATUS unlockStatus = Unlock(target);
            test::report::Add(counts, MAJOR, description, unlockStatus);
        }

        return;
    }

}

namespace mimo {

    namespace test {

        namespace operations {

            namespace lock {

                void Run(std::wstring_view ntDirectory, report::Counts& counts) {
                    RunLockOperations(ntDirectory, counts);
                    RunUnlockOperations(ntDirectory, counts);

                    return;
                }

            }

        }

    }

}
