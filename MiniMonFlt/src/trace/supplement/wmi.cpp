#include "wmi.h"

#include "..\..\..\..\inc\protocol.h"

#include <fltKernel.h>

using namespace mimo;

static_assert(sizeof(GUID) == sizeof(protocol::WmiSupplement::guid), "protocol::WmiSupplement guid does not mirror GUID");

namespace mimo {

    namespace trace {

        namespace supplement {

            namespace wmi {

                __declspec(code_seg("PAGE"))
                _Use_decl_annotations_
                void Populate(protocol::WmiSupplement* pSupplement, const FLT_CALLBACK_DATA* pData) {
                    PAGED_CODE();

                    const GUID* const pGuid = static_cast<const GUID*>(pData->Iopb->Parameters.WMI.DataPath);

                    if (!pGuid) return;

                    RtlCopyMemory(pSupplement->guid, pGuid, sizeof(pSupplement->guid));
                    pSupplement->captured |= protocol::WMI_CAPTURED_GUID;

                    return;
                }

            }

        }

    }

}
