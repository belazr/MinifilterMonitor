#include "ea.h"

#include "..\..\memory.h"

#include "..\..\..\..\inc\protocol.h"

#include <fltKernel.h>

namespace mimo {

    namespace trace {

        namespace supplement {

            namespace ea {

                __declspec(code_seg("PAGE"))
                _Use_decl_annotations_
                void PopulateEaList(protocol::QueryEaSupplement* pSupplement, const FLT_CALLBACK_DATA* pData) {
                    PAGED_CODE();

                    const void* const pEaList = pData->Iopb->Parameters.QueryEa.EaList;
                    const ULONG bufferSize = pData->Iopb->Parameters.QueryEa.EaListLength;

                    if (!pEaList || !bufferSize) return;

                    const ULONG copySize = bufferSize < protocol::QUERY_EA_LIST_SIZE ? bufferSize : protocol::QUERY_EA_LIST_SIZE;

                    RtlCopyMemory(pSupplement->eaList, pEaList, copySize);

                    if (copySize < bufferSize) {
                        pSupplement->captured |= protocol::QUERY_EA_TRUNCATED_LIST;
                    }

                    pSupplement->capturedEaListSize = static_cast<uint32_t>(copySize);
                    pSupplement->captured |= protocol::QUERY_EA_CAPTURED_LIST;

                    return;
                }


                __declspec(code_seg("PAGE"))
                _Use_decl_annotations_
                void PopulateSet(protocol::SetEaSupplement* pSupplement, const FLT_CALLBACK_DATA* pData) {
                    PAGED_CODE();

                    const ULONG bufferSize = pData->Iopb->Parameters.SetEa.Length;

                    if (!bufferSize) return;

                    ULONG readableSize = bufferSize;
                    const void* pEaBuffer = memory::GetReadableBuffer(pData, pData->Iopb->Parameters.SetEa.MdlAddress, pData->Iopb->Parameters.SetEa.EaBuffer, &readableSize);

                    if (!pEaBuffer || !readableSize) return;

                    const ULONG copySize = readableSize < protocol::SET_EA_PAYLOAD_SIZE ? readableSize : protocol::SET_EA_PAYLOAD_SIZE;

                    __try {
                        RtlCopyMemory(pSupplement->payload, pEaBuffer, copySize);
                    }
                    __except (EXCEPTION_EXECUTE_HANDLER) {

                        return;
                    }

                    if (copySize < bufferSize) {
                        pSupplement->captured |= protocol::SET_EA_TRUNCATED_PAYLOAD;
                    }

                    pSupplement->capturedSize = static_cast<uint32_t>(copySize);
                    pSupplement->captured |= protocol::SET_EA_CAPTURED_PAYLOAD;

                    return;
                }


                __declspec(code_seg("PAGE"))
                _Use_decl_annotations_
                void PopulateQuery(protocol::QueryEaSupplement* pSupplement, const FLT_CALLBACK_DATA* pData) {
                    PAGED_CODE();

                    const ULONG bufferSize = pData->Iopb->Parameters.QueryEa.Length;
                    const ULONG_PTR writtenSize = pData->IoStatus.Information;

                    if (!bufferSize || !writtenSize) return;

                    const ULONG dataSize = writtenSize < bufferSize ? static_cast<ULONG>(writtenSize) : bufferSize;
                    ULONG readableSize = dataSize;
                    const void* pEaBuffer = memory::GetReadableBuffer(pData, pData->Iopb->Parameters.QueryEa.MdlAddress, pData->Iopb->Parameters.QueryEa.EaBuffer, &readableSize);

                    if (!pEaBuffer || !readableSize) return;

                    const ULONG copySize = readableSize < protocol::QUERY_EA_PAYLOAD_SIZE ? readableSize : protocol::QUERY_EA_PAYLOAD_SIZE;

                    __try {
                        RtlCopyMemory(pSupplement->payload, pEaBuffer, copySize);
                    }
                    __except (EXCEPTION_EXECUTE_HANDLER) {

                        return;
                    }

                    if (copySize < dataSize) {
                        pSupplement->captured |= protocol::QUERY_EA_TRUNCATED_PAYLOAD;
                    }

                    pSupplement->capturedPayloadSize = static_cast<uint32_t>(copySize);
                    pSupplement->captured |= protocol::QUERY_EA_CAPTURED_PAYLOAD;

                    return;
                }

            }

        }

    }

}
