#include "driver.h"

#include "trace\modules.h"

#include "config.h"
#include "dispatch.h"
#include "port.h"
#include "records.h"
#include "transaction.h"

#include <fltKernel.h>
#include <ntstrsafe.h>

using namespace mimo;

namespace {

    constexpr FLT_CONTEXT_REGISTRATION CONTEXTS[]{
        { FLT_TRANSACTION_CONTEXT, 0, nullptr, sizeof(transaction::Context), driver::POOL_TAG },
        { FLT_INSTANCE_CONTEXT, 0, nullptr, sizeof(driver::InstanceContext), driver::POOL_TAG },
        { FLT_CONTEXT_END },
    };

    constexpr FLT_OPERATION_REGISTRATION CALLBACKS[]{
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
        { IRP_MJ_QUERY_OPEN, 0, dispatch::PreOperationCallback, dispatch::PostOperationCallback },
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
        { IRP_MJ_SYSTEM_CONTROL, 0, dispatch::PreOperationCallback, dispatch::PostOperationCallback },
        // FltMgr issues no post phase for SHUTDOWN
        { IRP_MJ_SHUTDOWN, 0, dispatch::PreOperationCallback, nullptr },

        { IRP_MJ_OPERATION_END }
    };

    NTSTATUS FilterUnloadCallback(_In_ FLT_FILTER_UNLOAD_FLAGS flags);

    NTSTATUS InstanceSetupCallback(
        _In_ const FLT_RELATED_OBJECTS* pFltObjects,
        _In_ FLT_INSTANCE_SETUP_FLAGS flags,
        _In_ DEVICE_TYPE volumeDeviceType,
        _In_ FLT_FILESYSTEM_TYPE volumeFilesystemType
    );

    NTSTATUS InstanceQueryTeardownCallback(_In_ const FLT_RELATED_OBJECTS* pFltObjects, _In_ FLT_INSTANCE_QUERY_TEARDOWN_FLAGS flags);

    constexpr FLT_REGISTRATION FILTER_REGISTRATION{
        .Size                            = sizeof(FLT_REGISTRATION),
        .Version                         = FLT_REGISTRATION_VERSION,
        .Flags                           = FLTFL_REGISTRATION_SUPPORT_NPFS_MSFS,
        .ContextRegistration             = CONTEXTS,
        .OperationRegistration           = CALLBACKS,
        .FilterUnloadCallback            = FilterUnloadCallback,
        .InstanceSetupCallback           = InstanceSetupCallback,
        .InstanceQueryTeardownCallback   = InstanceQueryTeardownCallback,
        .TransactionNotificationCallback = transaction::TransactionNotificationCallback,
    };

    __declspec(code_seg("PAGE"))
    _Use_decl_annotations_
    NTSTATUS FilterUnloadCallback(FLT_FILTER_UNLOAD_FLAGS flags) {
        PAGED_CODE();

        UNREFERENCED_PARAMETER(flags);

        port::Close();
        FltUnregisterFilter(driver::pFilter);

        trace::modules::Delete();
        records::Delete();

        return STATUS_SUCCESS;
    }


    __declspec(code_seg("PAGE"))
    NTSTATUS FormatInstanceAltitude(_In_ const FLT_RELATED_OBJECTS* pFltObjects, _Inout_ UNICODE_STRING* pAltitude) {
        PAGED_CODE();

        constexpr ULONG VOLUME_NAME_WCHAR_COUNT = 128u;
        constexpr ULONG ENTRY_STRINGS_WCHAR_COUNT = 512u;
        WCHAR volumeNameBuffer[VOLUME_NAME_WCHAR_COUNT]{};
        UNICODE_STRING volumeName{};
        volumeName.Buffer = volumeNameBuffer;
        volumeName.MaximumLength = static_cast<USHORT>(sizeof(volumeNameBuffer));
        NTSTATUS status = FltGetVolumeName(pFltObjects->Volume, &volumeName, nullptr);

        if (!NT_SUCCESS(status)) return status;

        struct InstanceEntry {
            INSTANCE_FULL_INFORMATION info;
            WCHAR strings[ENTRY_STRINGS_WCHAR_COUNT];
        };

        InstanceEntry entry{};

        for (ULONG i = 0u; ; i++) {
            ULONG bytesReturned = 0u;
            status = FltEnumerateInstanceInformationByFilter(pFltObjects->Filter, i, InstanceFullInformation, &entry, static_cast<ULONG>(sizeof(entry)), &bytesReturned);

            // this volume's entries always fit: its name fit above and instance names cap at INSTANCE_NAME_MAX_CHARS
            if (status == STATUS_BUFFER_TOO_SMALL) continue;

            if (!NT_SUCCESS(status)) break;

            UNICODE_STRING infoVolumeName{};
            infoVolumeName.Buffer = entry.strings + (entry.info.VolumeNameBufferOffset - sizeof(entry.info)) / sizeof(WCHAR);
            infoVolumeName.Length = entry.info.VolumeNameLength;
            infoVolumeName.MaximumLength = entry.info.VolumeNameLength;

            if (!RtlEqualUnicodeString(&infoVolumeName, &volumeName, TRUE)) continue;

            UNICODE_STRING infoInstanceName{};
            infoInstanceName.Buffer = entry.strings + (entry.info.InstanceNameBufferOffset - sizeof(entry.info)) / sizeof(WCHAR);
            infoInstanceName.Length = entry.info.InstanceNameLength;
            infoInstanceName.MaximumLength = entry.info.InstanceNameLength;

            PFLT_INSTANCE pCandidate = nullptr;

            if (!NT_SUCCESS(FltGetVolumeInstanceFromName(pFltObjects->Filter, pFltObjects->Volume, &infoInstanceName, &pCandidate))) continue;

            const bool found = pCandidate == pFltObjects->Instance;
            FltObjectDereference(pCandidate);

            if (!found) continue;

            UNICODE_STRING infoAltitude{};
            infoAltitude.Buffer = entry.strings + (entry.info.AltitudeBufferOffset - sizeof(entry.info)) / sizeof(WCHAR);
            infoAltitude.Length = entry.info.AltitudeLength;
            infoAltitude.MaximumLength = entry.info.AltitudeLength;

            return RtlUnicodeStringCopy(pAltitude, &infoAltitude);
        }

        return status;
    }


    __declspec(code_seg("PAGE"))
    _Use_decl_annotations_
    NTSTATUS InstanceSetupCallback(
        const FLT_RELATED_OBJECTS* pFltObjects,
        FLT_INSTANCE_SETUP_FLAGS flags,
        DEVICE_TYPE volumeDeviceType,
        FLT_FILESYSTEM_TYPE volumeFilesystemType
    ) {
        PAGED_CODE();

        UNREFERENCED_PARAMETER(flags);
        UNREFERENCED_PARAMETER(volumeDeviceType);
        UNREFERENCED_PARAMETER(volumeFilesystemType);

        PFLT_CONTEXT pFltContext = nullptr;
        driver::InstanceContext* pContext = nullptr;
        NTSTATUS status = STATUS_SUCCESS;
        UNICODE_STRING altitude{};

        status = FltAllocateContext(pFltObjects->Filter, FLT_INSTANCE_CONTEXT, sizeof(driver::InstanceContext), NonPagedPool, &pFltContext);

        if (!NT_SUCCESS(status)) goto done;

        pContext = static_cast<driver::InstanceContext*>(pFltContext);
        RtlZeroMemory(pContext, sizeof(driver::InstanceContext));
        RtlInitEmptyUnicodeString(&altitude, pContext->altitude, static_cast<USHORT>(sizeof(pContext->altitude)));
        pContext->altitudeTruncated = FormatInstanceAltitude(pFltObjects, &altitude) == STATUS_BUFFER_OVERFLOW;

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
    NTSTATUS InstanceQueryTeardownCallback(const FLT_RELATED_OBJECTS* pFltObjects, FLT_INSTANCE_QUERY_TEARDOWN_FLAGS flags) {
        PAGED_CODE();

        UNREFERENCED_PARAMETER(pFltObjects);
        UNREFERENCED_PARAMETER(flags);

        return STATUS_SUCCESS;
    }

}

namespace mimo {

    namespace driver {

        PFLT_FILTER pFilter;

    }

}

extern "C" DRIVER_INITIALIZE DriverEntry;

__declspec(code_seg("INIT"))
_Use_decl_annotations_
extern "C" NTSTATUS DriverEntry(DRIVER_OBJECT* pDriverObject, UNICODE_STRING* pRegistryPath) {
    UNREFERENCED_PARAMETER(pRegistryPath);

    NTSTATUS status = STATUS_SUCCESS;

    records::Create();
    trace::modules::Create();
    config::Create(pDriverObject);

    status = FltRegisterFilter(pDriverObject, &FILTER_REGISTRATION, &driver::pFilter);

    if (!NT_SUCCESS(status)) goto done;

    status = port::Open(driver::pFilter);

    if (!NT_SUCCESS(status)) goto done;

    status = trace::modules::Start();

    if (!NT_SUCCESS(status)) goto done;

    status = FltStartFiltering(driver::pFilter);

done:

    if (!NT_SUCCESS(status)) {
        port::Close();

        if (driver::pFilter) {
            FltUnregisterFilter(driver::pFilter);
        }

        trace::modules::Delete();
        records::Delete();
    }

    return status;
}
