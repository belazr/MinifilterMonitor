#pragma once

#include <stddef.h>

#ifdef _KERNEL_MODE

// kernel-mode CRT ships no <stdint.h>
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

        inline constexpr uint16_t MINIMON_MAJOR_VERSION = 0u;
        inline constexpr uint16_t MINIMON_MINOR_VERSION = 1u;
        inline constexpr uint16_t MINIMON_PATCH_VERSION = 0u;

        struct Version {
            uint16_t major;
            uint16_t minor;
            uint16_t patch;
        };

        static_assert(sizeof(Version) == 6u, "protocol::Version layout drift");

        typedef uint64_t ObjectId;

        // platform-free mirror of the kernel's FLT_PARAMETERS, member layout pinned by the driver
        union FltParameters {
            // IRP_MJ_CREATE
            struct {
                uint64_t securityContext;
                uint32_t options;           // low 24 bits create options, high 8 bits disposition
                uint8_t reserved1[4u];
                uint16_t fileAttributes;
                uint16_t shareAccess;
                uint8_t reserved2[4u];
                uint32_t eaLength;
                uint8_t reserved3[4u];
                uint64_t eaBuffer;
                int64_t allocationSize;
            } create;

            // IRP_MJ_READ / IRP_MJ_WRITE
            struct {
                uint32_t length;
                uint8_t reserved1[4u];
                uint32_t key;
                uint8_t reserved2[4u];
                int64_t byteOffset;
                uint64_t buffer;
                uint64_t mdlAddress;
            } readWrite;

            // IRP_MJ_QUERY_INFORMATION
            struct {
                uint32_t length;
                uint8_t reserved1[4u];
                uint32_t fileInformationClass;
                uint8_t reserved2[4u];
                uint64_t infoBuffer;
            } queryFileInformation;

            // IRP_MJ_SET_INFORMATION
            struct {
                uint32_t length;
                uint8_t reserved1[4u];
                uint32_t fileInformationClass;
                uint8_t reserved2[4u];
                uint64_t parentOfTarget;
                union {
                    struct {
                        uint8_t replaceIfExists;
                        uint8_t advanceOnly;
                    } flags;
                    uint32_t clusterCount;
                    uint64_t deleteHandle;
                };
                uint64_t infoBuffer;
            } setFileInformation;

            struct {
                uint64_t argument1;
                uint64_t argument2;
                uint64_t argument3;
                uint64_t argument4;
                uint64_t argument5;
                int64_t argument6;
            } others;
        };

        static_assert(sizeof(FltParameters) == 48u, "protocol::FltParameters layout drift");
        static_assert(offsetof(FltParameters, create.options) == 8u, "protocol::FltParameters layout drift");
        static_assert(offsetof(FltParameters, create.fileAttributes) == 16u, "protocol::FltParameters layout drift");
        static_assert(offsetof(FltParameters, create.shareAccess) == 18u, "protocol::FltParameters layout drift");
        static_assert(offsetof(FltParameters, create.eaLength) == 24u, "protocol::FltParameters layout drift");
        static_assert(offsetof(FltParameters, create.eaBuffer) == 32u, "protocol::FltParameters layout drift");
        static_assert(offsetof(FltParameters, create.allocationSize) == 40u, "protocol::FltParameters layout drift");
        static_assert(offsetof(FltParameters, readWrite.key) == 8u, "protocol::FltParameters layout drift");
        static_assert(offsetof(FltParameters, readWrite.byteOffset) == 16u, "protocol::FltParameters layout drift");
        static_assert(offsetof(FltParameters, readWrite.buffer) == 24u, "protocol::FltParameters layout drift");
        static_assert(offsetof(FltParameters, readWrite.mdlAddress) == 32u, "protocol::FltParameters layout drift");
        static_assert(offsetof(FltParameters, queryFileInformation.fileInformationClass) == 8u, "protocol::FltParameters layout drift");
        static_assert(offsetof(FltParameters, queryFileInformation.infoBuffer) == 16u, "protocol::FltParameters layout drift");
        static_assert(offsetof(FltParameters, setFileInformation.fileInformationClass) == 8u, "protocol::FltParameters layout drift");
        static_assert(offsetof(FltParameters, setFileInformation.parentOfTarget) == 16u, "protocol::FltParameters layout drift");
        static_assert(offsetof(FltParameters, setFileInformation.flags.replaceIfExists) == 24u, "protocol::FltParameters layout drift");
        static_assert(offsetof(FltParameters, setFileInformation.flags.advanceOnly) == 25u, "protocol::FltParameters layout drift");
        static_assert(offsetof(FltParameters, setFileInformation.clusterCount) == 24u, "protocol::FltParameters layout drift");
        static_assert(offsetof(FltParameters, setFileInformation.deleteHandle) == 24u, "protocol::FltParameters layout drift");
        static_assert(offsetof(FltParameters, setFileInformation.infoBuffer) == 32u, "protocol::FltParameters layout drift");

        inline constexpr uint32_t STACK_TRACE_FRAMES      = 8u;
        inline constexpr uint32_t STACK_FRAME_NAME_WCHARS = 32u;

        struct StackFrame {
            wchar_t moduleName[STACK_FRAME_NAME_WCHARS];
            uint64_t offset;
        };

        static_assert(sizeof(StackFrame) == 72u, "protocol::StackFrame layout drift");

        // per-operation supplements: data the parameters mirror cannot provide

        // IRP_MJ_CREATE

        // capture bits for CreateSupplement::captured
        inline constexpr uint32_t CREATE_CAPTURED_DESIRED_ACCESS   = 0x00000001u;
        inline constexpr uint32_t CREATE_CAPTURED_IMPERSONATED_SID = 0x00000002u;
        inline constexpr uint32_t CREATE_TRUNCATED_ECP_TEXT        = 0x00000004u;

        inline constexpr uint32_t SID_BYTES       = 68u;    // SECURITY_MAX_SID_SIZE, pinned by the driver
        inline constexpr uint32_t ECP_TEXT_WCHARS = 512u;

        struct CreateSupplement {
            uint32_t captured;
            uint32_t desiredAccess;
            uint8_t impersonatedSid[SID_BYTES];
            wchar_t ecpText[ECP_TEXT_WCHARS];
        };

        static_assert(offsetof(CreateSupplement, impersonatedSid) == 8u, "protocol::CreateSupplement layout drift");
        static_assert(offsetof(CreateSupplement, ecpText) == 76u, "protocol::CreateSupplement layout drift");

        // IRP_MJ_QUERY_INFORMATION

        // capture bit for QueryInfoSupplement::captured
        inline constexpr uint32_t QUERY_INFO_CAPTURED_PAYLOAD = 0x00000001u;

        inline constexpr uint32_t QUERY_INFO_PAYLOAD_BYTES = SID_BYTES + 2u * ECP_TEXT_WCHARS;

        struct QueryInfoSupplement {
            uint32_t captured;
            uint32_t capturedBytes;
            uint8_t payload[QUERY_INFO_PAYLOAD_BYTES];
        };

        static_assert(offsetof(QueryInfoSupplement, payload) == 8u, "protocol::QueryInfoSupplement layout drift");

        // IRP_MJ_SET_INFORMATION

        // capture bits for SetInfoSupplement::captured
        inline constexpr uint32_t SET_INFO_CAPTURED_PAYLOAD      = 0x00000001u;
        inline constexpr uint32_t SET_INFO_CAPTURED_TARGET_NAME  = 0x00000002u;
        inline constexpr uint32_t SET_INFO_TRUNCATED_TARGET_NAME = 0x00000004u;

        inline constexpr uint32_t TARGET_NAME_WCHARS = 256u;

        inline constexpr uint32_t SET_INFO_PAYLOAD_BYTES = QUERY_INFO_PAYLOAD_BYTES - 2u * TARGET_NAME_WCHARS;

        struct SetInfoSupplement {
            uint32_t captured;
            uint32_t capturedBytes;
            uint8_t payload[SET_INFO_PAYLOAD_BYTES];
            wchar_t targetName[TARGET_NAME_WCHARS];
        };

        static_assert(offsetof(SetInfoSupplement, payload) == 8u, "protocol::SetInfoSupplement layout drift");
        static_assert(offsetof(SetInfoSupplement, targetName) == 588u, "protocol::SetInfoSupplement layout drift");

        union Supplement {
            CreateSupplement create;
            QueryInfoSupplement queryInfo;
            SetInfoSupplement setInfo;
        };

        static_assert(sizeof(CreateSupplement) == 1100u, "protocol::CreateSupplement layout drift");
        static_assert(sizeof(QueryInfoSupplement) == 1100u, "protocol::QueryInfoSupplement layout drift");
        static_assert(sizeof(SetInfoSupplement) == 1100u, "protocol::SetInfoSupplement layout drift");
        static_assert(sizeof(Supplement) == 1100u, "protocol::Supplement layout drift");

        // truncation bit for RecordData::truncated
        inline constexpr uint8_t TRUNCATED_NAME = 0x01u;

        inline constexpr uint32_t NAME_WCHARS = 512u;

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
            uint8_t truncated;              // TRUNCATED_* bits
            uint8_t reserved[4u];
            uint32_t transactionNotify;     // raw TRANSACTION_NOTIFY_* code, non-zero marks a transaction lifecycle event, not an operation
            uint32_t transactionSequence;   // our per-transaction id, 0 if the operation is not transacted
            FltParameters parameters;
            uint32_t altitude;
            uint32_t stackFrameCount;
            StackFrame stackTrace[STACK_TRACE_FRAMES];
            wchar_t name[NAME_WCHARS];
            Supplement supplement;
        };

        static_assert(sizeof(RecordData) == 2872u, "protocol::RecordData layout drift");
        static_assert(offsetof(RecordData, deviceObject) == 16u, "protocol::RecordData layout drift");
        static_assert(offsetof(RecordData, transactionNotify) == 104u, "protocol::RecordData layout drift");
        static_assert(offsetof(RecordData, transactionSequence) == 108u, "protocol::RecordData layout drift");
        static_assert(offsetof(RecordData, parameters) == 112u, "protocol::RecordData layout drift");
        static_assert(offsetof(RecordData, stackTrace) == 168u, "protocol::RecordData layout drift");
        static_assert(offsetof(RecordData, name) == 744u, "protocol::RecordData layout drift");
        static_assert(offsetof(RecordData, supplement) == 1768u, "protocol::RecordData layout drift");

        struct Record {
            uint32_t sequenceNumber;
            uint32_t droppedRecords;
            uint8_t reserved[8u];
            RecordData data;
        };

        static_assert(sizeof(Record) == 2888u, "protocol::Record layout drift");
        static_assert(offsetof(Record, data) == 16u, "protocol::Record layout drift");

        enum class Command : uint32_t {
            GetRecords,
            GetVersion,
        };

        struct CommandMessage {
            Command command;
            uint32_t reserved;
        };

        static_assert(sizeof(CommandMessage) == 8u, "protocol::CommandMessage layout drift");

    }

}
