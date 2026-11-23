#include "deviceio.h"

#include "..\report.h"
#include "..\target.h"

#include "..\..\error.h"

#include <Windows.h>
#undef WIN32_NO_STATUS
#include <ntstatus.h>
#include <winioctl.h>
#include <winternl.h>

#include <cstddef>
#include <iostream>
#include <optional>
#include <string_view>
#include <vector>

using namespace mimo;

namespace {

    // the file system refuses every device control code on a file handle
    // the filter captures the input before the refusal, no output comes back
    constexpr test::report::Description DESCRIPTIONS[]{
        { L"deviceio-query-property", STATUS_INVALID_PARAMETER },
    };

    NTSTATUS QueryProperty(const test::Target& target) {
        constexpr size_t OUTPUT_BUFFER_SIZE = 1024u;
        STORAGE_PROPERTY_QUERY query{};
        query.PropertyId = StorageDeviceProperty;
        query.QueryType = PropertyStandardQuery;

        std::vector<std::byte> outputBuffer(OUTPUT_BUFFER_SIZE);
        IO_STATUS_BLOCK ioStatus{};

        return NtDeviceIoControlFile(target.Get(), nullptr, nullptr, nullptr, &ioStatus, IOCTL_STORAGE_QUERY_PROPERTY, &query, sizeof(query), outputBuffer.data(), static_cast<ULONG>(outputBuffer.size()));
    }


    constexpr std::string_view MAJOR = "Device control";

}

namespace mimo {

    namespace test {

        namespace operations {

            namespace deviceio {

                bool Run(std::wstring_view ntDirectory, report::Counts& counts) {

                    for (const report::Description& description : DESCRIPTIONS) {
                        Target target{ ntDirectory, description.name };
                        const NTSTATUS createStatus = target.Create(FILE_GENERIC_READ);

                        if (!NT_SUCCESS(createStatus)) {
                            std::cerr << "Failed to create the target file\n";
                            error::Display(RtlNtStatusToDosError(createStatus));
                            report::Add(counts, MAJOR, description, std::nullopt);

                            continue;
                        }

                        const NTSTATUS queryStatus = QueryProperty(target);
                        report::Add(counts, MAJOR, description, queryStatus);
                    }

                    return true;
                }

            }

        }

    }

}
