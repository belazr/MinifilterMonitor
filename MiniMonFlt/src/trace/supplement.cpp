#include "supplement.h"

#include "supplement\create.h"
#include "supplement\deviceio.h"
#include "supplement\directory.h"
#include "supplement\filesystem.h"
#include "supplement\info.h"
#include "supplement\security.h"
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

                if (KeGetCurrentIrql() >= DISPATCH_LEVEL) return;

                switch (pData->Iopb->MajorFunction) {

                    case IRP_MJ_CREATE:

                        create::Populate(&pSupplement->create, pData);

                        break;

                    case IRP_MJ_SET_INFORMATION:

                        if (pData->Iopb->Parameters.SetFileInformation.InfoBuffer) {
                            info::PopulateSet(&pSupplement->setInfo, pData, pFltObjects);
                        }

                        break;

                    case IRP_MJ_SET_VOLUME_INFORMATION:

                        if (pData->Iopb->Parameters.SetVolumeInformation.VolumeBuffer) {
                            volume::PopulateSet(&pSupplement->volumeInfo, pData);
                        }

                        break;

                    case IRP_MJ_DIRECTORY_CONTROL:

                        if (pData->Iopb->MinorFunction == IRP_MN_QUERY_DIRECTORY && pData->Iopb->Parameters.DirectoryControl.QueryDirectory.FileName) {
                            directory::PopulateFileName(&pSupplement->queryDirectory, pData);
                        }

                        break;

                    case IRP_MJ_FILE_SYSTEM_CONTROL:

                        if ((pData->Iopb->MinorFunction == IRP_MN_USER_FS_REQUEST || pData->Iopb->MinorFunction == IRP_MN_KERNEL_CALL) && (pData->Iopb->Parameters.FileSystemControl.Common.InputBufferLength || pData->Iopb->Parameters.FileSystemControl.Common.OutputBufferLength)) {
                            filesystem::PopulateInput(&pSupplement->fsControl, pData);
                        }

                        break;

                    case IRP_MJ_DEVICE_CONTROL:
                    case IRP_MJ_INTERNAL_DEVICE_CONTROL:

                        if (pData->Iopb->Parameters.DeviceIoControl.Common.InputBufferLength || pData->Iopb->Parameters.DeviceIoControl.Common.OutputBufferLength) {
                            deviceio::PopulateInput(&pSupplement->deviceIoControl, pData);
                        }

                        break;

                    case IRP_MJ_SET_SECURITY:

                        if (pData->Iopb->Parameters.SetSecurity.SecurityDescriptor) {
                            security::PopulateSet(&pSupplement->security, pData);
                        }

                        break;

                }

                return;
            }


            _Use_decl_annotations_
            void PopulatePostOperation(protocol::Supplement* pSupplement, const FLT_CALLBACK_DATA* pData) {
                const NTSTATUS status = pData->IoStatus.Status;

                if (!NT_SUCCESS(status) && status != STATUS_BUFFER_OVERFLOW) return;

                if (!pData->IoStatus.Information) return;

                if (KeGetCurrentIrql() >= DISPATCH_LEVEL) return;

                switch (pData->Iopb->MajorFunction) {

                    case IRP_MJ_QUERY_INFORMATION:

                        if (pData->Iopb->Parameters.QueryFileInformation.InfoBuffer) {
                            info::PopulateQuery(&pSupplement->queryInfo, pData);
                        }

                        break;

                    case IRP_MJ_QUERY_VOLUME_INFORMATION:

                        if (pData->Iopb->Parameters.QueryVolumeInformation.VolumeBuffer) {
                            volume::PopulateQuery(&pSupplement->volumeInfo, pData);
                        }

                        break;

                    case IRP_MJ_DIRECTORY_CONTROL:

                        if (pData->Iopb->MinorFunction == IRP_MN_QUERY_DIRECTORY && pData->Iopb->Parameters.DirectoryControl.QueryDirectory.Length) {
                            directory::PopulatePayload(&pSupplement->queryDirectory, pData);
                        }

                        break;

                    case IRP_MJ_FILE_SYSTEM_CONTROL:

                        if ((pData->Iopb->MinorFunction == IRP_MN_USER_FS_REQUEST || pData->Iopb->MinorFunction == IRP_MN_KERNEL_CALL) && pData->Iopb->Parameters.FileSystemControl.Common.OutputBufferLength) {
                            filesystem::PopulateOutput(&pSupplement->fsControl, pData);
                        }

                        break;

                    case IRP_MJ_DEVICE_CONTROL:
                    case IRP_MJ_INTERNAL_DEVICE_CONTROL:

                        if (pData->Iopb->Parameters.DeviceIoControl.Common.OutputBufferLength) {
                            deviceio::PopulateOutput(&pSupplement->deviceIoControl, pData);
                        }

                        break;

                    case IRP_MJ_QUERY_SECURITY:

                        if (pData->Iopb->Parameters.QuerySecurity.Length) {
                            security::PopulateQuery(&pSupplement->security, pData);
                        }

                        break;

                }

                return;
            }

        }

    }

}
