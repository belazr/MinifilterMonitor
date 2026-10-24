#include "mount.h"

#include "..\names.h"

#include "..\..\text.h"

#include "..\..\..\..\inc\protocol.h"

#include <cstdint>
#include <format>
#include <string>

using namespace mimo;

namespace {

    std::wstring RenderParameters(uint32_t deviceType) {

        return std::format(L"DeviceType: {}", trace::names::RenderDeviceType(deviceType));
    }


    std::wstring RenderVolumeName(const protocol::MountSupplement& supplement) {

        if (!(supplement.captured & protocol::MOUNT_CAPTURED_VOLUME_NAME)) return {};

        return std::format(L"VolumeName: {}", text::Extract(supplement.volumeName));
    }

}

namespace mimo {

    namespace trace {

        namespace details {

            namespace mount {

                // FltMgr builds this callback data without an IRP: IrpFlags and OperationFlags are always 0
                std::wstring Render(const protocol::RecordData& data) {
                    std::wstring details = RenderParameters(data.parameters.mountVolume.deviceType);

                    const protocol::MountSupplement& mountSupplement = data.supplement.mount;

                    const std::wstring volumeNameText = RenderVolumeName(mountSupplement);

                    if (!volumeNameText.empty()) {
                        details += L", ";
                        details += volumeNameText;
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
