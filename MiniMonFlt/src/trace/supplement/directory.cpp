#include "directory.h"

#include "..\..\memory.h"

#include "..\..\..\..\inc\protocol.h"

#include <fltKernel.h>
#include <ntstrsafe.h>

namespace mimo {

    namespace trace {

        namespace supplement {

            namespace directory {

                _Use_decl_annotations_
                void PopulateFileName(protocol::QueryDirectorySupplement* pSupplement, const FLT_CALLBACK_DATA* pData) {

                    if (KeGetCurrentIrql() != PASSIVE_LEVEL) return;

                    const UNICODE_STRING* const pFileName = pData->Iopb->Parameters.DirectoryControl.QueryDirectory.FileName;

                    if (!pFileName) return;

                    UNICODE_STRING fileName{};
                    RtlInitEmptyUnicodeString(&fileName, pSupplement->fileName, static_cast<USHORT>(sizeof(pSupplement->fileName)));

                    const NTSTATUS status = RtlUnicodeStringCopy(&fileName, pFileName);

                    if (!NT_SUCCESS(status) && status != STATUS_BUFFER_OVERFLOW) return;

                    if (status == STATUS_BUFFER_OVERFLOW) {
                        pSupplement->captured |= protocol::QUERY_DIRECTORY_TRUNCATED_FILE_NAME;
                    }

                    pSupplement->captured |= protocol::QUERY_DIRECTORY_CAPTURED_FILE_NAME;

                    return;
                }


                _Use_decl_annotations_
                void PopulateQuery(protocol::QueryDirectorySupplement* pSupplement, const FLT_CALLBACK_DATA* pData) {
                    const ULONG bufferSize = pData->Iopb->Parameters.DirectoryControl.QueryDirectory.Length;
                    const ULONG_PTR writtenSize = pData->IoStatus.Information;

                    if (!bufferSize || !writtenSize) return;

                    const ULONG dataSize = writtenSize < bufferSize ? static_cast<ULONG>(writtenSize) : bufferSize;
                    ULONG readableSize = dataSize;
                    const void* const pDirectoryBuffer = memory::GetReadableBuffer(pData, pData->Iopb->Parameters.DirectoryControl.QueryDirectory.MdlAddress, pData->Iopb->Parameters.DirectoryControl.QueryDirectory.DirectoryBuffer, &readableSize);

                    if (!pDirectoryBuffer || !readableSize) return;

                    const ULONG copySize = readableSize < sizeof(pSupplement->payload) ? readableSize : sizeof(pSupplement->payload);

                    if (!memory::CopyReadableBuffer(pSupplement->payload, pDirectoryBuffer, copySize)) return;

                    if (copySize < dataSize) {
                        pSupplement->captured |= protocol::QUERY_DIRECTORY_TRUNCATED_PAYLOAD;
                    }

                    pSupplement->capturedSize = static_cast<uint32_t>(copySize);
                    pSupplement->captured |= protocol::QUERY_DIRECTORY_CAPTURED_PAYLOAD;

                    return;
                }


                _Use_decl_annotations_
                void PopulateNotify(protocol::NotifyDirectorySupplement* pSupplement, const FLT_CALLBACK_DATA* pData) {
                    const ULONG bufferSize = pData->Iopb->Parameters.DirectoryControl.NotifyDirectory.Length;
                    const ULONG_PTR writtenSize = pData->IoStatus.Information;

                    if (!bufferSize || !writtenSize) return;

                    const ULONG dataSize = writtenSize < bufferSize ? static_cast<ULONG>(writtenSize) : bufferSize;
                    ULONG readableSize = dataSize;
                    const void* const pDirectoryBuffer = memory::GetReadableBuffer(pData, pData->Iopb->Parameters.DirectoryControl.NotifyDirectory.MdlAddress, pData->Iopb->Parameters.DirectoryControl.NotifyDirectory.DirectoryBuffer, &readableSize);

                    if (!pDirectoryBuffer || !readableSize) return;

                    const ULONG copySize = readableSize < sizeof(pSupplement->payload) ? readableSize : sizeof(pSupplement->payload);

                    if (!memory::CopyReadableBuffer(pSupplement->payload, pDirectoryBuffer, copySize)) return;

                    if (copySize < dataSize) {
                        pSupplement->captured |= protocol::NOTIFY_DIRECTORY_TRUNCATED_PAYLOAD;
                    }

                    pSupplement->capturedSize = static_cast<uint32_t>(copySize);
                    pSupplement->captured |= protocol::NOTIFY_DIRECTORY_CAPTURED_PAYLOAD;

                    return;
                }

            }

        }

    }

}
