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

                    const PSECURITY_DESCRIPTOR pSecurityDescriptor = pData->Iopb->Parameters.SetSecurity.SecurityDescriptor;

                    if (!pSecurityDescriptor) return;

                    ULONG size = 0u;

                    // absolute and self-relative layouts agree on Control
                    if (static_cast<const SECURITY_DESCRIPTOR_RELATIVE*>(pSecurityDescriptor)->Control & SE_SELF_RELATIVE) {
                        size = RtlLengthSecurityDescriptor(pSecurityDescriptor);

                        if (!size) return;

                        if (size > sizeof(pSupplement->payload)) return;

                        RtlCopyMemory(pSupplement->payload, pSecurityDescriptor, size);
                    }
                    else {
                        size = sizeof(pSupplement->payload);

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

                    if (!bufferSize || !writtenSize || writtenSize > bufferSize || pData->IoStatus.Status == STATUS_BUFFER_OVERFLOW) return;

                    if (writtenSize > sizeof(pSupplement->payload)) return;

                    ULONG copySize = static_cast<ULONG>(writtenSize);
                    const void* const pSecurityBuffer = memory::GetReadableBuffer(pData, pData->Iopb->Parameters.QuerySecurity.MdlAddress, pData->Iopb->Parameters.QuerySecurity.SecurityBuffer, &copySize);

                    if (!pSecurityBuffer || !copySize) return;

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
