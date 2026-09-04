#include "readwrite.h"

#include "..\kernel.h"
#include "..\values.h"

#include "..\..\..\..\inc\protocol.h"

#include <cstdint>
#include <format>
#include <string>

using namespace mimo;

namespace {

    std::wstring RenderWriteComplete(int64_t byteOffset, uint64_t mdlAddress) {
        std::wstring details = std::format(L"Offset: {}", byteOffset);

        if (mdlAddress) {
            details += std::format(L", Mdl: 0x{:X}", mdlAddress);
        }

        return details;
    }


    std::wstring RenderReadComplete(uint64_t mdlAddress) {

        if (!mdlAddress) return {};

        return std::format(L"Mdl: 0x{:X}", mdlAddress);
    }


    std::wstring RenderIoFlags(const protocol::RecordData& data) {
        std::wstring ioFlags;

        if (data.irpFlags & trace::kernel::IRP_NOCACHE) {
            ioFlags += L"Non-cached|";
        }

        if (data.irpFlags & trace::kernel::IRP_PAGING_IO) {
            ioFlags += L"Paging I/O|";
        }

        if ((data.irpFlags & trace::kernel::IRP_PAGING_IO) && (data.irpFlags & trace::kernel::IRP_SYNCHRONOUS_PAGING_IO)) {
            ioFlags += L"Synchronous Paging I/O|";
        }
        else if (data.irpFlags & trace::kernel::IRP_SYNCHRONOUS_API) {
            ioFlags += L"Synchronous|";
        }

        if (data.operationFlags & trace::kernel::SL_WRITE_THROUGH) {
            ioFlags += L"Write Through|";
        }

        if (!ioFlags.empty()) {
            ioFlags.resize(ioFlags.size() - 1u);
        }

        return ioFlags;
    }

}

namespace mimo {

    namespace trace {

        namespace details {

            namespace readwrite {

                std::wstring RenderCopy(const protocol::RecordData& data) {
                    const protocol::FltParameters& parameters = data.parameters;

                    if (data.callbackMinorId & kernel::IRP_MN_COMPLETE) {
                        const bool isWrite = (data.irpFlags & kernel::IRP_WRITE_OPERATION) || parameters.readWrite.byteOffset;

                        return isWrite ? RenderWriteComplete(parameters.readWrite.byteOffset, parameters.readWrite.mdlAddress) : RenderReadComplete(parameters.readWrite.mdlAddress);
                    }

                    std::wstring details = std::format(L"Offset: {}, Length: {}", values::RenderByteOffset(parameters.readWrite.byteOffset), parameters.readWrite.length);

                    if ((data.operationFlags & kernel::SL_KEY_SPECIFIED) || parameters.readWrite.key) {
                        details += std::format(L", Key: 0x{:X}", parameters.readWrite.key);
                    }

                    const std::wstring ioFlags = RenderIoFlags(data);

                    if (!ioFlags.empty()) {
                        details += L", ";
                        details += ioFlags;
                    }

                    if (data.status >= 0 && data.information != parameters.readWrite.length) {
                        details += std::format(L", Transferred: {}", data.information);
                    }

                    return details;
                }


                std::wstring RenderMdl(const protocol::RecordData& data) {
                    const protocol::FltParameters& parameters = data.parameters;
                    std::wstring details = std::format(L"Offset: {}, Length: {}", parameters.mdlReadWrite.fileOffset, parameters.mdlReadWrite.length);

                    if (parameters.mdlReadWrite.key) {
                        details += std::format(L", Key: 0x{:X}", parameters.mdlReadWrite.key);
                    }

                    return details;
                }


                std::wstring RenderMdlReadComplete(const protocol::RecordData& data) {

                    return RenderReadComplete(data.parameters.mdlReadComplete.mdlChain);
                }


                std::wstring RenderMdlWriteComplete(const protocol::RecordData& data) {
                    const protocol::FltParameters& parameters = data.parameters;

                    return RenderWriteComplete(parameters.mdlWriteComplete.fileOffset, parameters.mdlWriteComplete.mdlChain);
                }

            }

        }

    }

}
