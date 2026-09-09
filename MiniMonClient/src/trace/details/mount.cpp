#include "mount.h"

#include "..\names.h"

#include "..\..\text.h"

#include "..\..\..\..\inc\protocol.h"

#include <format>
#include <string>

namespace mimo {

    namespace trace {

        namespace details {

            namespace mount {

                std::wstring Render(const protocol::RecordData& data) {
                    std::wstring details = std::format(L"DeviceType: {}", names::RenderDeviceType(data.parameters.mountVolume.deviceType));

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
