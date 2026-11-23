#include "name.h"

#include <fltKernel.h>
#include <ntstrsafe.h>

namespace {

    // this runs inside every paging read and RtlUnicodeStringPrintf reads pageable memory
    // the read that brings such a page in runs this again, so a status is written without it
    NTSTATUS AppendStatus(_Inout_ UNICODE_STRING* pName, _In_ NTSTATUS status) {
        constexpr ULONG STATUS_DIGIT_COUNT = 8u;
        constexpr WCHAR HEX_DIGITS[] = L"0123456789ABCDEF";
        WCHAR digits[STATUS_DIGIT_COUNT]{};
        ULONG value = static_cast<ULONG>(status);

        for (ULONG i = STATUS_DIGIT_COUNT; i > 0u; i--) {
            digits[i - 1u] = HEX_DIGITS[value & 0xFu];
            value >>= 4u;
        }

        UNICODE_STRING digitText{};
        digitText.Buffer = digits;
        digitText.Length = static_cast<USHORT>(sizeof(digits));
        digitText.MaximumLength = digitText.Length;

        return RtlUnicodeStringCat(pName, &digitText);
    }

}

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

                if (FsRtlIsPagingFile(pFltObjects->FileObject)) {
                    formatStatus = RtlUnicodeStringCopyString(pName, L"[name unavailable: paging file]");

                    goto done;
                }

                if (KeGetCurrentIrql() != PASSIVE_LEVEL) {
                    formatStatus = RtlUnicodeStringCopyString(pName, L"[name unavailable: above passive level]");

                    goto done;
                }

                normalizedStatus = FltGetFileNameInformation(pData, FLT_FILE_NAME_NORMALIZED | FLT_FILE_NAME_QUERY_ALWAYS_ALLOW_CACHE_LOOKUP, &pNormalizedInfo);

                if (NT_SUCCESS(normalizedStatus)) {

                #if DBG

                    FLT_ASSERT(NT_SUCCESS(FltParseFileNameInformation(pNormalizedInfo)));

                #endif // DBG

                    formatStatus = RtlUnicodeStringCopy(pName, &pNormalizedInfo->Name);

                    goto done;
                }

                openedStatus = FltGetFileNameInformation(pData, FLT_FILE_NAME_OPENED | FLT_FILE_NAME_QUERY_ALWAYS_ALLOW_CACHE_LOOKUP, &pOpenedInfo);

                if (NT_SUCCESS(openedStatus)) {
                    formatStatus = RtlUnicodeStringCopy(pName, &pOpenedInfo->Name);

                    if (NT_SUCCESS(formatStatus)) {
                        formatStatus = RtlUnicodeStringCatString(pName, L" [opened: normalize 0x");
                    }

                    if (NT_SUCCESS(formatStatus)) {
                        formatStatus = AppendStatus(pName, normalizedStatus);
                    }

                    if (NT_SUCCESS(formatStatus)) {
                        formatStatus = RtlUnicodeStringCatString(pName, L"]");
                    }

                    goto done;
                }

                formatStatus = RtlUnicodeStringCopyString(pName, L"[name unavailable: normalize 0x");

                if (NT_SUCCESS(formatStatus)) {
                    formatStatus = AppendStatus(pName, normalizedStatus);
                }

                if (NT_SUCCESS(formatStatus)) {
                    formatStatus = RtlUnicodeStringCatString(pName, L", opened 0x");
                }

                if (NT_SUCCESS(formatStatus)) {
                    formatStatus = AppendStatus(pName, openedStatus);
                }

                if (NT_SUCCESS(formatStatus)) {
                    formatStatus = RtlUnicodeStringCatString(pName, L"]");
                }

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

                if (KeGetCurrentIrql() != PASSIVE_LEVEL) return STATUS_UNSUCCESSFUL;

                const void* const pInfoBuffer = pData->Iopb->Parameters.SetFileInformation.InfoBuffer;

                if (!pInfoBuffer) return STATUS_INVALID_PARAMETER;

                HANDLE hRootDirectory = nullptr;
                WCHAR* pFileName = nullptr;
                ULONG fileNameSize = 0u;

                switch (pData->Iopb->Parameters.SetFileInformation.FileInformationClass) {

                    case FileRenameInformation:
                    case FileRenameInformationBypassAccessCheck:
                    case FileRenameInformationEx:
                    case FileRenameInformationExBypassAccessCheck: {
                        const FILE_RENAME_INFORMATION* const pRenameInfo = static_cast<const FILE_RENAME_INFORMATION*>(pInfoBuffer);
                        hRootDirectory = pRenameInfo->RootDirectory;
                        pFileName = const_cast<WCHAR*>(pRenameInfo->FileName);
                        fileNameSize = pRenameInfo->FileNameLength;

                        break;
                    }

                    case FileLinkInformation:
                    case FileLinkInformationBypassAccessCheck:
                    case FileLinkInformationEx:
                    case FileLinkInformationExBypassAccessCheck: {
                        const FILE_LINK_INFORMATION* const pLinkInfo = static_cast<const FILE_LINK_INFORMATION*>(pInfoBuffer);
                        hRootDirectory = pLinkInfo->RootDirectory;
                        pFileName = const_cast<WCHAR*>(pLinkInfo->FileName);
                        fileNameSize = pLinkInfo->FileNameLength;

                        break;
                    }

                    default:

                        return STATUS_INVALID_PARAMETER;

                }

                // copy source, defaulted to the raw, possibly relative name from the info buffer
                UNICODE_STRING sourceName{};
                sourceName.Buffer = pFileName;
                sourceName.Length = static_cast<USHORT>((fileNameSize < MAXUSHORT ? fileNameSize : MAXUSHORT) & ~1u);
                sourceName.MaximumLength = sourceName.Length;

                FLT_FILE_NAME_INFORMATION* pTargetInfo = nullptr;

                if (FLT_IS_IRP_OPERATION(pData) && NT_SUCCESS(FltGetDestinationFileNameInformation(pFltObjects->Instance, pFltObjects->FileObject, hRootDirectory, pFileName, fileNameSize, FLT_FILE_NAME_NORMALIZED | FLT_FILE_NAME_QUERY_DEFAULT, &pTargetInfo))) {
                    sourceName = pTargetInfo->Name;
                }

                const NTSTATUS status = RtlUnicodeStringCopy(pName, &sourceName);

                if (pTargetInfo) {
                    FltReleaseFileNameInformation(pTargetInfo);
                }

                return status;
            }

        }

    }

}
