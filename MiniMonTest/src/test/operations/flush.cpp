#include "flush.h"

#include "..\report.h"
#include "..\target.h"

#include "..\..\error.h"
#include "..\..\test.h"

#include <Windows.h>
#undef WIN32_NO_STATUS
#include <ntstatus.h>
#include <winternl.h>

#include <iostream>
#include <optional>
#include <string_view>

extern "C" NTSYSAPI NTSTATUS NTAPI NtFlushBuffersFile(HANDLE hFile, IO_STATUS_BLOCK* pIoStatus);

using namespace mimo;

namespace {

    constexpr std::string_view MAJOR = "Flush";

    constexpr test::Description DESCRIPTIONS[]{
        { MAJOR, L"flush", STATUS_SUCCESS },
    };

    NTSTATUS Flush(const test::Target& target) {
        IO_STATUS_BLOCK ioStatus{};

        return NtFlushBuffersFile(target.Get(), &ioStatus);
    }

}

namespace mimo {

    namespace test {

        namespace operations {

            namespace flush {

                void Run(std::wstring_view ntDirectory, Report& report) {

                    for (const Description& description : DESCRIPTIONS) {
                        Target target{ ntDirectory, description.name };
                        const NTSTATUS createStatus = target.Create(FILE_WRITE_DATA);

                        if (!NT_SUCCESS(createStatus)) {
                            std::cerr << "Failed to create the target file\n";
                            error::Display(RtlNtStatusToDosError(createStatus));
                            report.Add(description, std::nullopt);

                            continue;
                        }

                        const NTSTATUS flushStatus = Flush(target);
                        report.Add(description, flushStatus);
                    }

                    return;
                }

            }

        }

    }

}
