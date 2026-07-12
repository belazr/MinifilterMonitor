#include "config.h"

#include "driver.h"

#include <fltKernel.h>

using namespace mimo;

namespace {

    constexpr ULONG DEFAULT_MAX_MEMORY_KB = 50u * 1024u;
    UNICODE_STRING MaxMemoryName = RTL_CONSTANT_STRING(L"MaxMemoryKB");
    ULONG MaxMemoryKb = DEFAULT_MAX_MEMORY_KB;

    UNICODE_STRING RegistryPath;

    __declspec(code_seg("INIT"))
    void ReadParameters(_In_ UNICODE_STRING* pRegistryPath) {
        OBJECT_ATTRIBUTES objAttribs{};
        HANDLE hKey = nullptr;
        NTSTATUS status = STATUS_SUCCESS;
        ULONG resultLength = 0u;
        PKEY_VALUE_PARTIAL_INFORMATION pValuePartialInfo = nullptr;
        UCHAR buffer[sizeof(KEY_VALUE_PARTIAL_INFORMATION) + sizeof(ULONG)]{};

        InitializeObjectAttributes(&objAttribs, pRegistryPath, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, nullptr, nullptr);
        status = ZwOpenKey(&hKey, KEY_READ, &objAttribs);

        if (!NT_SUCCESS(status)) goto done;

        status = ZwQueryValueKey(hKey, &MaxMemoryName, KeyValuePartialInformation, buffer, sizeof(buffer), &resultLength);

        if (status == STATUS_SUCCESS) {
            pValuePartialInfo = reinterpret_cast<PKEY_VALUE_PARTIAL_INFORMATION>(buffer);

            if (pValuePartialInfo->Type == REG_DWORD) {
                MaxMemoryKb = *reinterpret_cast<ULONG*>(&pValuePartialInfo->Data);
            }

        }

    done:

        if (hKey) {
            ZwClose(hKey);
        }

        return;
    }


    __declspec(code_seg("PAGE"))
    ULONG ReadAltitudeValue(_In_ HANDLE hInstanceKey) {
        PAGED_CODE();

        UNICODE_STRING altitudeName = RTL_CONSTANT_STRING(L"Altitude");
        UCHAR buffer[sizeof(KEY_VALUE_PARTIAL_INFORMATION) + 32u * sizeof(WCHAR)]{};
        ULONG resultLength = 0u;
        const KEY_VALUE_PARTIAL_INFORMATION* pInfo = nullptr;
        UNICODE_STRING altStr{};
        ULONG altitude = 0u;

        if (!NT_SUCCESS(ZwQueryValueKey(hInstanceKey, &altitudeName, KeyValuePartialInformation, buffer, sizeof(buffer), &resultLength))) {

            return 0u;
        }

        pInfo = reinterpret_cast<KEY_VALUE_PARTIAL_INFORMATION*>(buffer);

        if (pInfo->Type != REG_SZ || pInfo->DataLength < sizeof(WCHAR)) return 0u;

        altStr.Buffer = reinterpret_cast<PWCH>(const_cast<UCHAR*>(pInfo->Data));
        altStr.Length = static_cast<USHORT>(pInfo->DataLength);
        altStr.MaximumLength = altStr.Length;

        if (altStr.Length >= sizeof(WCHAR) && altStr.Buffer[(altStr.Length / sizeof(WCHAR)) - 1u] == L'\0') {
            altStr.Length -= sizeof(WCHAR);
        }

        if (!NT_SUCCESS(RtlUnicodeStringToInteger(&altStr, 10u, &altitude))) return 0u;

        return altitude;
    }

}

namespace mimo {

    namespace config {

        __declspec(code_seg("INIT"))
        _Use_decl_annotations_
        NTSTATUS Create(UNICODE_STRING* pRegistryPath) {
            RegistryPath.Buffer = reinterpret_cast<PWCH>(ExAllocatePool2(POOL_FLAG_PAGED, pRegistryPath->Length, driver::MEM_TAG));

            if (!RegistryPath.Buffer) return STATUS_INSUFFICIENT_RESOURCES;

            RtlCopyMemory(RegistryPath.Buffer, pRegistryPath->Buffer, pRegistryPath->Length);
            RegistryPath.Length = pRegistryPath->Length;
            RegistryPath.MaximumLength = pRegistryPath->Length;

            ReadParameters(pRegistryPath);

            return STATUS_SUCCESS;
        }


        void Delete() {

            if (RegistryPath.Buffer) {
                ExFreePoolWithTag(RegistryPath.Buffer, driver::MEM_TAG);
                RegistryPath.Buffer = nullptr;
                RegistryPath.Length = 0u;
                RegistryPath.MaximumLength = 0u;
            }

            return;
        }


        ULONG GetMaxMemoryKb() {

            return MaxMemoryKb;
        }


        __declspec(code_seg("PAGE"))
        _Use_decl_annotations_
        ULONG GetInstanceAltitude(const FLT_RELATED_OBJECTS* pFltObjects) {
            PAGED_CODE();

            const UNICODE_STRING suffix = RTL_CONSTANT_STRING(L"\\Parameters\\Instances");
            WCHAR pathBuffer[256u]{};
            UNICODE_STRING instancesPath{};
            OBJECT_ATTRIBUTES objAttribs{};
            HANDLE hInstancesKey = nullptr;
            bool found = false;
            ULONG index = 0u;
            UCHAR enumBuffer[sizeof(KEY_BASIC_INFORMATION) + 128u * sizeof(WCHAR)]{};
            ULONG resultLength = 0u;
            const KEY_BASIC_INFORMATION* pBasicInfo = nullptr;
            UNICODE_STRING instanceName{};
            PFLT_INSTANCE pCandidate = nullptr;
            HANDLE hCurInstanceKey = nullptr;
            OBJECT_ATTRIBUTES subAttribs{};
            ULONG altitude = 0u;

            if (!RegistryPath.Buffer) goto done;

            instancesPath.Buffer = pathBuffer;
            instancesPath.Length = 0u;
            instancesPath.MaximumLength = sizeof(pathBuffer);

            if (!NT_SUCCESS(RtlAppendUnicodeStringToString(&instancesPath, &RegistryPath))) goto done;

            if (!NT_SUCCESS(RtlAppendUnicodeStringToString(&instancesPath, &suffix))) goto done;

            InitializeObjectAttributes(&objAttribs, &instancesPath, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, nullptr, nullptr);

            if (!NT_SUCCESS(ZwOpenKey(&hInstancesKey, KEY_READ, &objAttribs))) {
                hInstancesKey = nullptr;

                goto done;
            }

            while (!found) {

                if (!NT_SUCCESS(ZwEnumerateKey(hInstancesKey, index, KeyBasicInformation, enumBuffer, sizeof(enumBuffer), &resultLength))) break;

                index++;

                pBasicInfo = reinterpret_cast<KEY_BASIC_INFORMATION*>(enumBuffer);
                instanceName.Buffer = const_cast<PWCH>(pBasicInfo->Name);
                instanceName.Length = static_cast<USHORT>(pBasicInfo->NameLength);
                instanceName.MaximumLength = instanceName.Length;

                if (!NT_SUCCESS(FltGetVolumeInstanceFromName(pFltObjects->Filter, pFltObjects->Volume, &instanceName, &pCandidate))) continue;

                found = pCandidate == pFltObjects->Instance;
                FltObjectDereference(pCandidate);

                if (!found) continue;

                InitializeObjectAttributes(&subAttribs, &instanceName, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, hInstancesKey, nullptr);

                if (!NT_SUCCESS(ZwOpenKey(&hCurInstanceKey, KEY_READ, &subAttribs))) break;

                altitude = ReadAltitudeValue(hCurInstanceKey);
                ZwClose(hCurInstanceKey);
            }

        done:

            if (hInstancesKey) {
                ZwClose(hInstancesKey);
            }

            return altitude;
        }

    }

}
