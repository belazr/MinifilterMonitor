#include "section.h"

#include "..\kernel.h"
#include "..\names.h"

#include "..\..\..\..\inc\protocol.h"

#include <format>
#include <string>

namespace mimo {

    namespace trace {

        namespace details {

            namespace section {

                std::wstring Render(const protocol::RecordData& data) {
                    const protocol::FltParameters& parameters = data.parameters;
                    std::wstring details = std::format(L"SyncType: {}", names::RenderSectionSyncType(parameters.acquireForSectionSynchronization.syncType));

                    if (parameters.acquireForSectionSynchronization.pageProtection) {
                        details += std::format(L", PageProtection: {}", names::RenderPageProtection(parameters.acquireForSectionSynchronization.pageProtection));
                    }

                    if (parameters.acquireForSectionSynchronization.flags) {
                        details += std::format(L", Flags: {}", names::RenderSectionSyncFlags(parameters.acquireForSectionSynchronization.flags));
                    }

                    if (parameters.acquireForSectionSynchronization.syncType == kernel::SyncTypeCreateSection && parameters.acquireForSectionSynchronization.allocationAttributes) {
                        details += std::format(L", AllocationAttributes: {}", names::RenderAllocationAttributes(parameters.acquireForSectionSynchronization.allocationAttributes));
                    }

                    return details;
                }

            }

        }

    }

}
