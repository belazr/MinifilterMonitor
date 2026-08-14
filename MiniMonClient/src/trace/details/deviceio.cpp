#include "deviceio.h"

#include "..\names.h"

#include "..\..\..\..\inc\protocol.h"

#include <format>
#include <string>

namespace mimo {

    namespace trace {

        namespace details {

            namespace deviceio {

                std::wstring Render(const protocol::RecordData& data) {
                    const protocol::FltParameters& parameters = data.parameters;

                    return std::format(L"Control: {}, InputBufferLength: {}, OutputBufferLength: {}", names::RenderIoControlCode(parameters.deviceIoControl.ioControlCode), parameters.deviceIoControl.inputBufferLength, parameters.deviceIoControl.outputBufferLength);
                }

            }

        }

    }

}
