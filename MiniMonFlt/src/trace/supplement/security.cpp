#include "security.h"

#include "..\..\memory.h"

#include "..\..\..\..\inc\protocol.h"

#include <fltKernel.h>

namespace mimo {

    namespace trace {

        namespace supplement {

            namespace security {

                _Use_decl_annotations_
                void PopulateSet(protocol::SecuritySupplement* pSupplement, const FLT_CALLBACK_DATA* pData) {

                    if (KeGetCurrentIrql() != PASSIVE_LEVEL) return;

                    PSECURITY_DESCRIPTOR const pSecurityDescriptor = pData->Iopb->Parameters.SetSecurity.SecurityDescriptor;
                    ULONG size = 0u;

                    if (!pSecurityDescriptor) return;

                    // absolute and self-relative layouts agree on Control
                    if (static_cast<const SECURITY_DESCRIPTOR_RELATIVE*>(pSecurityDescriptor)->Control & SE_SELF_RELATIVE) {
                        size = RtlLengthSecurityDescriptor(pSecurityDescriptor);

                        if (!size || size > protocol::SECURITY_PAYLOAD_SIZE) return;

                        RtlCopyMemory(pSupplement->payload, pSecurityDescriptor, size);
                    }
                    else {
                        size = protocol::SECURITY_PAYLOAD_SIZE;

                        if (!NT_SUCCESS(RtlAbsoluteToSelfRelativeSD(pSecurityDescriptor, pSupplement->payload, &size))) return;

                    }

                    if (!RtlValidRelativeSecurityDescriptor(pSupplement->payload, size, 0u)) return;

                    pSupplement->capturedSize = static_cast<uint32_t>(size);
                    pSupplement->captured |= protocol::SECURITY_CAPTURED_PAYLOAD;

                    return;
                }


                _Use_decl_annotations_
                void PopulateQuery(protocol::SecuritySupplement* pSupplement, const FLT_CALLBACK_DATA* pData) {

                    if (KeGetCurrentIrql() >= DISPATCH_LEVEL) return;

                    const ULONG bufferSize = pData->Iopb->Parameters.QuerySecurity.Length;
                    const ULONG_PTR writtenSize = pData->IoStatus.Information;

                    if (!bufferSize || !writtenSize) return;

                    ULONG readableSize = bufferSize;
                    const void* pSecurityBuffer = memory::GetReadableBuffer(pData, pData->Iopb->Parameters.QuerySecurity.MdlAddress, pData->Iopb->Parameters.QuerySecurity.SecurityBuffer, &readableSize);

                    if (!pSecurityBuffer || !readableSize) return;

                    const ULONG copySize = writtenSize < readableSize ? static_cast<ULONG>(writtenSize) : readableSize;

                    if (copySize > protocol::SECURITY_PAYLOAD_SIZE) return;

                    __try {
                        RtlCopyMemory(pSupplement->payload, pSecurityBuffer, copySize);
                    }
                    __except (EXCEPTION_EXECUTE_HANDLER) {

                        return;
                    }

                    if (!RtlValidRelativeSecurityDescriptor(pSupplement->payload, copySize, 0u)) return;

                    pSupplement->capturedSize = static_cast<uint32_t>(copySize);
                    pSupplement->captured |= protocol::SECURITY_CAPTURED_PAYLOAD;

                    return;
                }

            }

        }

    }

}
