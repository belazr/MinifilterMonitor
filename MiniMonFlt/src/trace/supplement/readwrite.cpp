#include "readwrite.h"

#include "..\..\memory.h"

#include "..\..\..\..\inc\protocol.h"

#include <fltKernel.h>

namespace mimo {

    namespace trace {

        namespace supplement {

            namespace readwrite {

                _Use_decl_annotations_
                void PopulateWrite(protocol::ReadWriteSupplement* pSupplement, const FLT_CALLBACK_DATA* pData) {
                    const ULONG bufferSize = pData->Iopb->Parameters.Write.Length;

                    if (!bufferSize) return;

                    ULONG readableSize = bufferSize;

                    // a paging buffer is read through its MDL only
                    const void* const pRawBuffer = pData->Iopb->IrpFlags & IRP_PAGING_IO ? nullptr : pData->Iopb->Parameters.Write.WriteBuffer;
                    const void* const pWriteBuffer = memory::GetReadableBuffer(pData, pData->Iopb->Parameters.Write.MdlAddress, pRawBuffer, &readableSize);

                    if (!pWriteBuffer || !readableSize) return;

                    const ULONG copySize = readableSize < sizeof(pSupplement->payload) ? readableSize : sizeof(pSupplement->payload);

                    if (!memory::CopyReadableBuffer(pSupplement->payload, pWriteBuffer, copySize)) return;

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

                    // a paging buffer is read through its MDL only
                    const void* const pRawBuffer = pData->Iopb->IrpFlags & IRP_PAGING_IO ? nullptr : pData->Iopb->Parameters.Read.ReadBuffer;
                    const void* const pReadBuffer = memory::GetReadableBuffer(pData, pData->Iopb->Parameters.Read.MdlAddress, pRawBuffer, &readableSize);

                    if (!pReadBuffer || !readableSize) return;

                    const ULONG copySize = readableSize < sizeof(pSupplement->payload) ? readableSize : sizeof(pSupplement->payload);

                    if (!memory::CopyReadableBuffer(pSupplement->payload, pReadBuffer, copySize)) return;

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
