#include "lock.h"

#include "..\..\..\..\inc\protocol.h"

#include <fltKernel.h>

namespace mimo {

    namespace trace {

        namespace supplement {

            namespace lock {

                __declspec(code_seg("PAGE"))
                _Use_decl_annotations_
                void Populate(protocol::LockControlSupplement* pSupplement, const FLT_CALLBACK_DATA* pData) {
                    PAGED_CODE();

                    const LARGE_INTEGER* const pLength = pData->Iopb->Parameters.LockControl.Length;

                    if (!pLength) return;

                    pSupplement->length = pLength->QuadPart;
                    pSupplement->captured |= protocol::LOCK_CONTROL_CAPTURED_LENGTH;

                    return;
                }

            }

        }

    }

}
