#include "section.h"

#include "..\report.h"
#include "..\target.h"

#include "..\..\error.h"
#include "..\..\handle.h"
#include "..\..\test.h"

#include <Windows.h>
#undef WIN32_NO_STATUS
#include <ntstatus.h>
#include <winternl.h>

#include <iostream>
#include <optional>
#include <string_view>

extern "C" NTSYSAPI NTSTATUS NTAPI NtCreateSection(
    HANDLE* phSection,
    ACCESS_MASK desiredAccess,
    OBJECT_ATTRIBUTES* pObjectAttributes,
    LARGE_INTEGER* pMaximumSize,
    ULONG pageProtection,
    ULONG allocationAttributes,
    HANDLE hFile
);

using namespace mimo;

namespace {

    constexpr std::string_view MAJOR = "Acquire for section synchronization";

    constexpr test::Description DESCRIPTIONS[]{
        { MAJOR, L"section", STATUS_SUCCESS },
    };

    NTSTATUS CreateSection(const test::Target& target) {
        constexpr LONGLONG SECTION_SIZE = 16;
        LARGE_INTEGER maximumSize{};
        maximumSize.QuadPart = SECTION_SIZE;
        NtHandle section;

        return NtCreateSection(section.Put(), SECTION_MAP_READ | SECTION_MAP_WRITE, nullptr, &maximumSize, PAGE_READWRITE, SEC_COMMIT, target.Get());
    }

}

namespace mimo {

    namespace test {

        namespace operations {

            namespace section {

                void Run(std::wstring_view ntDirectory, Report& report) {

                    for (const Description& description : DESCRIPTIONS) {
                        Target target{ ntDirectory, description.name };
                        const NTSTATUS createStatus = target.Create(FILE_READ_DATA | FILE_WRITE_DATA);

                        if (!NT_SUCCESS(createStatus)) {
                            std::cerr << "Failed to create the target file\n";
                            error::Display(RtlNtStatusToDosError(createStatus));
                            report.Print(description, std::nullopt);

                            continue;
                        }

                        const NTSTATUS sectionStatus = CreateSection(target);
                        report.Print(description, sectionStatus);
                    }

                    return;
                }

            }

        }

    }

}
