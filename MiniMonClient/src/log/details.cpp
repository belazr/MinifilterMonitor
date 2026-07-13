#include "details.h"

#include "..\kernel.h"

#include "..\..\..\inc\protocol.h"

#include <format>
#include <string>

using namespace mimo;

namespace {

    std::wstring RenderOffset(int64_t byteOffset) {

        if (byteOffset == kernel::FILE_WRITE_TO_END_OF_FILE) return L"EOF";

        if (byteOffset == kernel::FILE_USE_FILE_POINTER_POSITION) return L"Current";

        return std::to_wstring(byteOffset);
    }


    std::wstring RenderReadWriteIoFlags(const protocol::RecordData& data) {
        std::wstring ioFlags;

        if (data.irpFlags & kernel::IRP_NOCACHE) {
            ioFlags += L"Non-cached, ";
        }

        if (data.irpFlags & kernel::IRP_PAGING_IO) {
            ioFlags += L"Paging I/O, ";
        }

        if ((data.irpFlags & kernel::IRP_PAGING_IO) && (data.irpFlags & kernel::IRP_SYNCHRONOUS_PAGING_IO)) {
            ioFlags += L"Synchronous Paging I/O, ";
        }
        else if (data.irpFlags & kernel::IRP_SYNCHRONOUS_API) {
            ioFlags += L"Synchronous, ";
        }

        if (data.operationFlags & kernel::SL_WRITE_THROUGH) {
            ioFlags += L"Write Through, ";
        }

        if (!ioFlags.empty()) {
            ioFlags.resize(ioFlags.size() - 2u);
        }

        return ioFlags;
    }


    std::wstring RenderReadWrite(const protocol::RecordData& data) {
        const protocol::FltParameters& parameters = data.parameters;

        if (data.callbackMinorId & kernel::IRP_MN_COMPLETE) {

            if (!parameters.readWrite.mdlAddress) return {};

            return std::format(L"Mdl: 0x{:X}", parameters.readWrite.mdlAddress);
        }

        std::wstring details = std::format(L"Offset: {}, Length: {}", RenderOffset(parameters.readWrite.byteOffset), parameters.readWrite.length);

        if ((data.operationFlags & kernel::SL_KEY_SPECIFIED) || parameters.readWrite.key) {
            details += std::format(L", Key: 0x{:X}", parameters.readWrite.key);
        }

        const std::wstring ioFlags = RenderReadWriteIoFlags(data);

        if (!ioFlags.empty()) {
            details += L", ";
            details += ioFlags;
        }

        if (data.status >= 0 && data.information != parameters.readWrite.length) {
            details += std::format(L", Transferred: {}", data.information);
        }

        return details;
    }

}

namespace mimo {

    namespace log {

        namespace details {

            std::wstring Render(const protocol::RecordData& data) {

                if (data.transactionNotify) return {};

                switch (data.callbackMajorId) {

                    case kernel::IRP_MJ_READ:
                    case kernel::IRP_MJ_WRITE:

                        return RenderReadWrite(data);

                }

                return {};
            }

        }

    }

}
