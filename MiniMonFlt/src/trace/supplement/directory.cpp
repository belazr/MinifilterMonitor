#include "directory.h"

#include "..\..\..\..\inc\protocol.h"

#include <fltKernel.h>
#include <ntstrsafe.h>

namespace mimo {

    namespace trace {

        namespace supplement {

            namespace directory {

                __declspec(code_seg("PAGE"))
                _Use_decl_annotations_
                void PopulateFileName(protocol::QueryDirectorySupplement* pSupplement, const FLT_CALLBACK_DATA* pData) {
                    PAGED_CODE();

                    UNICODE_STRING fileName{};
                    RtlInitEmptyUnicodeString(&fileName, pSupplement->fileName, sizeof(pSupplement->fileName));

                    NTSTATUS status = STATUS_UNSUCCESSFUL;

                    __try {
                        status = RtlUnicodeStringCopy(&fileName, pData->Iopb->Parameters.DirectoryControl.QueryDirectory.FileName);
                    }
                    __except (EXCEPTION_EXECUTE_HANDLER) {

                        return;
                    }

                    if (!NT_SUCCESS(status) && status != STATUS_BUFFER_OVERFLOW) return;

                    if (status == STATUS_BUFFER_OVERFLOW) {
                        pSupplement->captured |= protocol::QUERY_DIRECTORY_TRUNCATED_FILE_NAME;
                    }

                    pSupplement->captured |= protocol::QUERY_DIRECTORY_CAPTURED_FILE_NAME;

                    return;
                }


                __declspec(code_seg("PAGE"))
                _Use_decl_annotations_
                void PopulatePayload(protocol::QueryDirectorySupplement* pSupplement, const FLT_CALLBACK_DATA* pData) {
                    PAGED_CODE();

                    const ULONG bufferSize = pData->Iopb->Parameters.DirectoryControl.QueryDirectory.Length;
                    const ULONG_PTR writtenSize = pData->IoStatus.Information;
                    const ULONG dataSize = writtenSize < bufferSize ? static_cast<ULONG>(writtenSize) : bufferSize;
                    const ULONG copySize = dataSize < protocol::QUERY_DIRECTORY_PAYLOAD_BYTES ? dataSize : protocol::QUERY_DIRECTORY_PAYLOAD_BYTES;

                    __try {
                        RtlCopyMemory(pSupplement->payload, pData->Iopb->Parameters.DirectoryControl.QueryDirectory.DirectoryBuffer, copySize);
                    }
                    __except (EXCEPTION_EXECUTE_HANDLER) {

                        return;
                    }

                    pSupplement->capturedBytes = copySize;
                    pSupplement->captured |= protocol::QUERY_DIRECTORY_CAPTURED_PAYLOAD;

                    return;
                }

            }

        }

    }

}
