#pragma once

#include <stddef.h>

#ifdef _KERNEL_MODE

// kernel-mode CRT ships no <stdint.h> - define the fixed-width types from built-ins
typedef unsigned __int8  uint8_t;
typedef unsigned __int16 uint16_t;
typedef unsigned __int32 uint32_t;
typedef unsigned __int64 uint64_t;
typedef __int32          int32_t;
typedef __int64          int64_t;

#else

#include <stdint.h>

#endif // _KERNEL_MODE

#define PORT_NAME L"\\MiniMonPort"

namespace mimo {

    namespace protocol {

        inline constexpr uint16_t MINIMON_MAJ_VERSION   = 0u;
        inline constexpr uint16_t MINIMON_MIN_VERSION   = 1u;
        inline constexpr uint16_t MINIMON_PATCH_VERSION = 0u;

        struct Version {
            uint16_t major;
            uint16_t minor;
            uint16_t patch;
        };

        typedef uint64_t ObjectId;

        // the kernel's FLT_PARAMETERS union as a platform-free mirror
        // the driver pins each mirrored member against FLT_PARAMETERS
        union FltParameters {

            struct {
                uint64_t argument1;
                uint64_t argument2;
                uint64_t argument3;
                uint64_t argument4;
                uint64_t argument5;
                int64_t argument6;
            } others;

            // IRP_MJ_READ / IRP_MJ_WRITE
            struct {
                uint32_t length;
                uint8_t reserved1[4];
                uint32_t key;
                uint8_t reserved2[4];
                int64_t byteOffset;
                uint64_t buffer;
                uint64_t mdlAddress;
            } readWrite;

        };

        static_assert(sizeof(FltParameters) == 48u, "protocol::FltParameters layout drift");
        static_assert(offsetof(FltParameters, readWrite.key) == 8u, "protocol::FltParameters layout drift");
        static_assert(offsetof(FltParameters, readWrite.byteOffset) == 16u, "protocol::FltParameters layout drift");
        static_assert(offsetof(FltParameters, readWrite.buffer) == 24u, "protocol::FltParameters layout drift");
        static_assert(offsetof(FltParameters, readWrite.mdlAddress) == 32u, "protocol::FltParameters layout drift");

        inline constexpr uint32_t NAME_WCHARS = 512u;

        inline constexpr uint32_t STACK_TRACE_FRAMES = 8u;
        inline constexpr uint32_t STACK_FRAME_NAME_WCHARS = 32u;

        struct StackFrame {
            wchar_t moduleName[STACK_FRAME_NAME_WCHARS];
            uint64_t offset;
        };

        struct RecordData {
            int64_t originatingTime;        // 100 ns ticks since 1601
            int64_t completionTime;         // 100 ns ticks since 1601
            ObjectId deviceObject;
            ObjectId fileObject;
            ObjectId transaction;
            ObjectId processId;
            ObjectId threadId;
            ObjectId operationId;
            ObjectId topLevelIrp;
            uint64_t information;
            int32_t status;                 // NTSTATUS
            uint32_t irpFlags;
            uint32_t flags;
            uint32_t reparseTag;            // IO_REPARSE_TAG_* of a reparse-point hit, 0 if none
            uint8_t callbackMajorId;
            uint8_t callbackMinorId;
            uint8_t operationFlags;         // IRP stack-location SL_* flags
            uint8_t reserved[5];
            uint32_t transactionNotify;     // raw TRANSACTION_NOTIFY_* code, non-zero marks a transaction lifecycle event, not an operation
            uint32_t transactionSequence;   // our per-transaction id, 0 if the operation is not transacted
            FltParameters parameters;
            uint32_t altitude;
            uint32_t stackFrameCount;
            StackFrame stackTrace[STACK_TRACE_FRAMES];
            wchar_t name[NAME_WCHARS];
        };

        struct Record {
            uint32_t sequenceNumber;
            uint32_t droppedRecords;
            uint8_t reserved[8];
            RecordData data;
        };

        static_assert(sizeof(Version) == 6u, "protocol::Version layout drift");
        static_assert(sizeof(StackFrame) == 72u, "protocol::StackFrame layout drift");
        static_assert(sizeof(RecordData) == 1768u, "protocol::RecordData layout drift");
        static_assert(sizeof(Record) == 1784u, "protocol::Record layout drift");
        static_assert(offsetof(RecordData, deviceObject) == 16u, "protocol::RecordData layout drift");
        static_assert(offsetof(RecordData, transactionNotify) == 104u, "protocol::RecordData layout drift");
        static_assert(offsetof(RecordData, transactionSequence) == 108u, "protocol::RecordData layout drift");
        static_assert(offsetof(RecordData, parameters) == 112u, "protocol::RecordData layout drift");
        static_assert(offsetof(RecordData, stackTrace) == 168u, "protocol::RecordData layout drift");
        static_assert(offsetof(RecordData, name) == 744u, "protocol::RecordData layout drift");
        static_assert(offsetof(Record, data) == 16u, "protocol::Record layout drift");

        enum class Command : uint32_t {
            GetRecords,
            GetVersion,
        };

        struct CommandMessage {
            Command command;
            uint32_t reserved;
        };

    }

}
