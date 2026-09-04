#include "ea.h"

#include "..\..\..\..\inc\protocol.h"

#include <format>
#include <string>

namespace mimo {

    namespace trace {

        namespace details {

            namespace ea {

                std::wstring RenderQuery(const protocol::RecordData& data) {
                    const protocol::FltParameters& parameters = data.parameters;

                    return std::format(L"Length: {}, EaListLength: {}", parameters.queryEa.length, parameters.queryEa.eaListLength);
                }


                std::wstring RenderSet(const protocol::RecordData& data) {

                    return std::format(L"Length: {}", data.parameters.setEa.length);
                }

            }

        }

    }

}
