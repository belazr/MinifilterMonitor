#include "config.h"

#include <fltKernel.h>
#include <ntstrsafe.h>

using namespace mimo;

namespace {

    constexpr ULONG DEFAULT_MAX_MEMORY_KB = 50u * 1024u;
    ULONG MaxMemoryKb = DEFAULT_MAX_MEMORY_KB;

}

namespace mimo {

    namespace config {

        __declspec(code_seg("INIT"))
        _Use_decl_annotations_
        void Create(const UNICODE_STRING* pRegistryPath) {
            constexpr ULONG PATH_WCHAR_COUNT = 256u;
            constexpr ULONG TABLE_ENTRY_COUNT = 2u;
            WCHAR path[PATH_WCHAR_COUNT]{};
            RTL_QUERY_REGISTRY_TABLE table[TABLE_ENTRY_COUNT]{};

            if (!NT_SUCCESS(RtlStringCchPrintfW(path, PATH_WCHAR_COUNT, L"%wZ\\Parameters", pRegistryPath))) return;

            table[0u].Flags        = RTL_QUERY_REGISTRY_DIRECT | RTL_QUERY_REGISTRY_TYPECHECK;
            table[0u].Name         = const_cast<WCHAR*>(L"MaxMemoryKB");
            table[0u].EntryContext = &MaxMemoryKb;
            table[0u].DefaultType  = (REG_DWORD << RTL_QUERY_REGISTRY_TYPECHECK_SHIFT) | REG_NONE;

            RtlQueryRegistryValues(RTL_REGISTRY_ABSOLUTE, path, table, nullptr, nullptr);

            return;
        }


        ULONG GetMaxMemoryKb() {

            return MaxMemoryKb;
        }

    }

}
