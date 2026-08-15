#include "deviceio.h"

#include "..\..\memory.h"

#include "..\..\..\..\inc\protocol.h"

#include <fltKernel.h>

using namespace mimo;

namespace {

    __declspec(code_seg("PAGE"))
    void PopulateSecondInput(_Inout_ protocol::DeviceIoControlSupplement* pSupplement, _In_ const FLT_CALLBACK_DATA* pData) {
        PAGED_CODE();

        ULONG bufferSize = pData->Iopb->Parameters.DeviceIoControl.Common.OutputBufferLength;

        if (!bufferSize) return;

        const bool isFastIo = FLT_IS_FASTIO_OPERATION(pData);
        MDL* pMdl = isFastIo ? nullptr : pData->Iopb->Parameters.DeviceIoControl.Direct.OutputMdlAddress;
        const void* pRawBuffer = isFastIo ? pData->Iopb->Parameters.DeviceIoControl.FastIo.OutputBuffer : pData->Iopb->Parameters.DeviceIoControl.Direct.OutputBuffer;

        const void* pSecondInput = nullptr;

        if (pMdl) {
            pSecondInput = memory::MapMdl(pMdl, pRawBuffer, &bufferSize);
        }

        if (!pSecondInput && memory::IsRawBufferReadable(pData, pRawBuffer, bufferSize)) {
            pSecondInput = pRawBuffer;
        }

        if (!pSecondInput || !bufferSize) return;

        const ULONG copySize = bufferSize < protocol::DEVICE_IO_CONTROL_OUTPUT_PAYLOAD_BYTES ? bufferSize : protocol::DEVICE_IO_CONTROL_OUTPUT_PAYLOAD_BYTES;

        __try {
            RtlCopyMemory(pSupplement->outputPayload, pSecondInput, copySize);
        }
        __except (EXCEPTION_EXECUTE_HANDLER) {

            return;
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
                    const bool isFastIo = FLT_IS_FASTIO_OPERATION(pData);

                    const void* pInputBuffer = nullptr;
                    const void* pRawBuffer = nullptr;

                    if (isFastIo) {
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
                    ULONG bufferSize = pData->Iopb->Parameters.DeviceIoControl.Common.OutputBufferLength;

                    if (!pData->IoStatus.Information || method == METHOD_IN_DIRECT || !bufferSize) return;

                    const void* pOutputBuffer = nullptr;
                    const void* pRawBuffer = nullptr;

                    if (FLT_IS_FASTIO_OPERATION(pData)) {
                        pRawBuffer = pData->Iopb->Parameters.DeviceIoControl.FastIo.OutputBuffer;
                    }
                    else {

                        switch (method) {

                            case METHOD_BUFFERED:
                                pOutputBuffer = pData->Iopb->Parameters.DeviceIoControl.Buffered.SystemBuffer;

                                break;

                            case METHOD_OUT_DIRECT:
                                pOutputBuffer = memory::MapMdl(pData->Iopb->Parameters.DeviceIoControl.Direct.OutputMdlAddress, pData->Iopb->Parameters.DeviceIoControl.Direct.OutputBuffer, &bufferSize);

                                break;

                            case METHOD_NEITHER:
                                pOutputBuffer = memory::MapMdl(pData->Iopb->Parameters.DeviceIoControl.Neither.OutputMdlAddress, pData->Iopb->Parameters.DeviceIoControl.Neither.OutputBuffer, &bufferSize);

                                if (!pOutputBuffer) {
                                    pRawBuffer = pData->Iopb->Parameters.DeviceIoControl.Neither.OutputBuffer;
                                }

                                break;

                            // METHOD_IN_DIRECT: the output buffer is a second input, not a result

                        }

                    }

                    if (pRawBuffer && memory::IsRawBufferReadable(pData, pRawBuffer, bufferSize)) {
                        pOutputBuffer = pRawBuffer;
                    }

                    if (!pOutputBuffer || !bufferSize) return;

                    const ULONG_PTR writtenSize = pData->IoStatus.Information;
                    const ULONG dataSize = writtenSize < bufferSize ? static_cast<ULONG>(writtenSize) : bufferSize;
                    const ULONG copySize = dataSize < protocol::DEVICE_IO_CONTROL_OUTPUT_PAYLOAD_BYTES ? dataSize : protocol::DEVICE_IO_CONTROL_OUTPUT_PAYLOAD_BYTES;

                    __try {
                        RtlCopyMemory(pSupplement->outputPayload, pOutputBuffer, copySize);
                    }
                    __except (EXCEPTION_EXECUTE_HANDLER) {

                        return;
                    }

                    pSupplement->capturedOutputBytes = static_cast<uint32_t>(copySize);
                    pSupplement->captured |= protocol::DEVICE_IO_CONTROL_CAPTURED_OUTPUT;

                    return;
                }

            }

        }

    }

}
