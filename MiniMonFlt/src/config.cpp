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
            HANDLE hKey = nullptr;
            NTSTATUS status = STATUS_SUCCESS;
            ULONG resultLength = 0u;
            const KEY_VALUE_PARTIAL_INFORMATION* pValuePartialInfo = nullptr;
            UCHAR buffer[sizeof(KEY_VALUE_PARTIAL_INFORMATION) + sizeof(ULONG)]{};

            status = IoOpenDriverRegistryKey(pDriverObject, DriverRegKeyParameters, KEY_READ, 0u, &hKey);

            if (!NT_SUCCESS(status)) goto done;

            pValuePartialInfo = reinterpret_cast<KEY_VALUE_PARTIAL_INFORMATION*>(buffer);
            status = ZwQueryValueKey(hKey, &MaxMemoryName, KeyValuePartialInformation, buffer, static_cast<ULONG>(sizeof(buffer)), &resultLength);

            if (NT_SUCCESS(status) && pValuePartialInfo->Type == REG_DWORD) {
                MaxMemoryKb = *reinterpret_cast<const ULONG*>(&pValuePartialInfo->Data);
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
