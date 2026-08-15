#include "name.h"

#include <fltKernel.h>
#include <ntstrsafe.h>

namespace mimo {

    namespace trace {

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

                if (KeGetCurrentIrql() >= DISPATCH_LEVEL) {
                    formatStatus = RtlUnicodeStringCopyString(pName, L"[name unavailable: dispatch level]");

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


            __declspec(code_seg("PAGE"))
            _Use_decl_annotations_
            NTSTATUS FormatTargetFileName(
                FLT_CALLBACK_DATA* pData,
                const FLT_RELATED_OBJECTS* pFltObjects,
                UNICODE_STRING* pName
            ) {
                PAGED_CODE();

                // rename and link layouts agree on RootDirectory, FileNameLength and FileName
                const FILE_RENAME_INFORMATION* const pInfo = static_cast<const FILE_RENAME_INFORMATION*>(pData->Iopb->Parameters.SetFileInformation.InfoBuffer);
                HANDLE rootDirectory = nullptr;
                PWSTR pFileName = nullptr;
                ULONG fileNameSize = 0u;

                __try {
                    rootDirectory = pInfo->RootDirectory;
                    pFileName = const_cast<PWSTR>(pInfo->FileName);
                    fileNameSize = pInfo->FileNameLength;
                }
                __except (EXCEPTION_EXECUTE_HANDLER) {

                    return GetExceptionCode();
                }

                // copy source, defaulted to the raw, possibly relative name from the info buffer
                UNICODE_STRING sourceName{};
                sourceName.Buffer = pFileName;
                sourceName.Length = static_cast<USHORT>((fileNameSize < MAXUSHORT ? fileNameSize : MAXUSHORT) & ~1u);
                sourceName.MaximumLength = sourceName.Length;

                FLT_FILE_NAME_INFORMATION* pTargetInfo = nullptr;

                if (KeGetCurrentIrql() == PASSIVE_LEVEL && FLT_IS_IRP_OPERATION(pData) && NT_SUCCESS(FltGetDestinationFileNameInformation(pFltObjects->Instance, pFltObjects->FileObject, rootDirectory, pFileName, fileNameSize, FLT_FILE_NAME_NORMALIZED | FLT_FILE_NAME_QUERY_DEFAULT, &pTargetInfo))) {
                    sourceName = pTargetInfo->Name;
                }

                NTSTATUS copyStatus = STATUS_UNSUCCESSFUL;

                __try {
                    copyStatus = RtlUnicodeStringCopy(pName, &sourceName);
                }
                __except (EXCEPTION_EXECUTE_HANDLER) {
                    copyStatus = GetExceptionCode();
                }

                if (pTargetInfo) {
                    FltReleaseFileNameInformation(pTargetInfo);
                }

                return copyStatus;
            }

        }

    }

}
