#include "name.h"

#include <fltKernel.h>
#include <ntstrsafe.h>

namespace mimo {

    namespace name {

        _Use_decl_annotations_
        NTSTATUS FormatFileName(
            FLT_CALLBACK_DATA* pData,
            const FLT_RELATED_OBJECTS* pFltObjects,
            UNICODE_STRING* pName
        ) {
            FLT_FILE_NAME_INFORMATION* pNormalizedInfo = nullptr;
            FLT_FILE_NAME_INFORMATION* pOpenedInfo = nullptr;
            NTSTATUS normalizedStatus = STATUS_UNSUCCESSFUL;
            NTSTATUS openedStatus = STATUS_UNSUCCESSFUL;
            NTSTATUS formatStatus = STATUS_UNSUCCESSFUL;

            if (!pFltObjects->FileObject) {
                formatStatus = RtlUnicodeStringCopyString(pName, L"[no file name]");

                goto done;
            }

            normalizedStatus = FltGetFileNameInformation(pData, FLT_FILE_NAME_NORMALIZED | FLT_FILE_NAME_QUERY_ALWAYS_ALLOW_CACHE_LOOKUP, &pNormalizedInfo);

            if (NT_SUCCESS(normalizedStatus)) {

            #if DBG

                FLT_ASSERT(NT_SUCCESS(FltParseFileNameInformation(pNormalizedInfo)));

            #endif // DBG

                formatStatus = RtlUnicodeStringPrintf(pName, L"%wZ", &pNormalizedInfo->Name);

                goto done;
            }

            openedStatus = FltGetFileNameInformation(pData, FLT_FILE_NAME_OPENED | FLT_FILE_NAME_QUERY_ALWAYS_ALLOW_CACHE_LOOKUP, &pOpenedInfo);

            if (NT_SUCCESS(openedStatus)) {
                formatStatus = RtlUnicodeStringPrintf(pName, L"%wZ [opened: normalize 0x%08X]", &pOpenedInfo->Name, normalizedStatus);

                goto done;
            }

            formatStatus = RtlUnicodeStringPrintf(pName, L"[name unavailable: normalize 0x%08X, opened 0x%08X]", normalizedStatus, openedStatus);

        done:

            if (pNormalizedInfo) {
                FltReleaseFileNameInformation(pNormalizedInfo);
            }

            if (pOpenedInfo) {
                FltReleaseFileNameInformation(pOpenedInfo);
            }

            return formatStatus;
        }

    }

}
