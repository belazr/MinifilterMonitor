#include "directory.h"

#include "..\..\memory.h"

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

                    const UNICODE_STRING* const pFileName = pData->Iopb->Parameters.DirectoryControl.QueryDirectory.FileName;

                    if (!pFileName) return;

                    UNICODE_STRING fileName{};
                    RtlInitEmptyUnicodeString(&fileName, pSupplement->fileName, static_cast<USHORT>(sizeof(pSupplement->fileName)));

                    const NTSTATUS status = RtlUnicodeStringCopy(&fileName, pFileName);

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

                    if (!bufferSize || !writtenSize) return;

                    const ULONG dataSize = writtenSize < bufferSize ? static_cast<ULONG>(writtenSize) : bufferSize;
                    ULONG readableSize = dataSize;
                    const void* pDirectoryBuffer = memory::GetReadableBuffer(pData, pData->Iopb->Parameters.DirectoryControl.QueryDirectory.MdlAddress, pData->Iopb->Parameters.DirectoryControl.QueryDirectory.DirectoryBuffer, &readableSize);

                    if (!pDirectoryBuffer || !readableSize) return;

                    const ULONG copySize = readableSize < protocol::QUERY_DIRECTORY_PAYLOAD_BYTES ? readableSize : protocol::QUERY_DIRECTORY_PAYLOAD_BYTES;

                    __try {
                        RtlCopyMemory(pSupplement->payload, pDirectoryBuffer, copySize);
                    }
                    __except (EXCEPTION_EXECUTE_HANDLER) {

                        return;
                    }

                    if (copySize < dataSize) {
                        pSupplement->captured |= protocol::QUERY_DIRECTORY_TRUNCATED_PAYLOAD;
                    }

                    pSupplement->capturedBytes = static_cast<uint32_t>(copySize);
                    pSupplement->captured |= protocol::QUERY_DIRECTORY_CAPTURED_PAYLOAD;

                    return;
                }

            }

        }

    }

}
