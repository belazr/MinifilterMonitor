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

            // IRP_MJ_QUERY_VOLUME_INFORMATION / IRP_MJ_SET_VOLUME_INFORMATION
            struct {
                uint32_t length;
                uint8_t reserved1[4u];
                uint32_t fsInformationClass;
                uint8_t reserved2[4u];
                uint64_t volumeBuffer;
            } volumeInformation;

            // IRP_MJ_DIRECTORY_CONTROL / IRP_MN_QUERY_DIRECTORY
            struct {
                uint32_t length;
                uint8_t reserved1[4u];
                uint64_t fileName;
                uint32_t fileInformationClass;
                uint8_t reserved2[4u];
                uint32_t fileIndex;
                uint8_t reserved3[4u];
                uint64_t directoryBuffer;
                uint64_t mdlAddress;
            } queryDirectory;

            // IRP_MJ_DIRECTORY_CONTROL / IRP_MN_NOTIFY_CHANGE_DIRECTORY / IRP_MN_NOTIFY_CHANGE_DIRECTORY_EX
            struct {
                uint32_t length;
                uint8_t reserved1[4u];
                uint32_t completionFilter;
                uint8_t reserved2[4u];
                uint32_t directoryNotifyInformationClass;   // IRP_MN_NOTIFY_CHANGE_DIRECTORY_EX only
                uint8_t reserved3[12u];
                uint64_t directoryBuffer;
                uint64_t mdlAddress;
            } notifyDirectory;

            // IRP_MJ_FILE_SYSTEM_CONTROL / IRP_MN_USER_FS_REQUEST / IRP_MN_KERNEL_CALL
            struct {
                uint32_t outputBufferLength;
                uint8_t reserved1[4u];
                uint32_t inputBufferLength;
                uint8_t reserved2[4u];
                uint32_t fsControlCode;
                uint8_t reserved3[4u];
                uint64_t inputBuffer;       // METHOD_BUFFERED: the shared input/output system buffer
                uint64_t outputBuffer;
                uint64_t outputMdlAddress;
            } fileSystemControl;

            // IRP_MJ_DEVICE_CONTROL / IRP_MJ_INTERNAL_DEVICE_CONTROL
            struct {
                uint32_t outputBufferLength;
                uint8_t reserved1[4u];
                uint32_t inputBufferLength;
                uint8_t reserved2[4u];
                uint32_t ioControlCode;
                uint8_t reserved3[4u];
                uint64_t inputBuffer;       // METHOD_BUFFERED: the shared input/output system buffer
                uint64_t outputBuffer;
                uint64_t outputMdlAddress;
            } deviceIoControl;

            // IRP_MJ_LOCK_CONTROL
            struct {
                uint64_t length;
                uint32_t key;
                uint8_t reserved1[4u];
                int64_t byteOffset;
                uint64_t processId;         // fast I/O only
                uint8_t failImmediately;    // fast I/O only (IRP operations carry SL_FAIL_IMMEDIATELY)
                uint8_t exclusiveLock;      // fast I/O only (IRP operations carry SL_EXCLUSIVE_LOCK)
            } lockControl;

            // IRP_MJ_QUERY_SECURITY
            struct {
                uint32_t securityInformation;
                uint8_t reserved1[4u];
                uint32_t length;
                uint8_t reserved2[4u];
                uint64_t securityBuffer;
                uint64_t mdlAddress;
            } querySecurity;

            // IRP_MJ_SET_SECURITY
            struct {
                uint32_t securityInformation;
                uint8_t reserved1[4u];
                uint64_t securityDescriptor;
            } setSecurity;

            // IRP_MJ_ACQUIRE_FOR_SECTION_SYNCHRONIZATION
            struct {
                uint32_t syncType;
                uint32_t pageProtection;
                uint64_t outputInformation;
                uint32_t flags;
                uint32_t allocationAttributes;    // SyncTypeCreateSection only
            } acquireForSectionSynchronization;

            // IRP_MJ_FAST_IO_CHECK_IF_POSSIBLE
            struct {
                int64_t fileOffset;
                uint32_t length;
                uint8_t reserved1[4u];
                uint32_t lockKey;
                uint8_t reserved2[4u];
                uint8_t checkForReadOperation;
            } fastIoCheckIfPossible;

            // IRP_MJ_MDL_READ / IRP_MJ_PREPARE_MDL_WRITE
            struct {
                int64_t fileOffset;
                uint32_t length;
                uint8_t reserved1[4u];
                uint32_t key;
                uint8_t reserved2[4u];
                uint64_t mdlChain;
            } mdlReadWrite;

            // IRP_MJ_MDL_READ_COMPLETE
            struct {
                uint64_t mdlChain;
            } mdlReadComplete;

            // IRP_MJ_MDL_WRITE_COMPLETE
            struct {
                int64_t fileOffset;
                uint64_t mdlChain;
            } mdlWriteComplete;

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
        static_assert(offsetof(FltParameters, volumeInformation.fsInformationClass) == 8u, "protocol::FltParameters layout drift");
        static_assert(offsetof(FltParameters, volumeInformation.volumeBuffer) == 16u, "protocol::FltParameters layout drift");
        static_assert(offsetof(FltParameters, queryDirectory.fileName) == 8u, "protocol::FltParameters layout drift");
        static_assert(offsetof(FltParameters, queryDirectory.fileInformationClass) == 16u, "protocol::FltParameters layout drift");
        static_assert(offsetof(FltParameters, queryDirectory.fileIndex) == 24u, "protocol::FltParameters layout drift");
        static_assert(offsetof(FltParameters, queryDirectory.directoryBuffer) == 32u, "protocol::FltParameters layout drift");
        static_assert(offsetof(FltParameters, queryDirectory.mdlAddress) == 40u, "protocol::FltParameters layout drift");
        static_assert(offsetof(FltParameters, notifyDirectory.completionFilter) == 8u, "protocol::FltParameters layout drift");
        static_assert(offsetof(FltParameters, notifyDirectory.directoryNotifyInformationClass) == 16u, "protocol::FltParameters layout drift");
        static_assert(offsetof(FltParameters, notifyDirectory.directoryBuffer) == 32u, "protocol::FltParameters layout drift");
        static_assert(offsetof(FltParameters, notifyDirectory.mdlAddress) == 40u, "protocol::FltParameters layout drift");
        static_assert(offsetof(FltParameters, fileSystemControl.inputBufferLength) == 8u, "protocol::FltParameters layout drift");
        static_assert(offsetof(FltParameters, fileSystemControl.fsControlCode) == 16u, "protocol::FltParameters layout drift");
        static_assert(offsetof(FltParameters, fileSystemControl.inputBuffer) == 24u, "protocol::FltParameters layout drift");
        static_assert(offsetof(FltParameters, fileSystemControl.outputBuffer) == 32u, "protocol::FltParameters layout drift");
        static_assert(offsetof(FltParameters, fileSystemControl.outputMdlAddress) == 40u, "protocol::FltParameters layout drift");
        static_assert(offsetof(FltParameters, deviceIoControl.inputBufferLength) == 8u, "protocol::FltParameters layout drift");
        static_assert(offsetof(FltParameters, deviceIoControl.ioControlCode) == 16u, "protocol::FltParameters layout drift");
        static_assert(offsetof(FltParameters, deviceIoControl.inputBuffer) == 24u, "protocol::FltParameters layout drift");
        static_assert(offsetof(FltParameters, deviceIoControl.outputBuffer) == 32u, "protocol::FltParameters layout drift");
        static_assert(offsetof(FltParameters, deviceIoControl.outputMdlAddress) == 40u, "protocol::FltParameters layout drift");
        static_assert(offsetof(FltParameters, lockControl.key) == 8u, "protocol::FltParameters layout drift");
        static_assert(offsetof(FltParameters, lockControl.byteOffset) == 16u, "protocol::FltParameters layout drift");
        static_assert(offsetof(FltParameters, lockControl.processId) == 24u, "protocol::FltParameters layout drift");
        static_assert(offsetof(FltParameters, lockControl.failImmediately) == 32u, "protocol::FltParameters layout drift");
        static_assert(offsetof(FltParameters, lockControl.exclusiveLock) == 33u, "protocol::FltParameters layout drift");
        static_assert(offsetof(FltParameters, querySecurity.length) == 8u, "protocol::FltParameters layout drift");
        static_assert(offsetof(FltParameters, querySecurity.securityBuffer) == 16u, "protocol::FltParameters layout drift");
        static_assert(offsetof(FltParameters, querySecurity.mdlAddress) == 24u, "protocol::FltParameters layout drift");
        static_assert(offsetof(FltParameters, setSecurity.securityDescriptor) == 8u, "protocol::FltParameters layout drift");
        static_assert(offsetof(FltParameters, acquireForSectionSynchronization.pageProtection) == 4u, "protocol::FltParameters layout drift");
        static_assert(offsetof(FltParameters, acquireForSectionSynchronization.outputInformation) == 8u, "protocol::FltParameters layout drift");
        static_assert(offsetof(FltParameters, acquireForSectionSynchronization.flags) == 16u, "protocol::FltParameters layout drift");
        static_assert(offsetof(FltParameters, acquireForSectionSynchronization.allocationAttributes) == 20u, "protocol::FltParameters layout drift");
        static_assert(offsetof(FltParameters, fastIoCheckIfPossible.length) == 8u, "protocol::FltParameters layout drift");
        static_assert(offsetof(FltParameters, fastIoCheckIfPossible.lockKey) == 16u, "protocol::FltParameters layout drift");
        static_assert(offsetof(FltParameters, fastIoCheckIfPossible.checkForReadOperation) == 24u, "protocol::FltParameters layout drift");
        static_assert(offsetof(FltParameters, mdlReadWrite.length) == 8u, "protocol::FltParameters layout drift");
        static_assert(offsetof(FltParameters, mdlReadWrite.key) == 16u, "protocol::FltParameters layout drift");
        static_assert(offsetof(FltParameters, mdlReadWrite.mdlChain) == 24u, "protocol::FltParameters layout drift");
        static_assert(offsetof(FltParameters, mdlWriteComplete.mdlChain) == 8u, "protocol::FltParameters layout drift");

        inline constexpr uint32_t STACK_TRACE_FRAMES      = 8u;
        inline constexpr uint32_t STACK_FRAME_NAME_WCHARS = 32u;

        struct StackFrame {
            wchar_t moduleName[STACK_FRAME_NAME_WCHARS];
            uint64_t offset;
        };

        static_assert(sizeof(StackFrame) == 72u, "protocol::StackFrame layout drift");

        // per-operation supplements: data the parameters mirror cannot provide

        // supplements with an elastic capacity fill the union exactly
        // fixed-member-only supplements stay at their natural size
        inline constexpr uint32_t SUPPLEMENT_BYTES = 1104u;

        // IRP_MJ_CREATE

        // capture bits for CreateSupplement::captured
        inline constexpr uint32_t CREATE_CAPTURED_DESIRED_ACCESS   = 0x00000001u;
        inline constexpr uint32_t CREATE_CAPTURED_IMPERSONATED_SID = 0x00000002u;
        inline constexpr uint32_t CREATE_TRUNCATED_ECP_TEXT        = 0x00000004u;

        inline constexpr uint32_t CREATE_SID_BYTES       = 68u;    // SECURITY_MAX_SID_SIZE, pinned by the driver
        inline constexpr uint32_t CREATE_ECP_TEXT_WCHARS = (SUPPLEMENT_BYTES - 2u * sizeof(uint32_t) - CREATE_SID_BYTES) / sizeof(wchar_t);

        struct CreateSupplement {
            uint32_t captured;
            uint32_t desiredAccess;
            uint8_t impersonatedSid[CREATE_SID_BYTES];
            wchar_t ecpText[CREATE_ECP_TEXT_WCHARS];
        };

        static_assert(offsetof(CreateSupplement, impersonatedSid) == 8u, "protocol::CreateSupplement layout drift");
        static_assert(offsetof(CreateSupplement, ecpText) == 8u + CREATE_SID_BYTES, "protocol::CreateSupplement layout drift");

        // IRP_MJ_QUERY_INFORMATION

        // capture bit for QueryInfoSupplement::captured
        inline constexpr uint32_t QUERY_INFO_CAPTURED_PAYLOAD = 0x00000001u;

        inline constexpr uint32_t QUERY_INFO_PAYLOAD_BYTES = SUPPLEMENT_BYTES - 2u * sizeof(uint32_t);

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

        inline constexpr uint32_t SET_INFO_TARGET_NAME_WCHARS = 256u;

        inline constexpr uint32_t SET_INFO_PAYLOAD_BYTES = SUPPLEMENT_BYTES - 2u * sizeof(uint32_t) - sizeof(wchar_t) * SET_INFO_TARGET_NAME_WCHARS;

        struct SetInfoSupplement {
            uint32_t captured;
            uint32_t capturedBytes;
            uint8_t payload[SET_INFO_PAYLOAD_BYTES];
            wchar_t targetName[SET_INFO_TARGET_NAME_WCHARS];
        };

        static_assert(offsetof(SetInfoSupplement, payload) == 8u, "protocol::SetInfoSupplement layout drift");
        static_assert(offsetof(SetInfoSupplement, targetName) == 8u + SET_INFO_PAYLOAD_BYTES, "protocol::SetInfoSupplement layout drift");

        // IRP_MJ_QUERY_VOLUME_INFORMATION / IRP_MJ_SET_VOLUME_INFORMATION

        // capture bit for VolumeInfoSupplement::captured
        inline constexpr uint32_t VOLUME_INFO_CAPTURED_PAYLOAD = 0x00000001u;

        inline constexpr uint32_t VOLUME_INFO_PAYLOAD_BYTES = SUPPLEMENT_BYTES - 2u * sizeof(uint32_t);

        struct VolumeInfoSupplement {
            uint32_t captured;
            uint32_t capturedBytes;
            uint8_t payload[VOLUME_INFO_PAYLOAD_BYTES];
        };

        static_assert(offsetof(VolumeInfoSupplement, payload) == 8u, "protocol::VolumeInfoSupplement layout drift");

        // IRP_MJ_DIRECTORY_CONTROL

        // capture bits for QueryDirectorySupplement::captured
        inline constexpr uint32_t QUERY_DIRECTORY_CAPTURED_PAYLOAD    = 0x00000001u;
        inline constexpr uint32_t QUERY_DIRECTORY_CAPTURED_FILE_NAME  = 0x00000002u;
        inline constexpr uint32_t QUERY_DIRECTORY_TRUNCATED_FILE_NAME = 0x00000004u;

        inline constexpr uint32_t QUERY_DIRECTORY_FILE_NAME_WCHARS = 256u;

        inline constexpr uint32_t QUERY_DIRECTORY_PAYLOAD_BYTES = SUPPLEMENT_BYTES - 2u * sizeof(uint32_t) - sizeof(wchar_t) * QUERY_DIRECTORY_FILE_NAME_WCHARS;

        struct QueryDirectorySupplement {
            uint32_t captured;
            uint32_t capturedBytes;
            uint8_t payload[QUERY_DIRECTORY_PAYLOAD_BYTES];
            wchar_t fileName[QUERY_DIRECTORY_FILE_NAME_WCHARS];
        };

        static_assert(offsetof(QueryDirectorySupplement, payload) == 8u, "protocol::QueryDirectorySupplement layout drift");
        static_assert(offsetof(QueryDirectorySupplement, fileName) == 8u + QUERY_DIRECTORY_PAYLOAD_BYTES, "protocol::QueryDirectorySupplement layout drift");

        // IRP_MJ_FILE_SYSTEM_CONTROL

        // capture bits for FsControlSupplement::captured
        inline constexpr uint32_t FS_CONTROL_CAPTURED_INPUT  = 0x00000001u;
        inline constexpr uint32_t FS_CONTROL_CAPTURED_OUTPUT = 0x00000002u;

        inline constexpr uint32_t FS_CONTROL_INPUT_PAYLOAD_BYTES  = 546u;
        inline constexpr uint32_t FS_CONTROL_OUTPUT_PAYLOAD_BYTES = SUPPLEMENT_BYTES - 3u * sizeof(uint32_t) - FS_CONTROL_INPUT_PAYLOAD_BYTES;

        struct FsControlSupplement {
            uint32_t captured;
            uint32_t capturedInputBytes;
            uint32_t capturedOutputBytes;
            uint8_t inputPayload[FS_CONTROL_INPUT_PAYLOAD_BYTES];
            uint8_t outputPayload[FS_CONTROL_OUTPUT_PAYLOAD_BYTES];    // METHOD_IN_DIRECT: the second input buffer, captured pre-operation
        };

        static_assert(offsetof(FsControlSupplement, inputPayload) == 12u, "protocol::FsControlSupplement layout drift");
        static_assert(offsetof(FsControlSupplement, outputPayload) == 12u + FS_CONTROL_INPUT_PAYLOAD_BYTES, "protocol::FsControlSupplement layout drift");

        // IRP_MJ_DEVICE_CONTROL / IRP_MJ_INTERNAL_DEVICE_CONTROL

        // capture bits for DeviceIoControlSupplement::captured
        inline constexpr uint32_t DEVICE_IO_CONTROL_CAPTURED_INPUT  = 0x00000001u;
        inline constexpr uint32_t DEVICE_IO_CONTROL_CAPTURED_OUTPUT = 0x00000002u;

        inline constexpr uint32_t DEVICE_IO_CONTROL_INPUT_PAYLOAD_BYTES  = 546u;
        inline constexpr uint32_t DEVICE_IO_CONTROL_OUTPUT_PAYLOAD_BYTES = SUPPLEMENT_BYTES - 3u * sizeof(uint32_t) - DEVICE_IO_CONTROL_INPUT_PAYLOAD_BYTES;

        struct DeviceIoControlSupplement {
            uint32_t captured;
            uint32_t capturedInputBytes;
            uint32_t capturedOutputBytes;
            uint8_t inputPayload[DEVICE_IO_CONTROL_INPUT_PAYLOAD_BYTES];
            uint8_t outputPayload[DEVICE_IO_CONTROL_OUTPUT_PAYLOAD_BYTES];    // METHOD_IN_DIRECT: the second input buffer, captured pre-operation
        };

        static_assert(offsetof(DeviceIoControlSupplement, inputPayload) == 12u, "protocol::DeviceIoControlSupplement layout drift");
        static_assert(offsetof(DeviceIoControlSupplement, outputPayload) == 12u + DEVICE_IO_CONTROL_INPUT_PAYLOAD_BYTES, "protocol::DeviceIoControlSupplement layout drift");

        // IRP_MJ_LOCK_CONTROL

        // capture bit for LockControlSupplement::captured
        inline constexpr uint32_t LOCK_CONTROL_CAPTURED_LENGTH = 0x00000001u;

        struct LockControlSupplement {
            uint32_t captured;
            uint8_t reserved[4u];
            int64_t length;
        };

        static_assert(offsetof(LockControlSupplement, length) == 8u, "protocol::LockControlSupplement layout drift");
        static_assert(sizeof(LockControlSupplement) == 16u, "protocol::LockControlSupplement layout drift");
        static_assert(sizeof(LockControlSupplement) <= SUPPLEMENT_BYTES, "protocol::LockControlSupplement exceeds the supplement union");

        // IRP_MJ_QUERY_SECURITY / IRP_MJ_SET_SECURITY

        // capture bit for SecuritySupplement::captured
        inline constexpr uint32_t SECURITY_CAPTURED_PAYLOAD = 0x00000001u;

        inline constexpr uint32_t SECURITY_PAYLOAD_BYTES = SUPPLEMENT_BYTES - 2u * sizeof(uint32_t);

        struct SecuritySupplement {
            uint32_t captured;
            uint32_t capturedBytes;
            uint8_t payload[SECURITY_PAYLOAD_BYTES];    // self-relative SECURITY_DESCRIPTOR, validated by the driver
        };

        static_assert(offsetof(SecuritySupplement, payload) == 8u, "protocol::SecuritySupplement layout drift");

        union Supplement {
            CreateSupplement create;
            QueryInfoSupplement queryInfo;
            SetInfoSupplement setInfo;
            VolumeInfoSupplement volumeInfo;
            QueryDirectorySupplement queryDirectory;
            FsControlSupplement fsControl;
            DeviceIoControlSupplement deviceIoControl;
            LockControlSupplement lockControl;
            SecuritySupplement security;
        };

        static_assert(sizeof(CreateSupplement) == SUPPLEMENT_BYTES, "protocol::CreateSupplement does not exactly fill the union: re-balance a capacity or SUPPLEMENT_BYTES");
        static_assert(sizeof(QueryInfoSupplement) == SUPPLEMENT_BYTES, "protocol::QueryInfoSupplement does not exactly fill the union: re-balance a capacity or SUPPLEMENT_BYTES");
        static_assert(sizeof(SetInfoSupplement) == SUPPLEMENT_BYTES, "protocol::SetInfoSupplement does not exactly fill the union: re-balance a capacity or SUPPLEMENT_BYTES");
        static_assert(sizeof(VolumeInfoSupplement) == SUPPLEMENT_BYTES, "protocol::VolumeInfoSupplement does not exactly fill the union: re-balance a capacity or SUPPLEMENT_BYTES");
        static_assert(sizeof(QueryDirectorySupplement) == SUPPLEMENT_BYTES, "protocol::QueryDirectorySupplement does not exactly fill the union: re-balance a capacity or SUPPLEMENT_BYTES");
        static_assert(sizeof(FsControlSupplement) == SUPPLEMENT_BYTES, "protocol::FsControlSupplement does not exactly fill the union: re-balance a capacity or SUPPLEMENT_BYTES");
        static_assert(sizeof(DeviceIoControlSupplement) == SUPPLEMENT_BYTES, "protocol::DeviceIoControlSupplement does not exactly fill the union: re-balance a capacity or SUPPLEMENT_BYTES");
        static_assert(sizeof(SecuritySupplement) == SUPPLEMENT_BYTES, "protocol::SecuritySupplement does not exactly fill the union: re-balance a capacity or SUPPLEMENT_BYTES");
        static_assert(sizeof(Supplement) == SUPPLEMENT_BYTES, "protocol::Supplement layout drift");

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
