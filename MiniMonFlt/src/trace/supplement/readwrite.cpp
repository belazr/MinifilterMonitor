#include "readwrite.h"

#include "..\..\memory.h"

#include "..\..\..\..\inc\protocol.h"

#include <fltKernel.h>

namespace mimo {

    namespace trace {

        namespace supplement {

            namespace readwrite {

                __declspec(code_seg("PAGE"))
                _Use_decl_annotations_
                void PopulateWrite(protocol::ReadWriteSupplement* pSupplement, const FLT_CALLBACK_DATA* pData) {
                    PAGED_CODE();

                    const ULONG bufferSize = pData->Iopb->Parameters.Write.Length;

                    if (!bufferSize) return;

                    ULONG readableSize = bufferSize;
                    const void* pWriteBuffer = memory::GetReadableBuffer(pData, pData->Iopb->Parameters.Write.MdlAddress, pData->Iopb->Parameters.Write.WriteBuffer, &readableSize);

                    if (!pWriteBuffer || !readableSize) return;

                    const ULONG copySize = readableSize < protocol::READ_WRITE_PAYLOAD_SIZE ? readableSize : protocol::READ_WRITE_PAYLOAD_SIZE;

                    __try {
                        RtlCopyMemory(pSupplement->payload, pWriteBuffer, copySize);
                    }
                    __except (EXCEPTION_EXECUTE_HANDLER) {

                        return;
                    }

                    if (copySize < bufferSize) {
                        pSupplement->captured |= protocol::READ_WRITE_TRUNCATED_PAYLOAD;
                    }

                    pSupplement->capturedSize = static_cast<uint32_t>(copySize);
                    pSupplement->captured |= protocol::READ_WRITE_CAPTURED_PAYLOAD;

                    return;
                }


                _Use_decl_annotations_
                void PopulateRead(protocol::ReadWriteSupplement* pSupplement, const FLT_CALLBACK_DATA* pData) {
                    const ULONG bufferSize = pData->Iopb->Parameters.Read.Length;
                    const ULONG_PTR writtenSize = pData->IoStatus.Information;

                    if (!bufferSize || !writtenSize) return;

                    const ULONG dataSize = writtenSize < bufferSize ? static_cast<ULONG>(writtenSize) : bufferSize;
                    ULONG readableSize = dataSize;
                    const void* pReadBuffer = memory::GetReadableBuffer(pData, pData->Iopb->Parameters.Read.MdlAddress, pData->Iopb->Parameters.Read.ReadBuffer, &readableSize);

                    if (!pReadBuffer || !readableSize) return;

                    const ULONG copySize = readableSize < protocol::READ_WRITE_PAYLOAD_SIZE ? readableSize : protocol::READ_WRITE_PAYLOAD_SIZE;

                    __try {
                        RtlCopyMemory(pSupplement->payload, pReadBuffer, copySize);
                    }
                    __except (EXCEPTION_EXECUTE_HANDLER) {

                        return;
                    }

                    if (copySize < dataSize) {
                        pSupplement->captured |= protocol::READ_WRITE_TRUNCATED_PAYLOAD;
                    }

                    pSupplement->capturedSize = static_cast<uint32_t>(copySize);
                    pSupplement->captured |= protocol::READ_WRITE_CAPTURED_PAYLOAD;

                    return;
                }

            }

        }

    }

}
