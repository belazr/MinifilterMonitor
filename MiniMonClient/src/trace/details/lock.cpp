#include "lock.h"

#include "..\kernel.h"
#include "..\values.h"

#include "..\..\..\..\inc\protocol.h"

#include <format>
#include <string>

using namespace mimo;

namespace {

    std::wstring RenderLength(const protocol::LockControlSupplement& supplement) {

        if (!(supplement.captured & protocol::LOCK_CONTROL_CAPTURED_LENGTH)) return {};

        return std::format(L"Length: {}", supplement.length);
    }


    std::wstring RenderRange(const protocol::RecordData& data) {
        const protocol::FltParameters& parameters = data.parameters;
        std::wstring details = std::format(L"Offset: {}", parameters.lockControl.byteOffset);

        const std::wstring lengthText = RenderLength(data.supplement.lockControl);

        if (!lengthText.empty()) {
            details += L", ";
            details += lengthText;
        }

        if (parameters.lockControl.key) {
            details += std::format(L", Key: 0x{:X}", parameters.lockControl.key);
        }

        return details;
    }


    std::wstring RenderLock(const protocol::RecordData& data) {
        const protocol::FltParameters& parameters = data.parameters;
        const bool fastIo = data.flags & trace::kernel::FLT_CALLBACK_DATA_FAST_IO_OPERATION;
        const bool exclusiveLock = fastIo ? parameters.lockControl.exclusiveLock : (data.operationFlags & trace::kernel::SL_EXCLUSIVE_LOCK);
        const bool failImmediately = fastIo ? parameters.lockControl.failImmediately : (data.operationFlags & trace::kernel::SL_FAIL_IMMEDIATELY);

        return std::format(L"Exclusive: {}, {}, Fail Immediately: {}", trace::values::RenderBoolean(exclusiveLock), RenderRange(data), trace::values::RenderBoolean(failImmediately));
    }

}

namespace mimo {

    namespace trace {

        namespace details {

            namespace lock {

                std::wstring Render(const protocol::RecordData& data) {

                    switch (data.callbackMinorId) {

                        case kernel::IRP_MN_LOCK:

                            return RenderLock(data);

                        case kernel::IRP_MN_UNLOCK_SINGLE:

                            return RenderRange(data);

                        case kernel::IRP_MN_UNLOCK_ALL_BY_KEY:

                            return std::format(L"Key: 0x{:X}", data.parameters.lockControl.key);

                    }

                    return {};
                }

            }

        }

    }

}
