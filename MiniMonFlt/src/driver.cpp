#include "driver.h"

#include "config.h"
#include "dispatch.h"
#include "modules.h"
#include "port.h"
#include "records.h"
#include "transaction.h"

#include <fltKernel.h>

using namespace mimo;

namespace {

    const FLT_CONTEXT_REGISTRATION Contexts[] {
        { FLT_TRANSACTION_CONTEXT, 0, nullptr, sizeof(transaction::Context), driver::MEM_TAG },
        { FLT_INSTANCE_CONTEXT, 0, nullptr, sizeof(driver::InstanceContext), driver::MEM_TAG },
        { FLT_CONTEXT_END },
    };

    const FLT_OPERATION_REGISTRATION Callbacks[] {
        // file lifecycle
        { IRP_MJ_CREATE, 0, dispatch::PreOperationCallback, dispatch::PostOperationCallback },
        { IRP_MJ_CREATE_NAMED_PIPE, 0, dispatch::PreOperationCallback, dispatch::PostOperationCallback },
        { IRP_MJ_CREATE_MAILSLOT, 0, dispatch::PreOperationCallback, dispatch::PostOperationCallback },
        { IRP_MJ_CLEANUP, 0, dispatch::PreOperationCallback, dispatch::PostOperationCallback },
        { IRP_MJ_CLOSE, 0, dispatch::PreOperationCallback, dispatch::PostOperationCallback },

        // data transfer
        { IRP_MJ_READ, 0, dispatch::PreOperationCallback, dispatch::PostOperationCallback },
        { IRP_MJ_WRITE, 0, dispatch::PreOperationCallback, dispatch::PostOperationCallback },
        { IRP_MJ_FLUSH_BUFFERS, 0, dispatch::PreOperationCallback, dispatch::PostOperationCallback },

        // file metadata
        { IRP_MJ_QUERY_INFORMATION, 0, dispatch::PreOperationCallback, dispatch::PostOperationCallback },
        { IRP_MJ_SET_INFORMATION, 0, dispatch::PreOperationCallback, dispatch::PostOperationCallback },
        { IRP_MJ_QUERY_EA, 0, dispatch::PreOperationCallback, dispatch::PostOperationCallback },
        { IRP_MJ_SET_EA, 0, dispatch::PreOperationCallback, dispatch::PostOperationCallback },
        { IRP_MJ_NETWORK_QUERY_OPEN, 0, dispatch::PreOperationCallback, dispatch::PostOperationCallback },

        // security and quota
        { IRP_MJ_QUERY_SECURITY, 0, dispatch::PreOperationCallback, dispatch::PostOperationCallback },
        { IRP_MJ_SET_SECURITY, 0, dispatch::PreOperationCallback, dispatch::PostOperationCallback },
        { IRP_MJ_QUERY_QUOTA, 0, dispatch::PreOperationCallback, dispatch::PostOperationCallback },
        { IRP_MJ_SET_QUOTA, 0, dispatch::PreOperationCallback, dispatch::PostOperationCallback },

        // directory and namespace
        { IRP_MJ_DIRECTORY_CONTROL, 0, dispatch::PreOperationCallback, dispatch::PostOperationCallback },

        // file-system and device control
        { IRP_MJ_FILE_SYSTEM_CONTROL, 0, dispatch::PreOperationCallback, dispatch::PostOperationCallback },
        { IRP_MJ_DEVICE_CONTROL, 0, dispatch::PreOperationCallback, dispatch::PostOperationCallback },
        { IRP_MJ_INTERNAL_DEVICE_CONTROL, 0, dispatch::PreOperationCallback, dispatch::PostOperationCallback },

        // byte-range locking
        { IRP_MJ_LOCK_CONTROL, 0, dispatch::PreOperationCallback, dispatch::PostOperationCallback },

        // cache and paging coherency
        { IRP_MJ_ACQUIRE_FOR_SECTION_SYNCHRONIZATION, 0, dispatch::PreOperationCallback, dispatch::PostOperationCallback },
        { IRP_MJ_RELEASE_FOR_SECTION_SYNCHRONIZATION, 0, dispatch::PreOperationCallback, dispatch::PostOperationCallback },
        { IRP_MJ_ACQUIRE_FOR_MOD_WRITE, 0, dispatch::PreOperationCallback, dispatch::PostOperationCallback },
        { IRP_MJ_RELEASE_FOR_MOD_WRITE, 0, dispatch::PreOperationCallback, dispatch::PostOperationCallback },
        { IRP_MJ_ACQUIRE_FOR_CC_FLUSH, 0, dispatch::PreOperationCallback, dispatch::PostOperationCallback },
        { IRP_MJ_RELEASE_FOR_CC_FLUSH, 0, dispatch::PreOperationCallback, dispatch::PostOperationCallback },
        { IRP_MJ_FAST_IO_CHECK_IF_POSSIBLE, 0, dispatch::PreOperationCallback, dispatch::PostOperationCallback },
        { IRP_MJ_MDL_READ, 0, dispatch::PreOperationCallback, dispatch::PostOperationCallback },
        { IRP_MJ_MDL_READ_COMPLETE, 0, dispatch::PreOperationCallback, dispatch::PostOperationCallback },
        { IRP_MJ_PREPARE_MDL_WRITE, 0, dispatch::PreOperationCallback, dispatch::PostOperationCallback },
        { IRP_MJ_MDL_WRITE_COMPLETE, 0, dispatch::PreOperationCallback, dispatch::PostOperationCallback },

        // volume
        { IRP_MJ_QUERY_VOLUME_INFORMATION, 0, dispatch::PreOperationCallback, dispatch::PostOperationCallback },
        { IRP_MJ_SET_VOLUME_INFORMATION, 0, dispatch::PreOperationCallback, dispatch::PostOperationCallback },
        { IRP_MJ_VOLUME_MOUNT, 0, dispatch::PreOperationCallback, dispatch::PostOperationCallback },
        { IRP_MJ_VOLUME_DISMOUNT, 0, dispatch::PreOperationCallback, dispatch::PostOperationCallback },

        // device and system events
        { IRP_MJ_PNP, 0, dispatch::PreOperationCallback, dispatch::PostOperationCallback },
        // FltMgr issues no post phase for SHUTDOWN
        { IRP_MJ_SHUTDOWN, 0, dispatch::PreOperationCallback, nullptr },

        { IRP_MJ_OPERATION_END }
    };

    NTSTATUS FilterUnload(_In_ FLT_FILTER_UNLOAD_FLAGS flags);

    NTSTATUS InstanceSetup(
        _In_ const FLT_RELATED_OBJECTS* pFltObjects,
        _In_ FLT_INSTANCE_SETUP_FLAGS flags,
        _In_ DEVICE_TYPE volumeDeviceType,
        _In_ FLT_FILESYSTEM_TYPE volumeFilesystemType
    );

    NTSTATUS QueryTeardown(_In_ const FLT_RELATED_OBJECTS* pFltObjects, _In_ FLT_INSTANCE_QUERY_TEARDOWN_FLAGS flags);

    const FLT_REGISTRATION FilterRegistration {
        .Size = sizeof(FLT_REGISTRATION),
        .Version = FLT_REGISTRATION_VERSION,
        .Flags = FLTFL_REGISTRATION_SUPPORT_NPFS_MSFS,
        .ContextRegistration = Contexts,
        .OperationRegistration = Callbacks,
        .FilterUnloadCallback = FilterUnload,
        .InstanceSetupCallback = InstanceSetup,
        .InstanceQueryTeardownCallback = QueryTeardown,
        .TransactionNotificationCallback = transaction::HandleNotification,
    };

    __declspec(code_seg("PAGE"))
    _Use_decl_annotations_
    NTSTATUS FilterUnload(FLT_FILTER_UNLOAD_FLAGS flags) {
        PAGED_CODE();

        UNREFERENCED_PARAMETER(flags);

        port::Close();
        FltUnregisterFilter(driver::Filter);

        modules::Delete();
        records::Delete();

        return STATUS_SUCCESS;
    }


    __declspec(code_seg("PAGE"))
    ULONG GetInstanceAltitude(_In_ const FLT_RELATED_OBJECTS* pFltObjects) {
        PAGED_CODE();

        WCHAR volumeNameBuffer[128u]{};
        UNICODE_STRING volumeName{};
        volumeName.Buffer = volumeNameBuffer;
        volumeName.MaximumLength = sizeof(volumeNameBuffer);

        if (!NT_SUCCESS(FltGetVolumeName(pFltObjects->Volume, &volumeName, nullptr))) return 0u;

        UCHAR infoBuffer[sizeof(INSTANCE_FULL_INFORMATION) + 512u * sizeof(WCHAR)]{};

        for (ULONG index = 0u; ; index++) {
            ULONG bytesReturned = 0u;
            const NTSTATUS status = FltEnumerateInstanceInformationByFilter(pFltObjects->Filter, index, InstanceFullInformation, infoBuffer, sizeof(infoBuffer), &bytesReturned);

            // this volume's entries always fit: its name fit above and instance names cap at INSTANCE_NAME_MAX_CHARS
            if (status == STATUS_BUFFER_TOO_SMALL) continue;

            if (!NT_SUCCESS(status)) break;

            const INSTANCE_FULL_INFORMATION* pInfo = reinterpret_cast<const INSTANCE_FULL_INFORMATION*>(infoBuffer);

            UNICODE_STRING infoVolumeName{};
            infoVolumeName.Buffer = reinterpret_cast<PWCH>(infoBuffer + pInfo->VolumeNameBufferOffset);
            infoVolumeName.Length = pInfo->VolumeNameLength;
            infoVolumeName.MaximumLength = pInfo->VolumeNameLength;

            if (!RtlEqualUnicodeString(&infoVolumeName, &volumeName, TRUE)) continue;

            UNICODE_STRING infoInstanceName{};
            infoInstanceName.Buffer = reinterpret_cast<PWCH>(infoBuffer + pInfo->InstanceNameBufferOffset);
            infoInstanceName.Length = pInfo->InstanceNameLength;
            infoInstanceName.MaximumLength = pInfo->InstanceNameLength;

            PFLT_INSTANCE pCandidate = nullptr;

            if (!NT_SUCCESS(FltGetVolumeInstanceFromName(pFltObjects->Filter, pFltObjects->Volume, &infoInstanceName, &pCandidate))) continue;

            const bool found = pCandidate == pFltObjects->Instance;
            FltObjectDereference(pCandidate);

            if (!found) continue;

            UNICODE_STRING infoAltitude{};
            infoAltitude.Buffer = reinterpret_cast<PWCH>(infoBuffer + pInfo->AltitudeBufferOffset);
            infoAltitude.Length = pInfo->AltitudeLength;
            infoAltitude.MaximumLength = pInfo->AltitudeLength;

            ULONG altitude = 0u;

            if (!NT_SUCCESS(RtlUnicodeStringToInteger(&infoAltitude, 10u, &altitude))) return 0u;

            return altitude;
        }

        return 0u;
    }


    __declspec(code_seg("PAGE"))
    _Use_decl_annotations_
    NTSTATUS InstanceSetup(
        const FLT_RELATED_OBJECTS* pFltObjects,
        FLT_INSTANCE_SETUP_FLAGS flags,
        DEVICE_TYPE volumeDeviceType,
        FLT_FILESYSTEM_TYPE volumeFilesystemType
    ) {
        PAGED_CODE();

        UNREFERENCED_PARAMETER(flags);
        UNREFERENCED_PARAMETER(volumeDeviceType);
        UNREFERENCED_PARAMETER(volumeFilesystemType);

        driver::InstanceContext* pContext = nullptr;
        NTSTATUS status = STATUS_SUCCESS;
        const ULONG altitude = GetInstanceAltitude(pFltObjects);

        status = FltAllocateContext(pFltObjects->Filter, FLT_INSTANCE_CONTEXT, sizeof(driver::InstanceContext), NonPagedPool, reinterpret_cast<PFLT_CONTEXT*>(&pContext));

        if (!NT_SUCCESS(status)) goto done;

        pContext->altitude = altitude;

        status = FltSetInstanceContext(pFltObjects->Instance, FLT_SET_CONTEXT_KEEP_IF_EXISTS, pContext, nullptr);

        if (status == STATUS_FLT_CONTEXT_ALREADY_DEFINED) {
            status = STATUS_SUCCESS;
        }

    done:

        if (pContext) {
            FltReleaseContext(pContext);
        }

        return status;
    }


    __declspec(code_seg("PAGE"))
    _Use_decl_annotations_
    NTSTATUS QueryTeardown(const FLT_RELATED_OBJECTS* pFltObjects, FLT_INSTANCE_QUERY_TEARDOWN_FLAGS flags) {
        PAGED_CODE();

        UNREFERENCED_PARAMETER(pFltObjects);
        UNREFERENCED_PARAMETER(flags);

        return STATUS_SUCCESS;
    }

}

namespace mimo {

    namespace driver {

        PFLT_FILTER Filter;

    }

}

extern "C" DRIVER_INITIALIZE DriverEntry;

__declspec(code_seg("INIT"))
_Use_decl_annotations_
extern "C" NTSTATUS DriverEntry(DRIVER_OBJECT* pDriverObject, UNICODE_STRING* pRegistryPath) {
    NTSTATUS status = STATUS_SUCCESS;

    records::Create();
    modules::Create();
    config::Create(pRegistryPath);

    status = FltRegisterFilter(pDriverObject, &FilterRegistration, &driver::Filter);

    if (!NT_SUCCESS(status)) goto done;

    status = port::Open(driver::Filter);

    if (!NT_SUCCESS(status)) goto done;

    status = modules::Init();

    if (!NT_SUCCESS(status)) goto done;

    status = FltStartFiltering(driver::Filter);

done:

    if (!NT_SUCCESS(status)) {
        port::Close();

        if (driver::Filter) {
            FltUnregisterFilter(driver::Filter);
        }

        modules::Delete();
        records::Delete();
    }

    return status;
}
