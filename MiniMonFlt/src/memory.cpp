#include "memory.h"

namespace mimo {

    namespace memory {

        __declspec(code_seg("PAGE"))
        _Use_decl_annotations_
        const void* MapMdl(
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


        __declspec(code_seg("PAGE"))
        _Use_decl_annotations_
        bool IsRawBufferReadable(
            const FLT_CALLBACK_DATA* pData,
            const void* pRawBuffer,
            ULONG bufferSize
        ) {
            PAGED_CODE();

            if (!pRawBuffer) return false;

            if (pData->RequestorMode == KernelMode && reinterpret_cast<ULONG_PTR>(pRawBuffer) >= reinterpret_cast<ULONG_PTR>(MmSystemRangeStart)) return true;

            if (!pData->Thread || IoThreadToProcess(pData->Thread) != PsGetCurrentProcess()) return false;

            __try {
                ProbeForRead(const_cast<void*>(pRawBuffer), bufferSize, 1u);
            }
            __except (EXCEPTION_EXECUTE_HANDLER) {

                return false;
            }

            return true;
        }


        __declspec(code_seg("PAGE"))
        _Use_decl_annotations_
        const void* GetReadableBuffer(
            const FLT_CALLBACK_DATA* pData,
            MDL* pMdl,
            const void* pRawBuffer,
            ULONG* pBufferSize
        ) {
            PAGED_CODE();

            const void* pBuffer = MapMdl(pMdl, pRawBuffer, pBufferSize);

            if (pBuffer) return pBuffer;

            if (pRawBuffer && IsRawBufferReadable(pData, pRawBuffer, *pBufferSize)) return pRawBuffer;

            return nullptr;
        }

    }

}
