#include "readwrite.h"

#include "..\kernel.h"
#include "..\names.h"
#include "..\values.h"

#include "..\..\..\..\inc\protocol.h"

#include <cstdint>
#include <format>
#include <string>

using namespace mimo;

namespace {

    std::wstring RenderWriteCompleteParameters(int64_t byteOffset, uint64_t mdlAddress) {
        std::wstring details = std::format(L"Offset: {}", trace::values::RenderByteOffset(byteOffset));

        if (mdlAddress) {
            details += std::format(L", Mdl: 0x{:X}", mdlAddress);
        }

        return details;
    }


    std::wstring RenderReadCompleteParameters(uint64_t mdlAddress) {

        if (!mdlAddress) return {};

        return std::format(L"Mdl: 0x{:X}", mdlAddress);
    }


    std::wstring RenderParameters(int64_t offset, uint32_t size) {

        return std::format(L"Offset: {}, Length: {}", trace::values::RenderByteOffset(offset), size);
    }


    std::wstring RenderIoFlags(const protocol::RecordData& data) {
        std::wstring result;

        if (data.irpFlags) {
            result += trace::names::RenderIrpFlags(data.irpFlags);
            result += L'|';
        }

        if (data.operationFlags) {
            result += trace::names::RenderReadWriteFlags(data.operationFlags);
            result += L'|';
        }

        if (!result.empty()) {
            result.resize(result.size() - 1u);
        }

        return result;
    }


    std::wstring RenderPayload(const protocol::ReadWriteSupplement& supplement) {

        if (!(supplement.captured & protocol::READ_WRITE_CAPTURED_PAYLOAD)) return {};

        const bool truncated = supplement.captured & protocol::READ_WRITE_TRUNCATED_PAYLOAD;

        return std::format(L"Data: {}", trace::values::RenderBytes({ supplement.payload, supplement.capturedSize }, truncated));
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

                        return isWrite ? RenderWriteCompleteParameters(parameters.readWrite.byteOffset, parameters.readWrite.mdlAddress) : RenderReadCompleteParameters(parameters.readWrite.mdlAddress);
                    }

                    std::wstring details = RenderParameters(parameters.readWrite.byteOffset, parameters.readWrite.length);

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

                    const std::wstring payloadText = RenderPayload(data.supplement.readWrite);

                    if (!payloadText.empty()) {
                        details += L", ";
                        details += payloadText;
                    }

                    return details;
                }


                std::wstring RenderMdl(const protocol::RecordData& data) {
                    const protocol::FltParameters& parameters = data.parameters;
                    std::wstring details = RenderParameters(parameters.mdlReadWrite.fileOffset, parameters.mdlReadWrite.length);

                    if (parameters.mdlReadWrite.key) {
                        details += std::format(L", Key: 0x{:X}", parameters.mdlReadWrite.key);
                    }

                    return details;
                }


                std::wstring RenderMdlReadComplete(const protocol::RecordData& data) {

                    return RenderReadCompleteParameters(data.parameters.mdlReadComplete.mdlChain);
                }


                std::wstring RenderMdlWriteComplete(const protocol::RecordData& data) {
                    const protocol::FltParameters& parameters = data.parameters;

                    return RenderWriteCompleteParameters(parameters.mdlWriteComplete.fileOffset, parameters.mdlWriteComplete.mdlChain);
                }

            }

        }

    }

}
