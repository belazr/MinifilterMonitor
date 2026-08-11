#include "info.h"

#include "..\name.h"

#include "..\..\..\..\inc\protocol.h"

#include <fltKernel.h>

using namespace mimo;

namespace {

    __declspec(code_seg("PAGE"))
    void PopulateTargetName(
        _Inout_ protocol::SetInfoSupplement* pSupplement,
        _In_ FLT_CALLBACK_DATA* pData,
        _In_ const FLT_RELATED_OBJECTS* pFltObjects
    ) {
        PAGED_CODE();

        UNICODE_STRING targetName{};
        RtlInitEmptyUnicodeString(&targetName, pSupplement->targetName, static_cast<USHORT>(sizeof(pSupplement->targetName)));

        const NTSTATUS status = trace::name::FormatTargetFileName(pData, pFltObjects, &targetName);

        if (!NT_SUCCESS(status) && status != STATUS_BUFFER_OVERFLOW) return;

        if (status == STATUS_BUFFER_OVERFLOW) {
            pSupplement->captured |= protocol::SET_INFO_TRUNCATED_TARGET_NAME;
        }

        pSupplement->captured |= protocol::SET_INFO_CAPTURED_TARGET_NAME;

        return;
    }

}

namespace mimo {

    namespace trace {

        namespace supplement {

            namespace info {

                __declspec(code_seg("PAGE"))
                _Use_decl_annotations_
                void PopulateSet(
                    protocol::SetInfoSupplement* pSupplement,
                    FLT_CALLBACK_DATA* pData,
                    const FLT_RELATED_OBJECTS* pFltObjects
                ) {
                    PAGED_CODE();

                    const ULONG size = pData->Iopb->Parameters.SetFileInformation.Length;
                    const ULONG copySize = size < protocol::SET_INFO_PAYLOAD_BYTES ? size : protocol::SET_INFO_PAYLOAD_BYTES;

                    __try {
                        RtlCopyMemory(pSupplement->payload, pData->Iopb->Parameters.SetFileInformation.InfoBuffer, copySize);
                    }
                    __except (EXCEPTION_EXECUTE_HANDLER) {

                        return;
                    }

                    pSupplement->capturedBytes = static_cast<uint32_t>(copySize);
                    pSupplement->captured |= protocol::SET_INFO_CAPTURED_PAYLOAD;

                    switch (pData->Iopb->Parameters.SetFileInformation.FileInformationClass) {

                        case FileRenameInformation:
                        case FileLinkInformation:
                        case FileRenameInformationBypassAccessCheck:
                        case FileLinkInformationBypassAccessCheck:
                        case FileRenameInformationEx:
                        case FileRenameInformationExBypassAccessCheck:
                        case FileLinkInformationEx:
                        case FileLinkInformationExBypassAccessCheck:
                            PopulateTargetName(pSupplement, pData, pFltObjects);

                            break;

                        default:

                            break;

                    }

                    return;
                }


                __declspec(code_seg("PAGE"))
                _Use_decl_annotations_
                void PopulateQuery(protocol::QueryInfoSupplement* pSupplement, const FLT_CALLBACK_DATA* pData) {
                    PAGED_CODE();

                    const ULONG bufferSize = pData->Iopb->Parameters.QueryFileInformation.Length;
                    const ULONG_PTR writtenSize = pData->IoStatus.Information;
                    const ULONG dataSize = writtenSize < bufferSize ? static_cast<ULONG>(writtenSize) : bufferSize;
                    const ULONG copySize = dataSize < protocol::QUERY_INFO_PAYLOAD_BYTES ? dataSize : protocol::QUERY_INFO_PAYLOAD_BYTES;

                    __try {
                        RtlCopyMemory(pSupplement->payload, pData->Iopb->Parameters.QueryFileInformation.InfoBuffer, copySize);
                    }
                    __except (EXCEPTION_EXECUTE_HANDLER) {

                        return;
                    }

                    pSupplement->capturedBytes = static_cast<uint32_t>(copySize);
                    pSupplement->captured |= protocol::QUERY_INFO_CAPTURED_PAYLOAD;

                    return;
                }

            }

        }

    }

}
