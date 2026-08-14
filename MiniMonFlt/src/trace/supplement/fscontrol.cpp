#include "fscontrol.h"

#include "..\..\mdl.h"

#include "..\..\..\..\inc\protocol.h"

#include <fltKernel.h>

using namespace mimo;

namespace {

    __declspec(code_seg("PAGE"))
    void PopulateSecondInput(_Inout_ protocol::FsControlSupplement* pSupplement, _In_ const FLT_CALLBACK_DATA* pData) {
        PAGED_CODE();

        ULONG dataSize = pData->Iopb->Parameters.FileSystemControl.Common.OutputBufferLength;
        const void* pSecondInput = mdl::Map(pData->Iopb->Parameters.FileSystemControl.Direct.OutputMdlAddress, pData->Iopb->Parameters.FileSystemControl.Direct.OutputBuffer, &dataSize);

        if (!pSecondInput || !dataSize) return;

        const ULONG copySize = dataSize < protocol::FS_CONTROL_OUTPUT_PAYLOAD_BYTES ? dataSize : protocol::FS_CONTROL_OUTPUT_PAYLOAD_BYTES;

        __try {
            RtlCopyMemory(pSupplement->outputPayload, pSecondInput, copySize);
        }
        __except (EXCEPTION_EXECUTE_HANDLER) {

            return;
        }

        pSupplement->capturedOutputBytes = static_cast<uint32_t>(copySize);
        pSupplement->captured |= protocol::FS_CONTROL_CAPTURED_OUTPUT;

        return;
    }

}

namespace mimo {

    namespace trace {

        namespace supplement {

            namespace fscontrol {

                __declspec(code_seg("PAGE"))
                _Use_decl_annotations_
                void PopulateInput(protocol::FsControlSupplement* pSupplement, const FLT_CALLBACK_DATA* pData) {
                    PAGED_CODE();

                    const ULONG method = METHOD_FROM_CTL_CODE(pData->Iopb->Parameters.FileSystemControl.Common.FsControlCode);

                    const void* pInputBuffer = nullptr;
                    bool isRawUserBuffer = false;

                    switch (method) {

                        case METHOD_BUFFERED:
                            pInputBuffer = pData->Iopb->Parameters.FileSystemControl.Buffered.SystemBuffer;

                            break;

                        case METHOD_IN_DIRECT:
                        case METHOD_OUT_DIRECT:
                            pInputBuffer = pData->Iopb->Parameters.FileSystemControl.Direct.InputSystemBuffer;

                            break;

                        case METHOD_NEITHER: {
                            const void* pRawBuffer = pData->Iopb->Parameters.FileSystemControl.Neither.InputBuffer;

                            if (pData->RequestorMode == KernelMode && reinterpret_cast<ULONG_PTR>(pRawBuffer) >= reinterpret_cast<ULONG_PTR>(MmSystemRangeStart)) {
                                pInputBuffer = pRawBuffer;
                            }
                            else if (pData->Thread && IoThreadToProcess(pData->Thread) == PsGetCurrentProcess()) {
                                pInputBuffer = pRawBuffer;
                                isRawUserBuffer = true;
                            }

                            break;
                        }

                    }

                    const ULONG size = pData->Iopb->Parameters.FileSystemControl.Common.InputBufferLength;

                    if (pInputBuffer && size) {
                        const ULONG copySize = size < protocol::FS_CONTROL_INPUT_PAYLOAD_BYTES ? size : protocol::FS_CONTROL_INPUT_PAYLOAD_BYTES;

                        __try {
                            if (isRawUserBuffer) {
                                ProbeForRead(const_cast<void*>(pInputBuffer), copySize, 1u);
                            }

                            RtlCopyMemory(pSupplement->inputPayload, pInputBuffer, copySize);

                            pSupplement->capturedInputBytes = static_cast<uint32_t>(copySize);
                            pSupplement->captured |= protocol::FS_CONTROL_CAPTURED_INPUT;
                        }
                        __except (EXCEPTION_EXECUTE_HANDLER) {}

                    }

                    // METHOD_IN_DIRECT: direct buffer is a second input, not a result
                    if (method == METHOD_IN_DIRECT && pData->Iopb->Parameters.FileSystemControl.Direct.OutputMdlAddress) {
                        PopulateSecondInput(pSupplement, pData);
                    }

                    return;
                }


                __declspec(code_seg("PAGE"))
                _Use_decl_annotations_
                void PopulateOutput(protocol::FsControlSupplement* pSupplement, const FLT_CALLBACK_DATA* pData) {
                    PAGED_CODE();

                    if (!pData->IoStatus.Information) return;

                    const void* pOutputBuffer = nullptr;
                    bool isRawUserBuffer = false;
                    ULONG bufferSize = pData->Iopb->Parameters.FileSystemControl.Common.OutputBufferLength;

                    switch (METHOD_FROM_CTL_CODE(pData->Iopb->Parameters.FileSystemControl.Common.FsControlCode)) {

                        case METHOD_BUFFERED:
                            pOutputBuffer = pData->Iopb->Parameters.FileSystemControl.Buffered.SystemBuffer;

                            break;

                        case METHOD_OUT_DIRECT:
                            pOutputBuffer = mdl::Map(pData->Iopb->Parameters.FileSystemControl.Direct.OutputMdlAddress, pData->Iopb->Parameters.FileSystemControl.Direct.OutputBuffer, &bufferSize);

                            break;

                        case METHOD_NEITHER:
                            pOutputBuffer = mdl::Map(pData->Iopb->Parameters.FileSystemControl.Neither.OutputMdlAddress, pData->Iopb->Parameters.FileSystemControl.Neither.OutputBuffer, &bufferSize);

                            if (!pOutputBuffer) {
                                const void* pRawBuffer = pData->Iopb->Parameters.FileSystemControl.Neither.OutputBuffer;

                                if (pData->RequestorMode == KernelMode && reinterpret_cast<ULONG_PTR>(pRawBuffer) >= reinterpret_cast<ULONG_PTR>(MmSystemRangeStart)) {
                                    pOutputBuffer = pRawBuffer;
                                }
                                else if (pData->Thread && IoThreadToProcess(pData->Thread) == PsGetCurrentProcess()) {
                                    pOutputBuffer = pRawBuffer;
                                    isRawUserBuffer = true;
                                }
                            }

                            break;

                        // METHOD_IN_DIRECT: the direct buffer is a second input, not a result

                    }

                    if (!pOutputBuffer) return;

                    const ULONG_PTR writtenSize = pData->IoStatus.Information;
                    const ULONG dataSize = writtenSize < bufferSize ? static_cast<ULONG>(writtenSize) : bufferSize;
                    const ULONG copySize = dataSize < protocol::FS_CONTROL_OUTPUT_PAYLOAD_BYTES ? dataSize : protocol::FS_CONTROL_OUTPUT_PAYLOAD_BYTES;

                    __try {
                        if (isRawUserBuffer) {
                            ProbeForRead(const_cast<void*>(pOutputBuffer), copySize, 1u);
                        }

                        RtlCopyMemory(pSupplement->outputPayload, pOutputBuffer, copySize);
                    }
                    __except (EXCEPTION_EXECUTE_HANDLER) {

                        return;
                    }

                    pSupplement->capturedOutputBytes = static_cast<uint32_t>(copySize);
                    pSupplement->captured |= protocol::FS_CONTROL_CAPTURED_OUTPUT;

                    return;
                }

            }

        }

    }

}
