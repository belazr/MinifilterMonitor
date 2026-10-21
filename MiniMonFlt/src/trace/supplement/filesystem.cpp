#include "filesystem.h"

#include "..\..\memory.h"

#include "..\..\..\..\inc\protocol.h"

#include <fltKernel.h>

using namespace mimo;

namespace {

    void PopulateSecondInput(_Inout_ protocol::FsControlSupplement* pSupplement, _In_ const FLT_CALLBACK_DATA* pData) {
        const ULONG bufferSize = pData->Iopb->Parameters.FileSystemControl.Common.OutputBufferLength;

        if (!bufferSize) return;

        ULONG readableSize = bufferSize;
        const void* const pSecondInput = memory::GetReadableBuffer(pData, pData->Iopb->Parameters.FileSystemControl.Direct.OutputMdlAddress, pData->Iopb->Parameters.FileSystemControl.Direct.OutputBuffer, &readableSize);

        if (!pSecondInput || !readableSize) return;

        const ULONG copySize = readableSize < sizeof(pSupplement->outputPayload) ? readableSize : sizeof(pSupplement->outputPayload);

        if (!memory::CopyReadableBuffer(pSupplement->outputPayload, pSecondInput, copySize)) return;

        if (copySize < bufferSize) {
            pSupplement->captured |= protocol::FS_CONTROL_TRUNCATED_OUTPUT;
        }

        pSupplement->capturedOutputSize = static_cast<uint32_t>(copySize);
        pSupplement->captured |= protocol::FS_CONTROL_CAPTURED_OUTPUT;

        return;
    }

}

namespace mimo {

    namespace trace {

        namespace supplement {

            namespace filesystem {

                _Use_decl_annotations_
                void PopulateInput(protocol::FsControlSupplement* pSupplement, const FLT_CALLBACK_DATA* pData) {

                    if (KeGetCurrentIrql() >= DISPATCH_LEVEL) return;

                    const ULONG inputSize = pData->Iopb->Parameters.FileSystemControl.Common.InputBufferLength;
                    const ULONG outputSize = pData->Iopb->Parameters.FileSystemControl.Common.OutputBufferLength;

                    if (!inputSize && !outputSize) return;

                    const ULONG method = METHOD_FROM_CTL_CODE(pData->Iopb->Parameters.FileSystemControl.Common.FsControlCode);
                    const bool probed = method != METHOD_NEITHER;
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

                    if (pRawBuffer && memory::IsRawBufferReadable(pData, pRawBuffer, inputSize, probed)) {
                        pInputBuffer = pRawBuffer;
                    }

                    if (method == METHOD_IN_DIRECT) {
                        PopulateSecondInput(pSupplement, pData);
                    }

                    if (!pInputBuffer || !inputSize) return;

                    const ULONG copySize = inputSize < sizeof(pSupplement->inputPayload) ? inputSize : sizeof(pSupplement->inputPayload);

                    if (!memory::CopyReadableBuffer(pSupplement->inputPayload, pInputBuffer, copySize)) return;

                    if (copySize < inputSize) {
                        pSupplement->captured |= protocol::FS_CONTROL_TRUNCATED_INPUT;
                    }

                    pSupplement->capturedInputSize = static_cast<uint32_t>(copySize);
                    pSupplement->captured |= protocol::FS_CONTROL_CAPTURED_INPUT;

                    return;
                }


                _Use_decl_annotations_
                void PopulateOutput(protocol::FsControlSupplement* pSupplement, const FLT_CALLBACK_DATA* pData) {
                    const ULONG method = METHOD_FROM_CTL_CODE(pData->Iopb->Parameters.FileSystemControl.Common.FsControlCode);
                    const ULONG bufferSize = pData->Iopb->Parameters.FileSystemControl.Common.OutputBufferLength;
                    const ULONG_PTR writtenSize = pData->IoStatus.Information;

                    if (method == METHOD_IN_DIRECT || !bufferSize || !writtenSize) return;

                    const bool probed = method != METHOD_NEITHER;
                    const ULONG dataSize = writtenSize < bufferSize ? static_cast<ULONG>(writtenSize) : bufferSize;
                    ULONG readableSize = dataSize;
                    const void* pOutputBuffer = nullptr;

                    switch (method) {

                        case METHOD_BUFFERED:

                            if (KeGetCurrentIrql() >= DISPATCH_LEVEL) break;

                            pOutputBuffer = pData->Iopb->Parameters.FileSystemControl.Buffered.SystemBuffer;

                            break;

                        case METHOD_OUT_DIRECT:
                            pOutputBuffer = memory::GetReadableBuffer(pData, pData->Iopb->Parameters.FileSystemControl.Direct.OutputMdlAddress, pData->Iopb->Parameters.FileSystemControl.Direct.OutputBuffer, &readableSize, probed);

                            break;

                        case METHOD_NEITHER:
                            pOutputBuffer = memory::GetReadableBuffer(pData, pData->Iopb->Parameters.FileSystemControl.Neither.OutputMdlAddress, pData->Iopb->Parameters.FileSystemControl.Neither.OutputBuffer, &readableSize, probed);

                            break;

                    }

                    if (!pOutputBuffer || !readableSize) return;

                    const ULONG copySize = readableSize < sizeof(pSupplement->outputPayload) ? readableSize : sizeof(pSupplement->outputPayload);

                    if (!memory::CopyReadableBuffer(pSupplement->outputPayload, pOutputBuffer, copySize)) return;

                    if (copySize < dataSize) {
                        pSupplement->captured |= protocol::FS_CONTROL_TRUNCATED_OUTPUT;
                    }

                    pSupplement->capturedOutputSize = static_cast<uint32_t>(copySize);
                    pSupplement->captured |= protocol::FS_CONTROL_CAPTURED_OUTPUT;

                    return;
                }

            }

        }

    }

}
