#include "modwrite.h"

#include "..\..\..\..\inc\protocol.h"

#include <fltKernel.h>

namespace mimo {

    namespace trace {

        namespace supplement {

            namespace modwrite {

                _Use_decl_annotations_
                void Populate(protocol::ModWriteSupplement* pSupplement, const FLT_CALLBACK_DATA* pData) {
                    const LARGE_INTEGER* const pEndingOffset = pData->Iopb->Parameters.AcquireForModifiedPageWriter.EndingOffset;

                    if (!pEndingOffset) return;

                    pSupplement->endingOffset = pEndingOffset->QuadPart;
                    pSupplement->captured |= protocol::MOD_WRITE_CAPTURED_ENDING_OFFSET;

                    return;
                }


                _Use_decl_annotations_
                void PopulateResource(protocol::ModWriteSupplement* pSupplement, const FLT_CALLBACK_DATA* pData) {
                    ERESOURCE* const* const pResourceToRelease = pData->Iopb->Parameters.AcquireForModifiedPageWriter.ResourceToRelease;

                    if (!pResourceToRelease) return;

                    pSupplement->resource = reinterpret_cast<uint64_t>(*pResourceToRelease);
                    pSupplement->captured |= protocol::MOD_WRITE_CAPTURED_RESOURCE;

                    return;
                }

            }

        }

    }

}
