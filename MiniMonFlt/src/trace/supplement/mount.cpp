#include "mount.h"

#include "..\..\..\..\inc\protocol.h"

#include <fltKernel.h>

namespace mimo {

    namespace trace {

        namespace supplement {

            namespace mount {

                __declspec(code_seg("PAGE"))
                _Use_decl_annotations_
                void Populate(protocol::MountSupplement* pSupplement, const FLT_RELATED_OBJECTS* pFltObjects) {
                    PAGED_CODE();

                    PFLT_VOLUME const pVolume = pFltObjects->Volume;

                    if (!pVolume) return;

                    UNICODE_STRING volumeName{};
                    RtlInitEmptyUnicodeString(&volumeName, pSupplement->volumeName, static_cast<USHORT>(sizeof(pSupplement->volumeName)));

                    if (NT_SUCCESS(FltGetVolumeName(pVolume, &volumeName, nullptr))) {
                        pSupplement->captured |= protocol::MOUNT_CAPTURED_VOLUME_NAME;
                    }

                    FLT_FILESYSTEM_TYPE fileSystemType = FLT_FSTYPE_UNKNOWN;

                    if (NT_SUCCESS(FltGetFileSystemType(pVolume, &fileSystemType))) {
                        pSupplement->fileSystemType = static_cast<uint32_t>(fileSystemType);
                        pSupplement->captured |= protocol::MOUNT_CAPTURED_FILE_SYSTEM_TYPE;
                    }

                    return;
                }

            }

        }

    }

}
