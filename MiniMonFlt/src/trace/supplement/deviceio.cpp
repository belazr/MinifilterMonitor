#include "deviceio.h"

#include "..\..\memory.h"

#include "..\..\..\..\inc\protocol.h"

#include <fltKernel.h>

using namespace mimo;

namespace {

    __declspec(code_seg("PAGE"))
    void PopulateSecondInput(_Inout_ protocol::DeviceIoControlSupplement* pSupplement, _In_ const FLT_CALLBACK_DATA* pData) {
        PAGED_CODE();

        const ULONG bufferSize = pData->Iopb->Parameters.DeviceIoControl.Common.OutputBufferLength;

        if (!bufferSize) return;

        const bool isFastIo = FLT_IS_FASTIO_OPERATION(pData);
        MDL* pMdl = isFastIo ? nullptr : pData->Iopb->Parameters.DeviceIoControl.Direct.OutputMdlAddress;
        const void* pRawBuffer = isFastIo ? pData->Iopb->Parameters.DeviceIoControl.FastIo.OutputBuffer : pData->Iopb->Parameters.DeviceIoControl.Direct.OutputBuffer;

        ULONG readableSize = bufferSize;
        const void* pSecondInput = memory::GetReadableBuffer(pData, pMdl, pRawBuffer, &readableSize);

        if (!pSecondInput || !readableSize) return;

        const ULONG copySize = readableSize < protocol::DEVICE_IO_CONTROL_OUTPUT_PAYLOAD_BYTES ? readableSize : protocol::DEVICE_IO_CONTROL_OUTPUT_PAYLOAD_BYTES;

        __try {
            RtlCopyMemory(pSupplement->outputPayload, pSecondInput, copySize);
        }
        __except (EXCEPTION_EXECUTE_HANDLER) {

            return;
        }

        if (copySize < bufferSize) {
            pSupplement->captured |= protocol::DEVICE_IO_CONTROL_TRUNCATED_OUTPUT;
        }

        pSupplement->capturedOutputBytes = static_cast<uint32_t>(copySize);
        pSupplement->captured |= protocol::DEVICE_IO_CONTROL_CAPTURED_OUTPUT;

        return;
    }

}

namespace mimo {

    namespace trace {

        namespace supplement {

            namespace deviceio {

                __declspec(code_seg("PAGE"))
                _Use_decl_annotations_
                void PopulateInput(protocol::DeviceIoControlSupplement* pSupplement, const FLT_CALLBACK_DATA* pData) {
                    PAGED_CODE();

                    const ULONG inSize = pData->Iopb->Parameters.DeviceIoControl.Common.InputBufferLength;
                    const ULONG outSize = pData->Iopb->Parameters.DeviceIoControl.Common.OutputBufferLength;

                    if (!inSize && !outSize) return;

                    const ULONG method = METHOD_FROM_CTL_CODE(pData->Iopb->Parameters.DeviceIoControl.Common.IoControlCode);

                    const void* pInputBuffer = nullptr;
                    const void* pRawBuffer = nullptr;

                    if (FLT_IS_FASTIO_OPERATION(pData)) {
                        pRawBuffer = pData->Iopb->Parameters.DeviceIoControl.FastIo.InputBuffer;
                    }
                    else {

                        switch (method) {

                            case METHOD_BUFFERED:
                                pInputBuffer = pData->Iopb->Parameters.DeviceIoControl.Buffered.SystemBuffer;

                                break;

                            case METHOD_IN_DIRECT:
                            case METHOD_OUT_DIRECT:
                                pInputBuffer = pData->Iopb->Parameters.DeviceIoControl.Direct.InputSystemBuffer;

                                break;

                            case METHOD_NEITHER:
                                pRawBuffer = pData->Iopb->Parameters.DeviceIoControl.Neither.InputBuffer;

                                break;

                        }

                    }

                    if (pRawBuffer && memory::IsRawBufferReadable(pData, pRawBuffer, inSize)) {
                        pInputBuffer = pRawBuffer;
                    }

                    if (pInputBuffer && inSize) {
                        const ULONG copySize = inSize < protocol::DEVICE_IO_CONTROL_INPUT_PAYLOAD_BYTES ? inSize : protocol::DEVICE_IO_CONTROL_INPUT_PAYLOAD_BYTES;

                        __try {
                            RtlCopyMemory(pSupplement->inputPayload, pInputBuffer, copySize);

                            if (copySize < inSize) {
                                pSupplement->captured |= protocol::DEVICE_IO_CONTROL_TRUNCATED_INPUT;
                            }

                            pSupplement->capturedInputBytes = static_cast<uint32_t>(copySize);
                            pSupplement->captured |= protocol::DEVICE_IO_CONTROL_CAPTURED_INPUT;
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
                void PopulateOutput(protocol::DeviceIoControlSupplement* pSupplement, const FLT_CALLBACK_DATA* pData) {
                    PAGED_CODE();

                    const ULONG method = METHOD_FROM_CTL_CODE(pData->Iopb->Parameters.DeviceIoControl.Common.IoControlCode);
                    const ULONG bufferSize = pData->Iopb->Parameters.DeviceIoControl.Common.OutputBufferLength;
                    const ULONG_PTR writtenSize = pData->IoStatus.Information;

                    if (method == METHOD_IN_DIRECT || !bufferSize || !writtenSize) return;

                    const ULONG dataSize = writtenSize < bufferSize ? static_cast<ULONG>(writtenSize) : bufferSize;
                    ULONG readableSize = dataSize;
                    const void* pOutputBuffer = nullptr;

                    if (FLT_IS_FASTIO_OPERATION(pData)) {
                        pOutputBuffer = memory::GetReadableBuffer(pData, nullptr, pData->Iopb->Parameters.DeviceIoControl.FastIo.OutputBuffer, &readableSize);
                    }
                    else {

                        switch (method) {

                            case METHOD_BUFFERED:
                                pOutputBuffer = pData->Iopb->Parameters.DeviceIoControl.Buffered.SystemBuffer;

                                break;

                            case METHOD_OUT_DIRECT:
                                pOutputBuffer = memory::GetReadableBuffer(pData, pData->Iopb->Parameters.DeviceIoControl.Direct.OutputMdlAddress, pData->Iopb->Parameters.DeviceIoControl.Direct.OutputBuffer, &readableSize);

                                break;

                            case METHOD_NEITHER:
                                pOutputBuffer = memory::GetReadableBuffer(pData, pData->Iopb->Parameters.DeviceIoControl.Neither.OutputMdlAddress, pData->Iopb->Parameters.DeviceIoControl.Neither.OutputBuffer, &readableSize);

                                break;

                            // METHOD_IN_DIRECT: the output buffer is a second input, not a result

                        }

                    }

                    if (!pOutputBuffer || !readableSize) return;

                    const ULONG copySize = readableSize < protocol::DEVICE_IO_CONTROL_OUTPUT_PAYLOAD_BYTES ? readableSize : protocol::DEVICE_IO_CONTROL_OUTPUT_PAYLOAD_BYTES;

                    __try {
                        RtlCopyMemory(pSupplement->outputPayload, pOutputBuffer, copySize);
                    }
                    __except (EXCEPTION_EXECUTE_HANDLER) {

                        return;
                    }

                    if (copySize < dataSize) {
                        pSupplement->captured |= protocol::DEVICE_IO_CONTROL_TRUNCATED_OUTPUT;
                    }

                    pSupplement->capturedOutputBytes = static_cast<uint32_t>(copySize);
                    pSupplement->captured |= protocol::DEVICE_IO_CONTROL_CAPTURED_OUTPUT;

                    return;
                }

            }

        }

    }

}
