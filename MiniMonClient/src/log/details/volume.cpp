#include "volume.h"

#include "..\names.h"

#include "..\..\..\..\inc\protocol.h"

#include <format>
#include <string>

namespace mimo {

    namespace log {

        namespace details {

            namespace volume {

                std::wstring Render(const protocol::RecordData& data) {
                    const protocol::FltParameters& parameters = data.parameters;

                    return std::format(L"Class: {}, Length: {}", names::RenderFsInformationClass(parameters.volumeInformation.fsInformationClass), parameters.volumeInformation.length);
                }

            }

        }

    }

}
