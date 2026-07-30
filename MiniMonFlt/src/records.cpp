#include "records.h"

#include "config.h"
#include "driver.h"

#include "..\..\inc\protocol.h"

#include <fltKernel.h>

using namespace mimo;

namespace {

    KSPIN_LOCK RecordListLock;
    LIST_ENTRY RecordList;
    NPAGED_LOOKASIDE_LIST EntryLookaside;
    volatile LONG RecordsAllocated;
    volatile LONG DroppedRecords;
    volatile LONG RecordSequenceNumber;

    records::Entry* AllocateEntry() {
        const LONG count = InterlockedIncrement(&RecordsAllocated);

        const ULONGLONG maxEntries = static_cast<ULONGLONG>(config::GetMaxMemoryKb()) * 1024ull / sizeof(records::Entry);

        if (static_cast<ULONGLONG>(count) > maxEntries) {
            InterlockedDecrement(&RecordsAllocated);

            return nullptr;
        }

        records::Entry* const pEntry = reinterpret_cast<records::Entry*>(ExAllocateFromNPagedLookasideList(&EntryLookaside));

        if (!pEntry) {
            InterlockedDecrement(&RecordsAllocated);

            return nullptr;
        }

        return pEntry;
    }


    void FreeEntry(_In_ records::Entry* pEntry) {
        InterlockedDecrement(&RecordsAllocated);
        ExFreeToNPagedLookasideList(&EntryLookaside, pEntry);

        return;
    }


    records::Entry* RemoveHead() {
        KIRQL oldIrql{};
        KeAcquireSpinLock(&RecordListLock, &oldIrql);

        records::Entry* pEntry = nullptr;

        if (!IsListEmpty(&RecordList)) {
            pEntry = CONTAINING_RECORD(RemoveHeadList(&RecordList), records::Entry, list);
        }

        KeReleaseSpinLock(&RecordListLock, oldIrql);

        return pEntry;
    }


    void InsertHead(_Inout_ records::Entry* pEntry) {
        KIRQL oldIrql{};
        KeAcquireSpinLock(&RecordListLock, &oldIrql);

        InsertHeadList(&RecordList, &pEntry->list);

        KeReleaseSpinLock(&RecordListLock, oldIrql);

        return;
    }

}

namespace mimo {

    namespace records {

        __declspec(code_seg("INIT"))
        void Create() {
            InitializeListHead(&RecordList);
            KeInitializeSpinLock(&RecordListLock);

            ExInitializeNPagedLookasideList(
                &EntryLookaside, nullptr, nullptr,
                POOL_NX_ALLOCATION, sizeof(Entry), driver::MEM_TAG, 0u
            );

            return;
        }


        Entry* CreateEntry() {
            Entry* const pEntry = AllocateEntry();

            if (!pEntry) {
                InterlockedIncrement(&DroppedRecords);

                return nullptr;
            }

            RtlZeroMemory(&pEntry->record, sizeof(pEntry->record));
            pEntry->record.sequenceNumber = InterlockedIncrement(&RecordSequenceNumber);

            return pEntry;
        }


        _Use_decl_annotations_
        void DeleteEntry(Entry* pEntry) {
            FreeEntry(pEntry);

            return;
        }


        _Use_decl_annotations_
        void Append(Entry* pEntry) {
            KIRQL oldIrql{};
            KeAcquireSpinLock(&RecordListLock, &oldIrql);

            pEntry->record.droppedRecords = DroppedRecords;
            InsertTailList(&RecordList, &pEntry->list);

            KeReleaseSpinLock(&RecordListLock, oldIrql);

            return;
        }


        _Use_decl_annotations_
        NTSTATUS Drain(
            UCHAR* pBuffer,
            ULONG size,
            ULONG* pBytesWritten
        ) {
            constexpr ULONG RECORD_STRIDE = static_cast<ULONG>(sizeof(protocol::Record));

            ULONG bytesWritten = 0u;
            bool tooSmall = false;

            for (Entry* pEntry = RemoveHead(); pEntry; pEntry = RemoveHead()) {

                if (size - bytesWritten < RECORD_STRIDE) {
                    InsertHead(pEntry);
                    tooSmall = true;

                    break;
                }

                __try {
                    RtlCopyMemory(pBuffer + bytesWritten, &pEntry->record, RECORD_STRIDE);
                }
                __except (EXCEPTION_EXECUTE_HANDLER) {
                    InsertHead(pEntry);
                    *pBytesWritten = bytesWritten;

                    return GetExceptionCode();
                }

                DeleteEntry(pEntry);

                bytesWritten += RECORD_STRIDE;
            }

            *pBytesWritten = bytesWritten;

            if (bytesWritten) return STATUS_SUCCESS;

            return tooSmall ? STATUS_BUFFER_TOO_SMALL : STATUS_NO_MORE_ENTRIES;
        }


        void Delete() {

            if (!RecordList.Flink) return;

            for (Entry* pEntry = RemoveHead(); pEntry; pEntry = RemoveHead()) {
                DeleteEntry(pEntry);
            }

            ExDeleteNPagedLookasideList(&EntryLookaside);

            return;
        }

    }

}
