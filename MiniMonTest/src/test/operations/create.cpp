#include "create.h"

#include "..\report.h"

#include "..\..\error.h"
#include "..\..\handle.h"

#include <Windows.h>
#undef WIN32_NO_STATUS
#include <ntstatus.h>
#include <winternl.h>

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <format>
#include <iostream>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>

using namespace mimo;

namespace {

    struct Operation {
        test::report::Description description;
        std::wstring_view fileName;
        ULONG disposition;
        uint16_t eaValueSize;
        bool impersonating;
    };

    constexpr Operation OPERATIONS[]{
        { { L"create-new",          STATUS_SUCCESS               }, L"file",         FILE_CREATE, 0u,    false },
        { { L"open-existing",       STATUS_SUCCESS               }, L"file",         FILE_OPEN,   0u,    false },
        { { L"open-missing",        STATUS_OBJECT_NAME_NOT_FOUND }, L"missing",      FILE_OPEN,   0u,    false },
        { { L"create-ea",           STATUS_SUCCESS               }, L"ea",           FILE_CREATE, 8u,    false },
        { { L"create-ea-oversized", STATUS_SUCCESS               }, L"ea-oversized", FILE_CREATE, 1024u, false },
        { { L"open-impersonating",  STATUS_SUCCESS               }, L"file",         FILE_OPEN,   0u,    true  },
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


    bool StartImpersonation() {

        if (!ImpersonateSelf(SecurityImpersonation)) {
            const DWORD lastError = GetLastError();
            std::cerr << "Failed to impersonate\n";
            error::Display(lastError);

            return false;
        }

        return true;
    }


    bool StopImpersonation() {

        if (!RevertToSelf()) {
            const DWORD lastError = GetLastError();
            std::cerr << "Failed to revert the impersonation\n";
            error::Display(lastError);

            return false;
        }

        return true;
    }


    using FileHandle = Handle<NtClose>;

    NTSTATUS Create(
        FileHandle& file,
        std::wstring_view path,
        ACCESS_MASK desiredAccess,
        ULONG shareAccess,
        ULONG disposition,
        ULONG options,
        std::span<const std::byte> eaBuffer
    ) {
        UNICODE_STRING objectName{};
        objectName.Length = static_cast<USHORT>(path.size() * sizeof(wchar_t));
        objectName.MaximumLength = objectName.Length;
        objectName.Buffer = const_cast<wchar_t*>(path.data());

        OBJECT_ATTRIBUTES objectAttributes{};
        InitializeObjectAttributes(&objectAttributes, &objectName, OBJ_CASE_INSENSITIVE, nullptr, nullptr);

        IO_STATUS_BLOCK ioStatus{};

        return NtCreateFile(file.Put(), desiredAccess, &objectAttributes, &ioStatus, nullptr, FILE_ATTRIBUTE_NORMAL, shareAccess, disposition, options, const_cast<std::byte*>(eaBuffer.data()), static_cast<ULONG>(eaBuffer.size()));
    }


    constexpr std::string_view MAJOR = "Create";

}

namespace mimo {

    namespace test {

        namespace operations {

            namespace create {

                bool Run(std::wstring_view ntDirectory, report::Counts& counts) {
                    constexpr ACCESS_MASK DESIRED_ACCESS = FILE_GENERIC_READ | FILE_WRITE_DATA | DELETE;
                    constexpr ULONG SHARE_ALL = FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE;

                    for (const Operation& operation : OPERATIONS) {
                        const std::wstring path = std::format(L"{}\\{}", ntDirectory, operation.fileName);
                        const std::vector<std::byte> eaBuffer = operation.eaValueSize ? MakeEaBuffer(operation.eaValueSize) : std::vector<std::byte>{};

                        if (operation.impersonating && !StartImpersonation()) {
                            report::Add(counts, MAJOR, operation.description, std::nullopt);

                            continue;
                        }

                        FileHandle file;
                        const NTSTATUS status = Create(file, path, DESIRED_ACCESS, SHARE_ALL, operation.disposition, FILE_NON_DIRECTORY_FILE, eaBuffer);

                        const bool reverted = !operation.impersonating || StopImpersonation();
                        report::Add(counts, MAJOR, operation.description, status);

                        if (!reverted) return false;

                    }

                    return true;
                }

            }

        }

    }

}
