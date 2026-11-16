#include "ea.h"

#include "..\report.h"
#include "..\target.h"

#include "..\..\error.h"

#include <Windows.h>
#undef WIN32_NO_STATUS
#include <ntstatus.h>
#include <winternl.h>

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <optional>
#include <span>
#include <string_view>
#include <vector>

extern "C" NTSYSAPI NTSTATUS NTAPI NtSetEaFile(
    HANDLE hFile,
    IO_STATUS_BLOCK* pIoStatus,
    void* pBuffer,
    ULONG bufferSize
);

using namespace mimo;

namespace {

    struct SetOperation {
        test::report::Description description;
        uint16_t valueSize;
    };

    constexpr SetOperation SET_OPERATIONS[]{
        { { L"set-ea",           STATUS_SUCCESS }, 8u    },
        { { L"set-ea-oversized", STATUS_SUCCESS }, 2048u },
    };

    struct FILE_FULL_EA_INFORMATION {
        ULONG NextEntryOffset;
        UCHAR Flags;
        UCHAR EaNameLength;
        USHORT EaValueLength;
        CHAR EaName[1u];
    };

    std::vector<std::byte> MakeEaBuffer(uint16_t valueSize) {
        constexpr std::string_view NAME = "MINIMON";
        constexpr size_t NAME_OFFSET = offsetof(FILE_FULL_EA_INFORMATION, EaName);
        FILE_FULL_EA_INFORMATION entry{};
        entry.EaNameLength = static_cast<UCHAR>(NAME.size());
        entry.EaValueLength = static_cast<USHORT>(valueSize);

        std::vector<std::byte> buffer(NAME_OFFSET + NAME.size() + 1u + valueSize);
        std::memcpy(buffer.data(), &entry, NAME_OFFSET);
        std::memcpy(buffer.data() + NAME_OFFSET, NAME.data(), NAME.size());

        return buffer;
    }


    NTSTATUS Set(const test::Target& target, std::span<const std::byte> data) {
        std::vector<std::byte> buffer(data.begin(), data.end());
        IO_STATUS_BLOCK ioStatus{};

        return NtSetEaFile(target.Get(), &ioStatus, buffer.data(), static_cast<ULONG>(buffer.size()));
    }


    constexpr std::string_view SET_MAJOR = "Set EA";

    void RunSetOperations(std::wstring_view ntDirectory, test::report::Counts& counts) {

        for (const SetOperation& operation : SET_OPERATIONS) {
            test::Target target{ ntDirectory, operation.description.name };
            const NTSTATUS createStatus = target.Create(FILE_WRITE_EA);

            if (!NT_SUCCESS(createStatus)) {
                std::cerr << "Failed to create the target file\n";
                error::Display(RtlNtStatusToDosError(createStatus));
                test::report::Add(counts, SET_MAJOR, operation.description, std::nullopt);

                continue;
            }

            const std::vector<std::byte> data = MakeEaBuffer(operation.valueSize);
            const NTSTATUS setStatus = Set(target, data);
            test::report::Add(counts, SET_MAJOR, operation.description, setStatus);
        }

        return;
    }

}

namespace mimo {

    namespace test {

        namespace operations {

            namespace ea {

                bool Run(std::wstring_view ntDirectory, report::Counts& counts) {
                    RunSetOperations(ntDirectory, counts);

                    return true;
                }

            }

        }

    }

}
