#include "readwrite.h"

#include "..\kernel.h"
#include "..\values.h"

#include "..\..\..\..\inc\protocol.h"

#include <cstdint>
#include <format>
#include <string>

using namespace mimo;

namespace {

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


    std::wstring RenderRange(int64_t fileOffset, uint32_t length, uint32_t key) {
        std::wstring details = std::format(L"Offset: {}, Length: {}", fileOffset, length);

        if (key) {
            details += std::format(L", Key: 0x{:X}", key);
        }

        return details;
    }

}

namespace mimo {

    namespace trace {

        namespace details {

            namespace readwrite {

                std::wstring RenderCopy(const protocol::RecordData& data) {
                    const protocol::FltParameters& parameters = data.parameters;

                    if (data.callbackMinorId & kernel::IRP_MN_COMPLETE) {

                        if (!parameters.readWrite.mdlAddress) return {};

                        return std::format(L"Mdl: 0x{:X}", parameters.readWrite.mdlAddress);
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


                std::wstring RenderCheckIfPossible(const protocol::RecordData& data) {
                    const protocol::FltParameters& parameters = data.parameters;
                    std::wstring details = std::format(L"Operation: {}", parameters.fastIoCheckIfPossible.checkForReadOperation ? L"Read" : L"Write");

                    details += L", ";
                    details += RenderRange(parameters.fastIoCheckIfPossible.fileOffset, parameters.fastIoCheckIfPossible.length, parameters.fastIoCheckIfPossible.lockKey);

                    return details;
                }


                std::wstring RenderMdl(const protocol::RecordData& data) {
                    const protocol::FltParameters& parameters = data.parameters;

                    return RenderRange(parameters.mdlReadWrite.fileOffset, parameters.mdlReadWrite.length, parameters.mdlReadWrite.key);
                }


                std::wstring RenderMdlReadComplete(const protocol::RecordData& data) {
                    const protocol::FltParameters& parameters = data.parameters;

                    if (!parameters.mdlReadComplete.mdlChain) return {};

                    return std::format(L"Mdl: 0x{:X}", parameters.mdlReadComplete.mdlChain);
                }


                std::wstring RenderMdlWriteComplete(const protocol::RecordData& data) {
                    const protocol::FltParameters& parameters = data.parameters;
                    std::wstring details = std::format(L"Offset: {}", parameters.mdlWriteComplete.fileOffset);

                    if (parameters.mdlWriteComplete.mdlChain) {
                        details += std::format(L", Mdl: 0x{:X}", parameters.mdlWriteComplete.mdlChain);
                    }

                    return details;
                }

            }

        }

    }

}
