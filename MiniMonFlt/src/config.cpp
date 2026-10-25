#include "config.h"

#include <fltKernel.h>

using namespace mimo;

namespace {

    constexpr ULONG DEFAULT_MAX_MEMORY_KB = 50u * 1024u;
    UNICODE_STRING MaxMemoryName = RTL_CONSTANT_STRING(L"MaxMemoryKB");
    ULONG MaxMemoryKb = DEFAULT_MAX_MEMORY_KB;

}

namespace mimo {

    namespace config {

        __declspec(code_seg("INIT"))
        _Use_decl_annotations_
        void Create(DRIVER_OBJECT* pDriverObject) {
            constexpr size_t TYPE_OFFSET = __builtin_offsetof(KEY_VALUE_PARTIAL_INFORMATION, Type);
            constexpr size_t DATA_OFFSET = __builtin_offsetof(KEY_VALUE_PARTIAL_INFORMATION, Data);
            HANDLE hKey = nullptr;
            NTSTATUS status = STATUS_SUCCESS;
            ULONG resultLength = 0u;
            ULONG type = 0u;
            UCHAR buffer[sizeof(KEY_VALUE_PARTIAL_INFORMATION) + sizeof(ULONG)]{};

            status = IoOpenDriverRegistryKey(pDriverObject, DriverRegKeyParameters, KEY_READ, 0u, &hKey);

            if (!NT_SUCCESS(status)) goto done;

            status = ZwQueryValueKey(hKey, &MaxMemoryName, KeyValuePartialInformation, buffer, static_cast<ULONG>(sizeof(buffer)), &resultLength);

            if (!NT_SUCCESS(status)) goto done;

            RtlCopyMemory(&type, buffer + TYPE_OFFSET, sizeof(type));

            if (type == REG_DWORD) {
                RtlCopyMemory(&MaxMemoryKb, buffer + DATA_OFFSET, sizeof(MaxMemoryKb));
            }

        done:

            if (hKey) {
                ZwClose(hKey);
            }

            return;
        }


        ULONG GetMaxMemoryKb() {

            return MaxMemoryKb;
        }

    }

}
