#include "security.h"

#include "..\..\memory.h"

#include "..\..\..\..\inc\protocol.h"

#include <fltKernel.h>

namespace mimo {

    namespace trace {

        namespace supplement {

            namespace security {

                __declspec(code_seg("PAGE"))
                _Use_decl_annotations_
                void PopulateSet(protocol::SecuritySupplement* pSupplement, const FLT_CALLBACK_DATA* pData) {
                    PAGED_CODE();

                    PSECURITY_DESCRIPTOR const pSecurityDescriptor = pData->Iopb->Parameters.SetSecurity.SecurityDescriptor;
                    ULONG size = 0u;

                    if (!pSecurityDescriptor) return;

                    // absolute and self-relative layouts agree on Control
                    if (static_cast<const SECURITY_DESCRIPTOR_RELATIVE*>(pSecurityDescriptor)->Control & SE_SELF_RELATIVE) {
                        size = RtlLengthSecurityDescriptor(pSecurityDescriptor);

                        if (!size || size > protocol::SECURITY_PAYLOAD_BYTES) return;

                        RtlCopyMemory(pSupplement->payload, pSecurityDescriptor, size);
                    }
                    else {
                        size = protocol::SECURITY_PAYLOAD_BYTES;

                        if (!NT_SUCCESS(RtlAbsoluteToSelfRelativeSD(pSecurityDescriptor, pSupplement->payload, &size))) return;

                    }

                    if (!RtlValidRelativeSecurityDescriptor(pSupplement->payload, size, 0u)) return;

                    pSupplement->capturedBytes = static_cast<uint32_t>(size);
                    pSupplement->captured |= protocol::SECURITY_CAPTURED_PAYLOAD;

                    return;
                }


                __declspec(code_seg("PAGE"))
                _Use_decl_annotations_
                void PopulateQuery(protocol::SecuritySupplement* pSupplement, const FLT_CALLBACK_DATA* pData) {
                    PAGED_CODE();

                    ULONG bufferSize = pData->Iopb->Parameters.QuerySecurity.Length;
                    const ULONG_PTR writtenSize = pData->IoStatus.Information;

                    if (!bufferSize || !writtenSize) return;

                    const void* pSecurityBuffer = memory::GetReadableBuffer(pData, pData->Iopb->Parameters.QuerySecurity.MdlAddress, pData->Iopb->Parameters.QuerySecurity.SecurityBuffer, &bufferSize);

                    if (!pSecurityBuffer || !bufferSize) return;

                    const ULONG dataSize = writtenSize < bufferSize ? static_cast<ULONG>(writtenSize) : bufferSize;

                    if (dataSize > protocol::SECURITY_PAYLOAD_BYTES) return;

                    __try {
                        RtlCopyMemory(pSupplement->payload, pSecurityBuffer, dataSize);
                    }
                    __except (EXCEPTION_EXECUTE_HANDLER) {

                        return;
                    }

                    if (!RtlValidRelativeSecurityDescriptor(pSupplement->payload, dataSize, 0u)) return;

                    pSupplement->capturedBytes = static_cast<uint32_t>(dataSize);
                    pSupplement->captured |= protocol::SECURITY_CAPTURED_PAYLOAD;

                    return;
                }

            }

        }

    }

}
