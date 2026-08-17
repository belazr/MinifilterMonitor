#include "security.h"

#include "..\names.h"

#include "..\..\..\..\inc\protocol.h"

#include <format>
#include <string>

namespace mimo {

    namespace trace {

        namespace details {

            namespace security {

                std::wstring RenderQuery(const protocol::RecordData& data) {
                    const protocol::FltParameters& parameters = data.parameters;

                    return std::format(L"Information: {}, Length: {}", names::RenderSecurityInformation(parameters.querySecurity.securityInformation), parameters.querySecurity.length);
                }


                std::wstring RenderSet(const protocol::RecordData& data) {
                    const protocol::FltParameters& parameters = data.parameters;

                    return std::format(L"Information: {}", names::RenderSecurityInformation(parameters.setSecurity.securityInformation));
                }

            }

        }

    }

}
