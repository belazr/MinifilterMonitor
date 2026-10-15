#include "wmi.h"

#include "..\..\..\..\inc\protocol.h"

#include <fltKernel.h>

using namespace mimo;

static_assert(sizeof(GUID) == sizeof(protocol::WmiSupplement::guid), "protocol::WmiSupplement mirror drift: guid");

namespace mimo {

    namespace trace {

        namespace supplement {

            namespace wmi {

                _Use_decl_annotations_
                void Populate(protocol::WmiSupplement* pSupplement, const FLT_CALLBACK_DATA* pData) {

                    if (KeGetCurrentIrql() >= DISPATCH_LEVEL) return;

                    const void* const pDataPath = pData->Iopb->Parameters.WMI.DataPath;

                    if (!pDataPath) return;

                    RtlCopyMemory(pSupplement->guid, pDataPath, sizeof(pSupplement->guid));
                    pSupplement->captured |= protocol::WMI_CAPTURED_GUID;

                    return;
                }

            }

        }

    }

}
