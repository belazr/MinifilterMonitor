#include "volume.h"

#include "..\..\..\..\inc\protocol.h"

#include <fltKernel.h>

namespace mimo {

    namespace trace {

        namespace supplement {

            namespace volume {

                __declspec(code_seg("PAGE"))
                _Use_decl_annotations_
                void PopulateSet(protocol::VolumeInfoSupplement* pSupplement, const FLT_CALLBACK_DATA* pData) {
                    PAGED_CODE();

                    const ULONG size = pData->Iopb->Parameters.SetVolumeInformation.Length;
                    const ULONG copySize = size < protocol::VOLUME_INFO_PAYLOAD_BYTES ? size : protocol::VOLUME_INFO_PAYLOAD_BYTES;

                    __try {
                        RtlCopyMemory(pSupplement->payload, pData->Iopb->Parameters.SetVolumeInformation.VolumeBuffer, copySize);
                    }
                    __except (EXCEPTION_EXECUTE_HANDLER) {

                        return;
                    }

                    pSupplement->capturedBytes = static_cast<uint32_t>(copySize);
                    pSupplement->captured |= protocol::VOLUME_INFO_CAPTURED_PAYLOAD;

                    return;
                }


                __declspec(code_seg("PAGE"))
                _Use_decl_annotations_
                void PopulateQuery(protocol::VolumeInfoSupplement* pSupplement, const FLT_CALLBACK_DATA* pData) {
                    PAGED_CODE();

                    if (!pData->IoStatus.Information) return;

                    const ULONG bufferSize = pData->Iopb->Parameters.QueryVolumeInformation.Length;
                    const ULONG_PTR writtenSize = pData->IoStatus.Information;
                    const ULONG dataSize = writtenSize < bufferSize ? static_cast<ULONG>(writtenSize) : bufferSize;
                    const ULONG copySize = dataSize < protocol::VOLUME_INFO_PAYLOAD_BYTES ? dataSize : protocol::VOLUME_INFO_PAYLOAD_BYTES;

                    __try {
                        RtlCopyMemory(pSupplement->payload, pData->Iopb->Parameters.QueryVolumeInformation.VolumeBuffer, copySize);
                    }
                    __except (EXCEPTION_EXECUTE_HANDLER) {

                        return;
                    }

                    pSupplement->capturedBytes = static_cast<uint32_t>(copySize);
                    pSupplement->captured |= protocol::VOLUME_INFO_CAPTURED_PAYLOAD;

                    return;
                }

            }

        }

    }

}
