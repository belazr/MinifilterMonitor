#include "security.h"

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
#include <string_view>
#include <vector>

extern "C" NTSYSAPI NTSTATUS NTAPI NtQuerySecurityObject(
    HANDLE hObject,
    SECURITY_INFORMATION securityInformation,
    PSECURITY_DESCRIPTOR pSecurityDescriptor,
    ULONG bufferSize,
    ULONG* pRequiredSize
);

using namespace mimo;

namespace {

    struct QueryOperation {
        test::report::Description description;
        uint32_t size;
    };

    constexpr QueryOperation QUERY_OPERATIONS[]{
        { { L"query-security",          STATUS_SUCCESS          }, 1024u },
        { { L"query-security-overflow", STATUS_BUFFER_TOO_SMALL }, 8u    },
    };

    NTSTATUS Query(const test::Target& target, uint32_t size) {
        constexpr SECURITY_INFORMATION SECURITY_INFORMATION_PARTS = OWNER_SECURITY_INFORMATION | GROUP_SECURITY_INFORMATION | DACL_SECURITY_INFORMATION;
        std::vector<std::byte> buffer(size);
        ULONG requiredSize = 0u;

        return NtQuerySecurityObject(target.Get(), SECURITY_INFORMATION_PARTS, buffer.data(), static_cast<ULONG>(buffer.size()), &requiredSize);
    }


    constexpr std::string_view QUERY_MAJOR = "Query security";

    void RunQueryOperations(std::wstring_view ntDirectory, test::report::Counts& counts) {

        for (const QueryOperation& operation : QUERY_OPERATIONS) {
            test::Target target{ ntDirectory, operation.description.name };
            const NTSTATUS createStatus = target.Create(READ_CONTROL);

            if (!NT_SUCCESS(createStatus)) {
                std::cerr << "Failed to create the target file\n";
                error::Display(RtlNtStatusToDosError(createStatus));
                test::report::Add(counts, QUERY_MAJOR, operation.description, std::nullopt);

                continue;
            }

            const NTSTATUS queryStatus = Query(target, operation.size);
            test::report::Add(counts, QUERY_MAJOR, operation.description, queryStatus);
        }

        return;
    }

}

namespace mimo {

    namespace test {

        namespace operations {

            namespace security {

                bool Run(std::wstring_view ntDirectory, report::Counts& counts) {
                    RunQueryOperations(ntDirectory, counts);

                    return true;
                }

            }

        }

    }

}
