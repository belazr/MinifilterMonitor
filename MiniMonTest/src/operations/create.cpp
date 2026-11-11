#include "create.h"

#include "..\handle.h"
#include "..\test.h"

#include <Windows.h>
#undef WIN32_NO_STATUS
#include <ntstatus.h>
#include <winternl.h>

#include <format>
#include <string>
#include <string_view>

using namespace mimo;

namespace {

    using FileHandle = Handle<NtClose>;

    NTSTATUS Create(
        FileHandle& file,
        std::wstring_view path,
        ACCESS_MASK desiredAccess,
        ULONG shareAccess,
        ULONG disposition,
        ULONG options
    ) {
        UNICODE_STRING objectName{};
        objectName.Length = static_cast<USHORT>(path.size() * sizeof(wchar_t));
        objectName.MaximumLength = objectName.Length;
        objectName.Buffer = const_cast<wchar_t*>(path.data());

        OBJECT_ATTRIBUTES objectAttributes{};
        InitializeObjectAttributes(&objectAttributes, &objectName, OBJ_CASE_INSENSITIVE, nullptr, nullptr);

        IO_STATUS_BLOCK ioStatus{};

        return NtCreateFile(file.Put(), desiredAccess, &objectAttributes, &ioStatus, nullptr, FILE_ATTRIBUTE_NORMAL, shareAccess, disposition, options, nullptr, 0u);
    }


    constexpr std::string_view MAJOR = "Create";

}

namespace mimo {

    namespace operations {

        namespace create {

            void Run(std::wstring_view ntDirectory, test::Counts& counts) {
                constexpr test::Description DESCRIPTION{ L"open-missing", STATUS_OBJECT_NAME_NOT_FOUND };
                constexpr ACCESS_MASK DESIRED_ACCESS = FILE_GENERIC_READ | FILE_WRITE_DATA | DELETE;
                constexpr ULONG SHARE_ALL = FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE;
                const std::wstring path = std::format(L"{}\\{}", ntDirectory, DESCRIPTION.name);
                FileHandle file;
                const test::Result result{ Create(file, path, DESIRED_ACCESS, SHARE_ALL, FILE_OPEN, FILE_NON_DIRECTORY_FILE) };
                test::Report(counts, MAJOR, DESCRIPTION, result);

                return;
            }

        }

    }

}
