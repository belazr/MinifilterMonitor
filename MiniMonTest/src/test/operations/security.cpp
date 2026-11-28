#include "security.h"

#include "..\report.h"
#include "..\target.h"

#include "..\..\error.h"
#include "..\..\test.h"

#include <Windows.h>
#undef WIN32_NO_STATUS
#include <ntstatus.h>
#include <sddl.h>
#include <winternl.h>

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <optional>
#include <span>
#include <string_view>
#include <vector>

extern "C" NTSYSAPI NTSTATUS NTAPI NtQuerySecurityObject(
    HANDLE hObject,
    SECURITY_INFORMATION securityInformation,
    PSECURITY_DESCRIPTOR pSecurityDescriptor,
    ULONG bufferSize,
    ULONG* pRequiredSize
);

extern "C" NTSYSAPI NTSTATUS NTAPI NtSetSecurityObject(
    HANDLE hObject,
    SECURITY_INFORMATION securityInformation,
    PSECURITY_DESCRIPTOR pSecurityDescriptor
);

using namespace mimo;

namespace {

    struct QueryOperation {
        test::Description description;
        uint32_t size;
    };

    constexpr std::string_view QUERY_MAJOR = "Query security";

    constexpr QueryOperation QUERY_OPERATIONS[]{
        { { QUERY_MAJOR, L"query-security",          STATUS_SUCCESS          }, 1024u },
        { { QUERY_MAJOR, L"query-security-overflow", STATUS_BUFFER_TOO_SMALL }, 8u    },
    };

    NTSTATUS Query(const test::Target& target, uint32_t size) {
        constexpr SECURITY_INFORMATION SECURITY_INFORMATION_PARTS = OWNER_SECURITY_INFORMATION | GROUP_SECURITY_INFORMATION | DACL_SECURITY_INFORMATION;
        std::vector<std::byte> buffer(size);
        ULONG requiredSize = 0u;

        return NtQuerySecurityObject(target.Get(), SECURITY_INFORMATION_PARTS, buffer.data(), static_cast<ULONG>(buffer.size()), &requiredSize);
    }


    void RunQueryOperations(std::wstring_view ntDirectory, test::Report& report) {

        for (const QueryOperation& operation : QUERY_OPERATIONS) {
            test::Target target{ ntDirectory, operation.description.name };
            const NTSTATUS createStatus = target.Create(READ_CONTROL);

            if (!NT_SUCCESS(createStatus)) {
                std::cerr << "Failed to create the target file\n";
                error::Display(RtlNtStatusToDosError(createStatus));
                report.Add(operation.description, std::nullopt);

                continue;
            }

            const NTSTATUS queryStatus = Query(target, operation.size);
            report.Add(operation.description, queryStatus);
        }

        return;
    }


    constexpr std::string_view SET_MAJOR = "Set security";

    constexpr test::Description SET_DESCRIPTIONS[]{
        { SET_MAJOR, L"set-security", STATUS_SUCCESS },
    };

    std::optional<std::vector<std::byte>> MakeDescriptorBuffer() {
        constexpr std::wstring_view DESCRIPTOR_TEXT = L"D:(A;;FA;;;WD)";
        PSECURITY_DESCRIPTOR pSecurityDescriptor = nullptr;
        ULONG size = 0u;

        if (!ConvertStringSecurityDescriptorToSecurityDescriptorW(DESCRIPTOR_TEXT.data(), SDDL_REVISION_1, &pSecurityDescriptor, &size)) {
            const DWORD lastError = GetLastError();
            std::cerr << "Failed to build the security descriptor\n";
            error::Display(lastError);

            return std::nullopt;
        }

        std::vector<std::byte> buffer(static_cast<size_t>(size));
        std::memcpy(buffer.data(), pSecurityDescriptor, buffer.size());
        LocalFree(pSecurityDescriptor);

        return buffer;
    }


    NTSTATUS Set(const test::Target& target, std::span<const std::byte> descriptorData) {
        std::vector<std::byte> buffer(descriptorData.begin(), descriptorData.end());

        return NtSetSecurityObject(target.Get(), DACL_SECURITY_INFORMATION, buffer.data());
    }


    void RunSetOperations(std::wstring_view ntDirectory, test::Report& report) {

        for (const test::Description& description : SET_DESCRIPTIONS) {
            test::Target target{ ntDirectory, description.name };
            const NTSTATUS createStatus = target.Create(WRITE_DAC);

            if (!NT_SUCCESS(createStatus)) {
                std::cerr << "Failed to create the target file\n";
                error::Display(RtlNtStatusToDosError(createStatus));
                report.Add(description, std::nullopt);

                continue;
            }

            const std::optional<std::vector<std::byte>> descriptorData = MakeDescriptorBuffer();

            if (!descriptorData.has_value()) {
                report.Add(description, std::nullopt);

                continue;
            }

            const NTSTATUS setStatus = Set(target, *descriptorData);
            report.Add(description, setStatus);
        }

        return;
    }

}

namespace mimo {

    namespace test {

        namespace operations {

            namespace security {

                void Run(std::wstring_view ntDirectory, Report& report) {
                    RunQueryOperations(ntDirectory, report);
                    RunSetOperations(ntDirectory, report);

                    return;
                }

            }

        }

    }

}
