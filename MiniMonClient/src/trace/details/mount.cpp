#include "mount.h"

#include "..\names.h"

#include "..\..\text.h"

#include "..\..\..\..\inc\protocol.h"

#include <cstdint>
#include <format>
#include <string>

using namespace mimo;

namespace {

    std::wstring RenderMountParameters(uint32_t deviceType) {

        return std::format(L"DeviceType: {}", trace::names::RenderDeviceType(deviceType));
    }

}

namespace mimo {

    namespace trace {

        namespace details {

            namespace mount {

                // FltMgr builds this callback data without an IRP: IrpFlags and OperationFlags are always 0
                std::wstring Render(const protocol::RecordData& data) {
                    std::wstring details = RenderMountParameters(data.parameters.mountVolume.deviceType);

                    const protocol::MountSupplement& mountSupplement = data.supplement.mount;

                    if (mountSupplement.captured & protocol::MOUNT_CAPTURED_VOLUME_NAME) {
                        details += std::format(L", VolumeName: {}", text::Extract(mountSupplement.volumeName));
                    }

                    if (mountSupplement.captured & protocol::MOUNT_CAPTURED_FILE_SYSTEM_TYPE) {
                        details += std::format(L", FileSystemType: {}", names::RenderFileSystemType(mountSupplement.fileSystemType));
                    }

                    return details;
                }

            }

        }

    }

}
