#include "modwrite.h"

#include "..\..\..\..\inc\protocol.h"

#include <fltKernel.h>

namespace mimo {

    namespace trace {

        namespace supplement {

            namespace modwrite {

                __declspec(code_seg("PAGE"))
                _Use_decl_annotations_
                void Populate(protocol::ModWriteSupplement* pSupplement, const FLT_CALLBACK_DATA* pData) {
                    PAGED_CODE();

                    const LARGE_INTEGER* const pEndingOffset = pData->Iopb->Parameters.AcquireForModifiedPageWriter.EndingOffset;

                    if (!pEndingOffset) return;

                    pSupplement->endingOffset = pEndingOffset->QuadPart;
                    pSupplement->captured |= protocol::MOD_WRITE_CAPTURED_ENDING_OFFSET;

                    return;
                }

            }

        }

    }

}
