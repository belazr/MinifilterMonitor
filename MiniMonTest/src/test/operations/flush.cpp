#include "flush.h"

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

extern "C" NTSYSAPI NTSTATUS NTAPI NtFlushBuffersFileEx(
    HANDLE hFile,
    ULONG flags,
    void* pParameters,
    ULONG parametersSize,
    IO_STATUS_BLOCK* pIoStatus
);

using namespace mimo;

namespace {

    struct Operation {
        test::report::Description description;
        uint32_t flags;
    };

    constexpr Operation OPERATIONS[]{
        { { L"flush",                STATUS_SUCCESS }, 0u                              },
        { { L"flush-data-only",      STATUS_SUCCESS }, FLUSH_FLAGS_FILE_DATA_ONLY      },
        { { L"flush-no-sync",        STATUS_SUCCESS }, FLUSH_FLAGS_NO_SYNC             },
        { { L"flush-data-sync-only", STATUS_SUCCESS }, FLUSH_FLAGS_FILE_DATA_SYNC_ONLY },
        { { L"flush-and-purge",      STATUS_SUCCESS }, FLUSH_FLAGS_FLUSH_AND_PURGE     },
    };

    NTSTATUS Flush(const test::Target& target, uint32_t flags) {
        IO_STATUS_BLOCK ioStatus{};

        return NtFlushBuffersFileEx(target.Get(), static_cast<ULONG>(flags), nullptr, 0u, &ioStatus);
    }


    constexpr std::string_view MAJOR = "Flush";

}

namespace mimo {

    namespace test {

        namespace operations {

            namespace flush {

                bool Run(std::wstring_view ntDirectory, report::Counts& counts) {

                    for (const Operation& operation : OPERATIONS) {
                        Target target{ ntDirectory, operation.description.name };
                        const NTSTATUS createStatus = target.Create(FILE_WRITE_DATA);

                        if (!NT_SUCCESS(createStatus)) {
                            std::cerr << "Failed to create the target file\n";
                            error::Display(RtlNtStatusToDosError(createStatus));
                            report::Add(counts, MAJOR, operation.description, std::nullopt);

                            continue;
                        }

                        const NTSTATUS flushStatus = Flush(target, operation.flags);
                        report::Add(counts, MAJOR, operation.description, flushStatus);
                    }

                    return true;
                }

            }

        }

    }

}
