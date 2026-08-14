#include "directory.h"

#include "..\..\mdl.h"

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
                    RtlInitEmptyUnicodeString(&fileName, pSupplement->fileName, static_cast<USHORT>(sizeof(pSupplement->fileName)));

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

                    if (!pData->IoStatus.Information) return;

                    ULONG bufferSize = pData->Iopb->Parameters.DirectoryControl.QueryDirectory.Length;
                    const void* pDirectoryBuffer = mdl::Map(pData->Iopb->Parameters.DirectoryControl.QueryDirectory.MdlAddress, pData->Iopb->Parameters.DirectoryControl.QueryDirectory.DirectoryBuffer, &bufferSize);
                    bool isRawUserBuffer = false;

                    if (!pDirectoryBuffer) {
                        const void* pRawBuffer = pData->Iopb->Parameters.DirectoryControl.QueryDirectory.DirectoryBuffer;

                        if (pData->RequestorMode == KernelMode && reinterpret_cast<ULONG_PTR>(pRawBuffer) >= reinterpret_cast<ULONG_PTR>(MmSystemRangeStart)) {
                            pDirectoryBuffer = pRawBuffer;
                        }
                        else if (pData->Thread && IoThreadToProcess(pData->Thread) == PsGetCurrentProcess()) {
                            pDirectoryBuffer = pRawBuffer;
                            isRawUserBuffer = true;
                        }
                    }

                    if (!pDirectoryBuffer) return;

                    const ULONG_PTR writtenSize = pData->IoStatus.Information;
                    const ULONG dataSize = writtenSize < bufferSize ? static_cast<ULONG>(writtenSize) : bufferSize;
                    const ULONG copySize = dataSize < protocol::QUERY_DIRECTORY_PAYLOAD_BYTES ? dataSize : protocol::QUERY_DIRECTORY_PAYLOAD_BYTES;

                    __try {
                        if (isRawUserBuffer) {
                            ProbeForRead(const_cast<void*>(pDirectoryBuffer), copySize, 1u);
                        }

                        RtlCopyMemory(pSupplement->payload, pDirectoryBuffer, copySize);
                    }
                    __except (EXCEPTION_EXECUTE_HANDLER) {

                        return;
                    }

                    pSupplement->capturedBytes = static_cast<uint32_t>(copySize);
                    pSupplement->captured |= protocol::QUERY_DIRECTORY_CAPTURED_PAYLOAD;

                    return;
                }

            }

        }

    }

}
