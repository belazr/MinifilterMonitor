#include "quota.h"

#include "..\..\memory.h"

#include "..\..\..\..\inc\protocol.h"

#include <fltKernel.h>

namespace mimo {

    namespace trace {

        namespace supplement {

            namespace quota {

                __declspec(code_seg("PAGE"))
                _Use_decl_annotations_
                void PopulateList(protocol::QueryQuotaSupplement* pSupplement, const FLT_CALLBACK_DATA* pData) {
                    PAGED_CODE();

                    const void* const pSidList = pData->Iopb->Parameters.QueryQuota.SidList;
                    const ULONG bufferSize = pData->Iopb->Parameters.QueryQuota.SidListLength;

                    if (!pSidList || !bufferSize) return;

                    const ULONG copySize = bufferSize < protocol::QUERY_QUOTA_LIST_SIZE ? bufferSize : protocol::QUERY_QUOTA_LIST_SIZE;

                    RtlCopyMemory(pSupplement->list, pSidList, copySize);

                    if (copySize < bufferSize) {
                        pSupplement->captured |= protocol::QUERY_QUOTA_TRUNCATED_LIST;
                    }

                    pSupplement->capturedListSize = static_cast<uint32_t>(copySize);
                    pSupplement->captured |= protocol::QUERY_QUOTA_CAPTURED_LIST;

                    return;
                }


                __declspec(code_seg("PAGE"))
                _Use_decl_annotations_
                void PopulateSet(protocol::SetQuotaSupplement* pSupplement, const FLT_CALLBACK_DATA* pData) {
                    PAGED_CODE();

                    const ULONG bufferSize = pData->Iopb->Parameters.SetQuota.Length;

                    if (!bufferSize) return;

                    ULONG readableSize = bufferSize;
                    const void* pQuotaBuffer = memory::GetReadableBuffer(pData, pData->Iopb->Parameters.SetQuota.MdlAddress, pData->Iopb->Parameters.SetQuota.QuotaBuffer, &readableSize);

                    if (!pQuotaBuffer || !readableSize) return;

                    const ULONG copySize = readableSize < protocol::SET_QUOTA_PAYLOAD_SIZE ? readableSize : protocol::SET_QUOTA_PAYLOAD_SIZE;

                    __try {
                        RtlCopyMemory(pSupplement->payload, pQuotaBuffer, copySize);
                    }
                    __except (EXCEPTION_EXECUTE_HANDLER) {

                        return;
                    }

                    if (copySize < bufferSize) {
                        pSupplement->captured |= protocol::SET_QUOTA_TRUNCATED_PAYLOAD;
                    }

                    pSupplement->capturedSize = static_cast<uint32_t>(copySize);
                    pSupplement->captured |= protocol::SET_QUOTA_CAPTURED_PAYLOAD;

                    return;
                }


                __declspec(code_seg("PAGE"))
                _Use_decl_annotations_
                void PopulateQuery(protocol::QueryQuotaSupplement* pSupplement, const FLT_CALLBACK_DATA* pData) {
                    PAGED_CODE();

                    const ULONG bufferSize = pData->Iopb->Parameters.QueryQuota.Length;
                    const ULONG_PTR writtenSize = pData->IoStatus.Information;

                    if (!bufferSize || !writtenSize) return;

                    const ULONG dataSize = writtenSize < bufferSize ? static_cast<ULONG>(writtenSize) : bufferSize;
                    ULONG readableSize = dataSize;
                    const void* pQuotaBuffer = memory::GetReadableBuffer(pData, pData->Iopb->Parameters.QueryQuota.MdlAddress, pData->Iopb->Parameters.QueryQuota.QuotaBuffer, &readableSize);

                    if (!pQuotaBuffer || !readableSize) return;

                    const ULONG copySize = readableSize < protocol::QUERY_QUOTA_PAYLOAD_SIZE ? readableSize : protocol::QUERY_QUOTA_PAYLOAD_SIZE;

                    __try {
                        RtlCopyMemory(pSupplement->payload, pQuotaBuffer, copySize);
                    }
                    __except (EXCEPTION_EXECUTE_HANDLER) {

                        return;
                    }

                    if (copySize < dataSize) {
                        pSupplement->captured |= protocol::QUERY_QUOTA_TRUNCATED_PAYLOAD;
                    }

                    pSupplement->capturedPayloadSize = static_cast<uint32_t>(copySize);
                    pSupplement->captured |= protocol::QUERY_QUOTA_CAPTURED_PAYLOAD;

                    return;
                }

            }

        }

    }

}
