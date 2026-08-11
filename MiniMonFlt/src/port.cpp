#include "port.h"

#include "driver.h"
#include "records.h"

#include "..\..\inc\protocol.h"

#include <fltKernel.h>

namespace {

    PFLT_PORT ServerPort;
    PFLT_PORT ClientPort;

}

namespace mimo {

    namespace port {

        __declspec(code_seg("INIT"))
        _Use_decl_annotations_
        NTSTATUS Open(PFLT_FILTER pFilter) {
            SECURITY_DESCRIPTOR* pSecDesc = nullptr;
            OBJECT_ATTRIBUTES objAttribs{};
            UNICODE_STRING portName = RTL_CONSTANT_STRING(PORT_NAME);
            NTSTATUS status = STATUS_SUCCESS;

            status = FltBuildDefaultSecurityDescriptor(reinterpret_cast<void**>(&pSecDesc), FLT_PORT_ALL_ACCESS);

            if (!NT_SUCCESS(status)) goto done;

            InitializeObjectAttributes(&objAttribs, &portName, OBJ_KERNEL_HANDLE | OBJ_CASE_INSENSITIVE, nullptr, pSecDesc);

            status = FltCreateCommunicationPort(
                pFilter, &ServerPort, &objAttribs, nullptr,
                Connect, Disconnect, Message, 1
            );

        done:

            if (pSecDesc) {
                FltFreeSecurityDescriptor(pSecDesc);
            }

            return status;
        }


        void Close() {

            if (ServerPort) {
                FltCloseCommunicationPort(ServerPort);
                ServerPort = nullptr;
            }

            return;
        }


        __declspec(code_seg("PAGE"))
        _Use_decl_annotations_
        NTSTATUS Connect(
            PFLT_PORT pClientPort,
            void* pServerPortCookie,
            void* pConnectionContext,
            ULONG sizeOfContext,
            void** ppConnectionCookie
        ) {
            PAGED_CODE();

            UNREFERENCED_PARAMETER(pServerPortCookie);
            UNREFERENCED_PARAMETER(pConnectionContext);
            UNREFERENCED_PARAMETER(sizeOfContext);
            UNREFERENCED_PARAMETER(ppConnectionCookie);

            void* const pOldClientPort = InterlockedCompareExchangePointer(reinterpret_cast<void**>(&ClientPort), pClientPort, nullptr);

            if (pOldClientPort) {

                return STATUS_DEVICE_BUSY;
            }

            return STATUS_SUCCESS;
        }


        __declspec(code_seg("PAGE"))
        _Use_decl_annotations_
        void Disconnect(void* pConnectionCookie) {
            PAGED_CODE();

            UNREFERENCED_PARAMETER(pConnectionCookie);

            FltCloseClientPort(driver::Filter, &ClientPort);

            return;
        }


        __declspec(code_seg("PAGE"))
        _Use_decl_annotations_
        NTSTATUS Message(
            void* pConnectionCookie,
            void* pInputBuffer,
            ULONG inputSize,
            void* pOutputBuffer,
            ULONG outputSize,
            ULONG* pBytesWritten
        ) {
            PAGED_CODE();

            UNREFERENCED_PARAMETER(pConnectionCookie);

            if (!pInputBuffer || inputSize < sizeof(protocol::CommandMessage)) return STATUS_INVALID_PARAMETER;

            if (!pOutputBuffer) return STATUS_INVALID_PARAMETER;

            protocol::Command command{};

            __try {
                command = reinterpret_cast<protocol::CommandMessage*>(pInputBuffer)->command;
            }
            __except (EXCEPTION_EXECUTE_HANDLER) {

                return GetExceptionCode();
            }

            NTSTATUS status = STATUS_SUCCESS;

            switch (command) {

                case protocol::Command::GetRecords:

                    if (outputSize == 0u) return STATUS_INVALID_PARAMETER;

                    if (!IS_ALIGNED(pOutputBuffer, alignof(protocol::Record))) return STATUS_DATATYPE_MISALIGNMENT;

                    status = records::Drain(reinterpret_cast<UCHAR*>(pOutputBuffer), outputSize, pBytesWritten);

                    break;

                case protocol::Command::GetVersion:

                    if (outputSize < sizeof(protocol::Version)) return STATUS_INVALID_PARAMETER;

                    if (!IS_ALIGNED(pOutputBuffer, alignof(protocol::Version))) return STATUS_DATATYPE_MISALIGNMENT;

                    __try {
                        protocol::Version* const pVersion = reinterpret_cast<protocol::Version*>(pOutputBuffer);
                        pVersion->major = protocol::MINIMON_MAJOR_VERSION;
                        pVersion->minor = protocol::MINIMON_MINOR_VERSION;
                        pVersion->patch = protocol::MINIMON_PATCH_VERSION;
                    }
                    __except (EXCEPTION_EXECUTE_HANDLER) {

                        return GetExceptionCode();
                    }

                    *pBytesWritten = static_cast<ULONG>(sizeof(protocol::Version));
                    status = STATUS_SUCCESS;

                    break;

                default:
                    status = STATUS_INVALID_PARAMETER;

                    break;
            }

            return status;
        }

    }

}
