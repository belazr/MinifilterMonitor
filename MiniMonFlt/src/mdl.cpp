#include "mdl.h"

namespace mimo {

    namespace mdl {

        __declspec(code_seg("PAGE"))
        _Use_decl_annotations_
        const void* Map(
            MDL* pMdl,
            const void* pExpectedAddress,
            ULONG* pBufferSize
        ) {
            PAGED_CODE();

            if (!pMdl) return nullptr;

            if (pExpectedAddress && MmGetMdlVirtualAddress(pMdl) != pExpectedAddress) return nullptr;

            const void* pBuffer = MmGetSystemAddressForMdlSafe(pMdl, NormalPagePriority | MdlMappingNoExecute);

            if (!pBuffer) return nullptr;

            const ULONG mdlSize = MmGetMdlByteCount(pMdl);

            if (mdlSize < *pBufferSize) {
                *pBufferSize = mdlSize;
            }

            return pBuffer;
        }

    }

}
