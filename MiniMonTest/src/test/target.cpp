#include "target.h"

#include <Windows.h>
#include <winternl.h>

#include <format>
#include <string_view>

namespace mimo {

    namespace test {

        Target::Target(std::wstring_view ntDirectory, std::wstring_view name) : path(std::format(L"{}\\{}", ntDirectory, name)) {}


        NTSTATUS Target::Create(ACCESS_MASK desiredAccess) {
            constexpr ULONG SHARE_ALL = FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE;
            constexpr ULONG OPTIONS = FILE_NON_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT;
            UNICODE_STRING objectName{};
            objectName.Length = static_cast<USHORT>(this->path.size() * sizeof(wchar_t));
            objectName.MaximumLength = objectName.Length;
            objectName.Buffer = this->path.data();

            OBJECT_ATTRIBUTES objectAttributes{};
            InitializeObjectAttributes(&objectAttributes, &objectName, OBJ_CASE_INSENSITIVE, nullptr, nullptr);

            IO_STATUS_BLOCK ioStatus{};

            return NtCreateFile(this->file.Put(), desiredAccess | SYNCHRONIZE, &objectAttributes, &ioStatus, nullptr, FILE_ATTRIBUTE_NORMAL, SHARE_ALL, FILE_CREATE, OPTIONS, nullptr, 0u);
        }


        HANDLE Target::Get() const {

            return this->file.Get();
        }

    }

}
