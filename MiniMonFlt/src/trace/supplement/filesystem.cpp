#include "filesystem.h"

#include "..\..\memory.h"

#include "..\..\..\..\inc\protocol.h"

#include <fltKernel.h>

using namespace mimo;

namespace {

    __declspec(code_seg("PAGE"))
    void PopulateSecondInput(_Inout_ protocol::FsControlSupplement* pSupplement, _In_ const FLT_CALLBACK_DATA* pData) {
        PAGED_CODE();

        ULONG bufferSize = pData->Iopb->Parameters.FileSystemControl.Common.OutputBufferLength;

        if (!bufferSize) return;

        const void* pSecondInput = memory::MapMdl(pData->Iopb->Parameters.FileSystemControl.Direct.OutputMdlAddress, pData->Iopb->Parameters.FileSystemControl.Direct.OutputBuffer, &bufferSize);

        if (!pSecondInput || !bufferSize) return;

        const ULONG copySize = bufferSize < protocol::FS_CONTROL_OUTPUT_PAYLOAD_BYTES ? bufferSize : protocol::FS_CONTROL_OUTPUT_PAYLOAD_BYTES;

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

            namespace filesystem {

                __declspec(code_seg("PAGE"))
                _Use_decl_annotations_
                void PopulateInput(protocol::FsControlSupplement* pSupplement, const FLT_CALLBACK_DATA* pData) {
                    PAGED_CODE();

                    const ULONG inSize = pData->Iopb->Parameters.FileSystemControl.Common.InputBufferLength;
                    const ULONG outSize = pData->Iopb->Parameters.FileSystemControl.Common.OutputBufferLength;

                    if (!inSize && !outSize) return;

                    const ULONG method = METHOD_FROM_CTL_CODE(pData->Iopb->Parameters.FileSystemControl.Common.FsControlCode);

                    const void* pInputBuffer = nullptr;
                    const void* pRawBuffer = nullptr;

                    switch (method) {

                        case METHOD_BUFFERED:
                            pInputBuffer = pData->Iopb->Parameters.FileSystemControl.Buffered.SystemBuffer;

                            break;

                        case METHOD_IN_DIRECT:
                        case METHOD_OUT_DIRECT:
                            pInputBuffer = pData->Iopb->Parameters.FileSystemControl.Direct.InputSystemBuffer;

                            break;

                        case METHOD_NEITHER:
                            pRawBuffer = pData->Iopb->Parameters.FileSystemControl.Neither.InputBuffer;

                            break;

                    }

                    if (pRawBuffer && memory::IsRawBufferReadable(pData, pRawBuffer, inSize)) {
                        pInputBuffer = pRawBuffer;
                    }

                    if (pInputBuffer && inSize) {
                        const ULONG copySize = inSize < protocol::FS_CONTROL_INPUT_PAYLOAD_BYTES ? inSize : protocol::FS_CONTROL_INPUT_PAYLOAD_BYTES;

                        __try {
                            RtlCopyMemory(pSupplement->inputPayload, pInputBuffer, copySize);

                            pSupplement->capturedInputBytes = static_cast<uint32_t>(copySize);
                            pSupplement->captured |= protocol::FS_CONTROL_CAPTURED_INPUT;
                        }
                        __except (EXCEPTION_EXECUTE_HANDLER) {}

                    }

                    // METHOD_IN_DIRECT: output buffer is a second input, not a result
                    if (method == METHOD_IN_DIRECT) {
                        PopulateSecondInput(pSupplement, pData);
                    }

                    return;
                }


                __declspec(code_seg("PAGE"))
                _Use_decl_annotations_
                void PopulateOutput(protocol::FsControlSupplement* pSupplement, const FLT_CALLBACK_DATA* pData) {
                    PAGED_CODE();

                    const ULONG method = METHOD_FROM_CTL_CODE(pData->Iopb->Parameters.FileSystemControl.Common.FsControlCode);
                    ULONG bufferSize = pData->Iopb->Parameters.FileSystemControl.Common.OutputBufferLength;

                    if (!pData->IoStatus.Information || method == METHOD_IN_DIRECT || !bufferSize) return;

                    const void* pOutputBuffer = nullptr;

                    switch (method) {

                        case METHOD_BUFFERED:
                            pOutputBuffer = pData->Iopb->Parameters.FileSystemControl.Buffered.SystemBuffer;

                            break;

                        case METHOD_OUT_DIRECT:
                            pOutputBuffer = memory::MapMdl(pData->Iopb->Parameters.FileSystemControl.Direct.OutputMdlAddress, pData->Iopb->Parameters.FileSystemControl.Direct.OutputBuffer, &bufferSize);

                            break;

                        case METHOD_NEITHER:
                            pOutputBuffer = memory::GetReadableBuffer(pData, pData->Iopb->Parameters.FileSystemControl.Neither.OutputMdlAddress, pData->Iopb->Parameters.FileSystemControl.Neither.OutputBuffer, &bufferSize);

                            break;

                        // METHOD_IN_DIRECT: the output buffer is a second input, not a result

                    }

                    if (!pOutputBuffer || !bufferSize) return;

                    const ULONG_PTR writtenSize = pData->IoStatus.Information;
                    const ULONG dataSize = writtenSize < bufferSize ? static_cast<ULONG>(writtenSize) : bufferSize;
                    const ULONG copySize = dataSize < protocol::FS_CONTROL_OUTPUT_PAYLOAD_BYTES ? dataSize : protocol::FS_CONTROL_OUTPUT_PAYLOAD_BYTES;

                    __try {
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
