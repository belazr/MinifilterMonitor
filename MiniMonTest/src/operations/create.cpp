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

    struct Operation {
        test::Description description;
        std::wstring_view fileName;
        ULONG disposition;
    };

    constexpr Operation OPERATIONS[]{
        { { L"create-new",    STATUS_SUCCESS               }, L"file",    FILE_CREATE },
        { { L"open-existing", STATUS_SUCCESS               }, L"file",    FILE_OPEN   },
        { { L"open-missing",  STATUS_OBJECT_NAME_NOT_FOUND }, L"missing", FILE_OPEN   },
    };

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
                constexpr ACCESS_MASK DESIRED_ACCESS = FILE_GENERIC_READ | FILE_WRITE_DATA | DELETE;
                constexpr ULONG SHARE_ALL = FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE;

                for (const Operation& operation : OPERATIONS) {
                    const std::wstring path = std::format(L"{}\\{}", ntDirectory, operation.fileName);
                    FileHandle file;
                    const NTSTATUS status = Create(file, path, DESIRED_ACCESS, SHARE_ALL, operation.disposition, FILE_NON_DIRECTORY_FILE);
                    test::Report(counts, MAJOR, operation.description, status);
                }

                return;
            }

        }

    }

}
