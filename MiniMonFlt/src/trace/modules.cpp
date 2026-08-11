#include "modules.h"

#include "..\driver.h"

#include "..\..\..\inc\protocol.h"

#include <aux_klib.h>
#include <fltKernel.h>

using namespace mimo;
// ResolveAddress's SAL needs the unqualified name
using protocol::STACK_FRAME_NAME_WCHARS;

namespace {

    constexpr ULONG MODULE_NAME_WCHARS = 64u;

    struct ModuleEntry {
        LIST_ENTRY list;
        const void* pBase;
        SIZE_T size;
        WCHAR name[MODULE_NAME_WCHARS];
    };

    KSPIN_LOCK ModuleListLock;
    LIST_ENTRY ModuleList;
    bool ImageNotifyRegistered;

    void AddModule(
        _In_ const void* pBase,
        _In_ SIZE_T size,
        _In_ const UNICODE_STRING* pName
    ) {
        FLT_ASSERT(pName);

        ModuleEntry* const pNewEntry = reinterpret_cast<ModuleEntry*>(ExAllocatePool2(POOL_FLAG_NON_PAGED, sizeof(ModuleEntry), driver::MEM_TAG));

        if (!pNewEntry) return;

        pNewEntry->pBase = pBase;
        pNewEntry->size = size;

        USHORT copyChars = pName->Length / sizeof(WCHAR);

        if (copyChars >= MODULE_NAME_WCHARS) {
            copyChars = MODULE_NAME_WCHARS - 1u;
        }

        RtlCopyMemory(pNewEntry->name, pName->Buffer, copyChars * sizeof(WCHAR));
        pNewEntry->name[copyChars] = L'\0';

        KIRQL oldIrql{};
        KeAcquireSpinLock(&ModuleListLock, &oldIrql);

        bool duplicate = false;

        for (LIST_ENTRY* pListEntry = ModuleList.Flink; pListEntry != &ModuleList; pListEntry = pListEntry->Flink) {
            const ModuleEntry* const pExisting = CONTAINING_RECORD(pListEntry, ModuleEntry, list);

            if (pExisting->pBase == pBase) {
                duplicate = true;

                break;
            }

        }

        if (!duplicate) {
            InsertHeadList(&ModuleList, &pNewEntry->list);
        }

        KeReleaseSpinLock(&ModuleListLock, oldIrql);

        if (duplicate) {
            ExFreePoolWithTag(pNewEntry, driver::MEM_TAG);
        }

        return;
    }


    void OnImageLoad(
        _In_opt_ PUNICODE_STRING pFullImageName,
        _In_ HANDLE processId,
        _In_ PIMAGE_INFO pImageInfo
    ) {
        UNREFERENCED_PARAMETER(processId);

        if (!pImageInfo || !pImageInfo->SystemModeImage) return;

        if (!pFullImageName || !pFullImageName->Buffer || pFullImageName->Length == 0u) return;

        const USHORT charCount = pFullImageName->Length / sizeof(WCHAR);
        USHORT i = charCount;

        while (i > 0u && pFullImageName->Buffer[i - 1u] != L'\\') {
            i--;
        }

        UNICODE_STRING baseName{};
        baseName.Buffer = pFullImageName->Buffer + i;
        baseName.Length = static_cast<USHORT>((charCount - i) * sizeof(WCHAR));
        baseName.MaximumLength = baseName.Length;

        if (baseName.Length == 0u) return;

        AddModule(pImageInfo->ImageBase, pImageInfo->ImageSize, &baseName);

        return;
    }


    void ResolveAddress(
        _In_ const void* pAddress,
        _Out_writes_z_(STACK_FRAME_NAME_WCHARS) WCHAR* pNameBuffer,
        _Out_ ULONGLONG* pOffset
    ) {
        const ULONG_PTR addressVal = reinterpret_cast<ULONG_PTR>(pAddress);
        bool found = false;

        for (LIST_ENTRY* pListEntry = ModuleList.Flink; pListEntry != &ModuleList; pListEntry = pListEntry->Flink) {
            const ModuleEntry* const pEntry = CONTAINING_RECORD(pListEntry, ModuleEntry, list);
            const ULONG_PTR baseVal = reinterpret_cast<ULONG_PTR>(pEntry->pBase);

            if (addressVal < baseVal) continue;

            const ULONG_PTR offset = addressVal - baseVal;

            if (offset >= pEntry->size) continue;

            USHORT copyChars = 0u;

            while (copyChars < protocol::STACK_FRAME_NAME_WCHARS - 1u && pEntry->name[copyChars] != L'\0') {
                pNameBuffer[copyChars] = pEntry->name[copyChars];
                copyChars++;
            }

            pNameBuffer[copyChars] = L'\0';

            *pOffset = offset;
            found = true;

            break;
        }

        if (!found) {
            pNameBuffer[0] = L'\0';
            *pOffset = addressVal;
        }

        return;
    }

}

namespace mimo {

    namespace trace {

        namespace modules {

            __declspec(code_seg("INIT"))
            void Create() {
                InitializeListHead(&ModuleList);
                KeInitializeSpinLock(&ModuleListLock);
                ImageNotifyRegistered = false;

                return;
            }


            __declspec(code_seg("INIT"))
            NTSTATUS Init() {
                NTSTATUS status = STATUS_SUCCESS;
                ULONG bufferSize = 0u;
                AUX_MODULE_EXTENDED_INFO* pModules = nullptr;
                ULONG moduleCount = 0u;
                ANSI_STRING ansiName{};
                UNICODE_STRING unicodeName{};
                WCHAR wideBuffer[MODULE_NAME_WCHARS]{};
                const UCHAR* pBaseName = nullptr;
                USHORT ansiCount = 0u;
                USHORT remaining = 0u;

                if (!ModuleList.Flink) {
                    status = STATUS_INVALID_DEVICE_STATE;

                    goto done;
                }

                status = PsSetLoadImageNotifyRoutine(OnImageLoad);

                if (!NT_SUCCESS(status)) goto done;

                ImageNotifyRegistered = true;

                status = AuxKlibInitialize();

                if (!NT_SUCCESS(status)) goto done;

                status = AuxKlibQueryModuleInformation(&bufferSize, sizeof(AUX_MODULE_EXTENDED_INFO), nullptr);

                if (!NT_SUCCESS(status) || bufferSize == 0u) goto done;

                pModules = reinterpret_cast<AUX_MODULE_EXTENDED_INFO*>(ExAllocatePool2(POOL_FLAG_PAGED, bufferSize, driver::MEM_TAG));

                if (!pModules) {
                    status = STATUS_INSUFFICIENT_RESOURCES;

                    goto done;
                }

                status = AuxKlibQueryModuleInformation(&bufferSize, sizeof(AUX_MODULE_EXTENDED_INFO), pModules);

                if (!NT_SUCCESS(status)) goto done;

                moduleCount = bufferSize / sizeof(AUX_MODULE_EXTENDED_INFO);

                for (ULONG i = 0u; i < moduleCount; i++) {
                    pBaseName = pModules[i].FullPathName + pModules[i].FileNameOffset;
                    remaining = static_cast<USHORT>(AUX_KLIB_MODULE_PATH_LEN - pModules[i].FileNameOffset);
                    ansiCount = 0u;

                    while (ansiCount < remaining && pBaseName[ansiCount] != 0u) {
                        ansiCount++;
                    }

                    if (ansiCount == 0u) continue;

                    ansiName.Buffer = reinterpret_cast<PCHAR>(const_cast<UCHAR*>(pBaseName));
                    ansiName.Length = ansiCount;
                    ansiName.MaximumLength = ansiCount;

                    unicodeName.Buffer = wideBuffer;
                    unicodeName.Length = 0u;
                    unicodeName.MaximumLength = sizeof(wideBuffer);

                    if (!NT_SUCCESS(RtlAnsiStringToUnicodeString(&unicodeName, &ansiName, FALSE))) continue;

                    AddModule(pModules[i].BasicInfo.ImageBase, pModules[i].ImageSize, &unicodeName);
                }

                status = STATUS_SUCCESS;

            done:

                if (pModules) {
                    ExFreePoolWithTag(pModules, driver::MEM_TAG);
                }

                if (!NT_SUCCESS(status) && ImageNotifyRegistered) {
                    PsRemoveLoadImageNotifyRoutine(OnImageLoad);
                    ImageNotifyRegistered = false;
                }

                return status;
            }


            void Delete() {

                if (ImageNotifyRegistered) {
                    PsRemoveLoadImageNotifyRoutine(OnImageLoad);
                    ImageNotifyRegistered = false;
                }

                if (!ModuleList.Flink) return;

                KIRQL oldIrql{};
                KeAcquireSpinLock(&ModuleListLock, &oldIrql);

                while (!IsListEmpty(&ModuleList)) {
                    LIST_ENTRY* const pListEntry = RemoveHeadList(&ModuleList);
                    KeReleaseSpinLock(&ModuleListLock, oldIrql);
                    ModuleEntry* const pEntry = CONTAINING_RECORD(pListEntry, ModuleEntry, list);
                    ExFreePoolWithTag(pEntry, driver::MEM_TAG);
                    KeAcquireSpinLock(&ModuleListLock, &oldIrql);
                }

                KeReleaseSpinLock(&ModuleListLock, oldIrql);

                return;
            }


            _Use_decl_annotations_
            void Resolve(
                const void* const* ppAddresses,
                ULONG count,
                protocol::StackFrame* pFrames
            ) {
                KIRQL oldIrql{};
                KeAcquireSpinLock(&ModuleListLock, &oldIrql);

                for (ULONG i = 0u; i < count; i++) {

                    if (!ppAddresses[i]) {
                        pFrames[i].moduleName[0] = L'\0';
                        pFrames[i].offset = 0ull;

                        continue;
                    }

                    ResolveAddress(ppAddresses[i], pFrames[i].moduleName, &pFrames[i].offset);
                }

                KeReleaseSpinLock(&ModuleListLock, oldIrql);

                return;
            }

        }

    }

}
