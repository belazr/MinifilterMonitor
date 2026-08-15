#include "supplement.h"

#include "supplement\create.h"
#include "supplement\deviceio.h"
#include "supplement\directory.h"
#include "supplement\filesystem.h"
#include "supplement\info.h"
#include "supplement\volume.h"

#include "..\..\..\inc\protocol.h"

#include <fltKernel.h>

namespace mimo {

    namespace trace {

        namespace supplement {

            _Use_decl_annotations_
            void PopulatePreOperation(
                protocol::Supplement* pSupplement,
                FLT_CALLBACK_DATA* pData,
                const FLT_RELATED_OBJECTS* pFltObjects
            ) {

                switch (pData->Iopb->MajorFunction) {

                    case IRP_MJ_CREATE:

                        if (KeGetCurrentIrql() == PASSIVE_LEVEL) {
                            create::Populate(&pSupplement->create, pData);
                        }

                        break;

                    case IRP_MJ_SET_INFORMATION:

                        if (KeGetCurrentIrql() < DISPATCH_LEVEL && pData->Iopb->Parameters.SetFileInformation.InfoBuffer) {
                            info::PopulateSet(&pSupplement->setInfo, pData, pFltObjects);
                        }

                        break;

                    case IRP_MJ_SET_VOLUME_INFORMATION:

                        if (KeGetCurrentIrql() < DISPATCH_LEVEL && pData->Iopb->Parameters.SetVolumeInformation.VolumeBuffer) {
                            volume::PopulateSet(&pSupplement->volumeInfo, pData);
                        }

                        break;

                    case IRP_MJ_DIRECTORY_CONTROL:

                        if (pData->Iopb->MinorFunction == IRP_MN_QUERY_DIRECTORY && KeGetCurrentIrql() < DISPATCH_LEVEL && pData->Iopb->Parameters.DirectoryControl.QueryDirectory.FileName) {
                            directory::PopulateFileName(&pSupplement->queryDirectory, pData);
                        }

                        break;

                    case IRP_MJ_FILE_SYSTEM_CONTROL:

                        if ((pData->Iopb->MinorFunction == IRP_MN_USER_FS_REQUEST || pData->Iopb->MinorFunction == IRP_MN_KERNEL_CALL) && KeGetCurrentIrql() < DISPATCH_LEVEL && (pData->Iopb->Parameters.FileSystemControl.Common.InputBufferLength || (METHOD_FROM_CTL_CODE(pData->Iopb->Parameters.FileSystemControl.Common.FsControlCode) == METHOD_IN_DIRECT && pData->Iopb->Parameters.FileSystemControl.Common.OutputBufferLength))) {
                            filesystem::PopulateInput(&pSupplement->fsControl, pData);
                        }

                        break;

                    case IRP_MJ_DEVICE_CONTROL:
                    case IRP_MJ_INTERNAL_DEVICE_CONTROL:

                        if (KeGetCurrentIrql() < DISPATCH_LEVEL && (pData->Iopb->Parameters.DeviceIoControl.Common.InputBufferLength || pData->Iopb->Parameters.DeviceIoControl.Common.OutputBufferLength)) {
                            deviceio::PopulateInput(&pSupplement->deviceIoControl, pData);
                        }

                        break;

                }

                return;
            }


            _Use_decl_annotations_
            void PopulatePostOperation(protocol::Supplement* pSupplement, const FLT_CALLBACK_DATA* pData) {

                switch (pData->Iopb->MajorFunction) {

                    case IRP_MJ_QUERY_INFORMATION:

                        if ((NT_SUCCESS(pData->IoStatus.Status) || pData->IoStatus.Status == STATUS_BUFFER_OVERFLOW) && KeGetCurrentIrql() < DISPATCH_LEVEL && pData->Iopb->Parameters.QueryFileInformation.InfoBuffer) {
                            info::PopulateQuery(&pSupplement->queryInfo, pData);
                        }

                        break;

                    case IRP_MJ_QUERY_VOLUME_INFORMATION:

                        if ((NT_SUCCESS(pData->IoStatus.Status) || pData->IoStatus.Status == STATUS_BUFFER_OVERFLOW) && KeGetCurrentIrql() < DISPATCH_LEVEL && pData->Iopb->Parameters.QueryVolumeInformation.VolumeBuffer) {
                            volume::PopulateQuery(&pSupplement->volumeInfo, pData);
                        }

                        break;

                    case IRP_MJ_DIRECTORY_CONTROL:

                        if (pData->Iopb->MinorFunction == IRP_MN_QUERY_DIRECTORY && (NT_SUCCESS(pData->IoStatus.Status) || pData->IoStatus.Status == STATUS_BUFFER_OVERFLOW) && KeGetCurrentIrql() < DISPATCH_LEVEL && pData->Iopb->Parameters.DirectoryControl.QueryDirectory.Length) {
                            directory::PopulatePayload(&pSupplement->queryDirectory, pData);
                        }

                        break;

                    case IRP_MJ_FILE_SYSTEM_CONTROL:

                        if ((pData->Iopb->MinorFunction == IRP_MN_USER_FS_REQUEST || pData->Iopb->MinorFunction == IRP_MN_KERNEL_CALL) && (NT_SUCCESS(pData->IoStatus.Status) || pData->IoStatus.Status == STATUS_BUFFER_OVERFLOW) && KeGetCurrentIrql() < DISPATCH_LEVEL && pData->Iopb->Parameters.FileSystemControl.Common.OutputBufferLength) {
                            filesystem::PopulateOutput(&pSupplement->fsControl, pData);
                        }

                        break;

                    case IRP_MJ_DEVICE_CONTROL:
                    case IRP_MJ_INTERNAL_DEVICE_CONTROL:

                        if ((NT_SUCCESS(pData->IoStatus.Status) || pData->IoStatus.Status == STATUS_BUFFER_OVERFLOW) && KeGetCurrentIrql() < DISPATCH_LEVEL && pData->Iopb->Parameters.DeviceIoControl.Common.OutputBufferLength) {
                            deviceio::PopulateOutput(&pSupplement->deviceIoControl, pData);
                        }

                        break;

                }

                return;
            }

        }

    }

}
