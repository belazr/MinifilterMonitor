#include "info.h"

#include "..\buffer.h"
#include "..\report.h"
#include "..\target.h"

#include "..\..\error.h"
#include "..\..\test.h"

#include "..\..\..\..\inc\kernel.h"

#include <Windows.h>
#undef WIN32_NO_STATUS
#include <ntstatus.h>
#include <winternl.h>

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <format>
#include <iostream>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>

extern "C" NTSYSAPI NTSTATUS NTAPI NtQueryInformationFile(
    HANDLE hFile,
    IO_STATUS_BLOCK* pIoStatus,
    void* pBuffer,
    ULONG bufferSize,
    ULONG infoClass
);

extern "C" NTSYSAPI NTSTATUS NTAPI NtSetInformationFile(
    HANDLE hFile,
    IO_STATUS_BLOCK* pIoStatus,
    void* pBuffer,
    ULONG bufferSize,
    ULONG infoClass
);

extern "C" NTSYSAPI NTSTATUS NTAPI NtQueryInformationByName(
    OBJECT_ATTRIBUTES* pObjectAttributes,
    IO_STATUS_BLOCK* pIoStatus,
    void* pBuffer,
    ULONG bufferSize,
    ULONG infoClass
);

extern "C" NTSYSAPI NTSTATUS NTAPI NtQueryFullAttributesFile(OBJECT_ATTRIBUTES* pObjectAttributes, void* pBuffer);

using namespace mimo;

namespace {

    struct QueryOperation {
        test::Description description;
        uint32_t infoClass;
        uint32_t size;
        bool asynchronous;
    };

    constexpr std::string_view QUERY_MAJOR = "Query information";

    constexpr QueryOperation QUERY_OPERATIONS[]{
        { { QUERY_MAJOR, L"query-info-basic",         STATUS_SUCCESS         }, kernel::FileBasicInformation,        1024u, false },
        { { QUERY_MAJOR, L"query-info-standard",      STATUS_SUCCESS         }, kernel::FileStandardInformation,     1024u, false },
        { { QUERY_MAJOR, L"query-info-internal",      STATUS_SUCCESS         }, kernel::FileInternalInformation,     1024u, false },
        { { QUERY_MAJOR, L"query-info-ea",            STATUS_SUCCESS         }, kernel::FileEaInformation,           1024u, false },
        { { QUERY_MAJOR, L"query-info-name",          STATUS_SUCCESS         }, kernel::FileNameInformation,         1024u, false },
        { { QUERY_MAJOR, L"query-info-name-overflow", STATUS_BUFFER_OVERFLOW }, kernel::FileNameInformation,         8u,    false },
        // the handle is asynchronous because NtQueryInformationFile answers FilePositionInformation on a synchronous handle without a request to the file system
        { { QUERY_MAJOR, L"query-info-position",      STATUS_SUCCESS         }, kernel::FilePositionInformation,     1024u, true  },
        { { QUERY_MAJOR, L"query-info-all",           STATUS_SUCCESS         }, kernel::FileAllInformation,          1024u, false },
        { { QUERY_MAJOR, L"query-info-stream",        STATUS_SUCCESS         }, kernel::FileStreamInformation,       1024u, false },
        { { QUERY_MAJOR, L"query-info-compression",   STATUS_SUCCESS         }, kernel::FileCompressionInformation,  1024u, false },
        { { QUERY_MAJOR, L"query-info-network-open",  STATUS_SUCCESS         }, kernel::FileNetworkOpenInformation,  1024u, false },
        { { QUERY_MAJOR, L"query-info-attribute-tag", STATUS_SUCCESS         }, kernel::FileAttributeTagInformation, 1024u, false },
        { { QUERY_MAJOR, L"query-info-hard-link",     STATUS_SUCCESS         }, kernel::FileHardLinkInformation,     1024u, false },
        { { QUERY_MAJOR, L"query-info-standard-link", STATUS_SUCCESS         }, kernel::FileStandardLinkInformation, 1024u, false },
        { { QUERY_MAJOR, L"query-info-id",            STATUS_SUCCESS         }, kernel::FileIdInformation,           1024u, false },
        { { QUERY_MAJOR, L"query-info-stat",          STATUS_SUCCESS         }, kernel::FileStatInformation,         1024u, false },
        { { QUERY_MAJOR, L"query-info-stat-lx",       STATUS_SUCCESS         }, kernel::FileStatLxInformation,       1024u, false },
    };

    NTSTATUS Query(
        const test::Target& target,
        uint32_t infoClass,
        uint32_t size
    ) {
        std::vector<std::byte> buffer(size);
        IO_STATUS_BLOCK ioStatus{};

        return NtQueryInformationFile(target.Get(), &ioStatus, buffer.data(), static_cast<ULONG>(buffer.size()), static_cast<ULONG>(infoClass));
    }


    void RunQueryOperations(std::wstring_view ntDirectory, test::Report& report) {

        for (const QueryOperation& operation : QUERY_OPERATIONS) {
            test::Target target{ ntDirectory, operation.description.name };
            const NTSTATUS createStatus = target.Create(FILE_GENERIC_READ, operation.asynchronous);

            if (!NT_SUCCESS(createStatus)) {
                std::cerr << "Failed to create the target file\n";
                error::Display(RtlNtStatusToDosError(createStatus));
                report.Print(operation.description, std::nullopt);

                continue;
            }

            const NTSTATUS queryStatus = Query(target, operation.infoClass, operation.size);
            report.Print(operation.description, queryStatus);
        }

        return;
    }


    struct SetOperation {
        test::Description description;
        uint32_t infoClass;
        uint32_t value;
        bool asynchronous;
    };

    constexpr std::string_view SET_MAJOR = "Set information";

    constexpr SetOperation SET_OPERATIONS[]{
        { { SET_MAJOR, L"set-info-basic",                 STATUS_SUCCESS                        }, kernel::FileBasicInformation,               FILE_ATTRIBUTE_HIDDEN,                               false },
        { { SET_MAJOR, L"set-info-rename",                STATUS_SUCCESS                        }, kernel::FileRenameInformation,              1u,                                                  false },
        { { SET_MAJOR, L"set-info-link",                  STATUS_SUCCESS                        }, kernel::FileLinkInformation,                1u,                                                  false },
        { { SET_MAJOR, L"set-info-disposition",           STATUS_SUCCESS                        }, kernel::FileDispositionInformation,         1u,                                                  false },
        // the handle is asynchronous because NtSetInformationFile answers FilePositionInformation on a synchronous handle without a request to the file system
        { { SET_MAJOR, L"set-info-position",              STATUS_SUCCESS                        }, kernel::FilePositionInformation,            8u,                                                  true  },
        { { SET_MAJOR, L"set-info-allocation",            STATUS_SUCCESS                        }, kernel::FileAllocationInformation,          4096u,                                               false },
        { { SET_MAJOR, L"set-info-end-of-file",           STATUS_SUCCESS                        }, kernel::FileEndOfFileInformation,           16u,                                                 false },
        // STATUS_PRIVILEGE_NOT_HELD is expected because the process runs without SeManageVolumePrivilege
        { { SET_MAJOR, L"set-info-valid-data-length",     STATUS_PRIVILEGE_NOT_HELD             }, kernel::FileValidDataLengthInformation,     16u,                                                 false },
        { { SET_MAJOR, L"set-info-short-name",            STATUS_SUCCESS                        }, kernel::FileShortNameInformation,           0u,                                                  false },
        { { SET_MAJOR, L"set-info-disposition-ex",        STATUS_SUCCESS                        }, kernel::FileDispositionInformationEx,       kernel::FILE_DISPOSITION_DELETE,                     false },
        { { SET_MAJOR, L"set-info-rename-ex",             STATUS_SUCCESS                        }, kernel::FileRenameInformationEx,            kernel::FILE_RENAME_POSIX_SEMANTICS,                 false },
        { { SET_MAJOR, L"set-info-desired-storage-class", STATUS_SUCCESS                        }, kernel::FileDesiredStorageClassInformation, static_cast<uint32_t>(FileStorageTierClassCapacity), false },
        { { SET_MAJOR, L"set-info-case-sensitive",        STATUS_INVALID_PARAMETER              }, kernel::FileCaseSensitiveInformation,       FILE_CS_FLAG_CASE_SENSITIVE_DIR,                     false },
        { { SET_MAJOR, L"set-info-link-ex",               STATUS_SUCCESS                        }, kernel::FileLinkInformationEx,              kernel::FILE_LINK_POSIX_SEMANTICS,                   false },
        { { SET_MAJOR, L"set-info-storage-reserve-id",    STATUS_STORAGE_RESERVE_DOES_NOT_EXIST }, kernel::FileStorageReserveIdInformation,    static_cast<uint32_t>(StorageReserveIdSoft),         false },
        { { SET_MAJOR, L"set-info-known-folder",          STATUS_INVALID_PARAMETER              }, kernel::FileKnownFolderInformation,         kernel::KnownFolderDocuments,                        false },
    };

    std::vector<std::byte> MakeNameBuffer(uint32_t flags, std::wstring_view targetPath) {
        constexpr size_t NAME_OFFSET = offsetof(FILE_RENAME_INFO, FileName);
        const size_t nameSize = targetPath.size() * sizeof(wchar_t);
        FILE_RENAME_INFO header{};
        header.Flags = static_cast<DWORD>(flags);
        header.FileNameLength = static_cast<DWORD>(nameSize);

        std::vector<std::byte> buffer(NAME_OFFSET + nameSize);
        std::memcpy(buffer.data(), &header, NAME_OFFSET);
        std::memcpy(buffer.data() + NAME_OFFSET, targetPath.data(), nameSize);

        return buffer;
    }


    std::vector<std::byte> MakeShortNameBuffer() {
        constexpr std::wstring_view NAME = L"MIMO~1";
        constexpr size_t NAME_OFFSET = offsetof(FILE_NAME_INFO, FileName);
        constexpr size_t NAME_SIZE = NAME.size() * sizeof(wchar_t);
        FILE_NAME_INFO header{};
        header.FileNameLength = static_cast<DWORD>(NAME_SIZE);

        std::vector<std::byte> buffer(NAME_OFFSET + NAME_SIZE);
        std::memcpy(buffer.data(), &header, NAME_OFFSET);
        std::memcpy(buffer.data() + NAME_OFFSET, NAME.data(), NAME_SIZE);

        return buffer;
    }


    struct FILE_POSITION_INFORMATION {
        LARGE_INTEGER CurrentByteOffset;
    };

    struct FILE_VALID_DATA_LENGTH_INFORMATION {
        LARGE_INTEGER ValidDataLength;
    };

    struct FILE_STORAGE_RESERVE_ID_INFORMATION {
        STORAGE_RESERVE_ID StorageReserveId;
    };

    struct FILE_KNOWN_FOLDER_INFORMATION {
        ULONG Type;
    };

    std::optional<std::vector<std::byte>> MakeSetBuffer(
        uint32_t infoClass,
        uint32_t value,
        std::wstring_view targetPath
    ) {

        switch (infoClass) {

            case kernel::FileBasicInformation: {
                FILE_BASIC_INFO basic{};
                basic.FileAttributes = static_cast<DWORD>(value);

                return test::MakeBuffer(basic);
            }

            case kernel::FileRenameInformation:
            case kernel::FileLinkInformation:
            case kernel::FileRenameInformationEx:
            case kernel::FileLinkInformationEx:

                return MakeNameBuffer(value, targetPath);

            case kernel::FileDispositionInformation: {
                FILE_DISPOSITION_INFO disposition{};
                disposition.DeleteFile = static_cast<BOOLEAN>(value);

                return test::MakeBuffer(disposition);
            }

            case kernel::FilePositionInformation: {
                FILE_POSITION_INFORMATION position{};
                position.CurrentByteOffset.QuadPart = static_cast<LONGLONG>(value);

                return test::MakeBuffer(position);
            }

            case kernel::FileAllocationInformation: {
                FILE_ALLOCATION_INFO allocation{};
                allocation.AllocationSize.QuadPart = static_cast<LONGLONG>(value);

                return test::MakeBuffer(allocation);
            }

            case kernel::FileEndOfFileInformation: {
                FILE_END_OF_FILE_INFO endOfFile{};
                endOfFile.EndOfFile.QuadPart = static_cast<LONGLONG>(value);

                return test::MakeBuffer(endOfFile);
            }

            case kernel::FileValidDataLengthInformation: {
                FILE_VALID_DATA_LENGTH_INFORMATION validDataLength{};
                validDataLength.ValidDataLength.QuadPart = static_cast<LONGLONG>(value);

                return test::MakeBuffer(validDataLength);
            }

            case kernel::FileShortNameInformation:

                return MakeShortNameBuffer();

            case kernel::FileDispositionInformationEx: {
                FILE_DISPOSITION_INFO_EX dispositionEx{};
                dispositionEx.Flags = static_cast<DWORD>(value);

                return test::MakeBuffer(dispositionEx);
            }

            case kernel::FileDesiredStorageClassInformation: {
                FILE_DESIRED_STORAGE_CLASS_INFORMATION desiredStorageClass{};
                desiredStorageClass.Class = static_cast<FILE_STORAGE_TIER_CLASS>(value);

                return test::MakeBuffer(desiredStorageClass);
            }

            case kernel::FileCaseSensitiveInformation: {
                FILE_CASE_SENSITIVE_INFO caseSensitive{};
                caseSensitive.Flags = static_cast<ULONG>(value);

                return test::MakeBuffer(caseSensitive);
            }

            case kernel::FileStorageReserveIdInformation: {
                FILE_STORAGE_RESERVE_ID_INFORMATION storageReserveId{};
                storageReserveId.StorageReserveId = static_cast<STORAGE_RESERVE_ID>(value);

                return test::MakeBuffer(storageReserveId);
            }

            case kernel::FileKnownFolderInformation: {
                FILE_KNOWN_FOLDER_INFORMATION knownFolder{};
                knownFolder.Type = static_cast<ULONG>(value);

                return test::MakeBuffer(knownFolder);
            }

        }

        return std::nullopt;
    }


    NTSTATUS Set(
        const test::Target& target,
        uint32_t infoClass,
        std::span<const std::byte> infoData
    ) {
        std::vector<std::byte> buffer(infoData.begin(), infoData.end());
        IO_STATUS_BLOCK ioStatus{};

        return NtSetInformationFile(target.Get(), &ioStatus, buffer.data(), static_cast<ULONG>(buffer.size()), static_cast<ULONG>(infoClass));
    }


    void RunSetOperations(std::wstring_view ntDirectory, test::Report& report) {

        for (const SetOperation& operation : SET_OPERATIONS) {
            test::Target target{ ntDirectory, operation.description.name };
            const NTSTATUS createStatus = target.Create(FILE_WRITE_ATTRIBUTES | FILE_WRITE_DATA | DELETE, operation.asynchronous);

            if (!NT_SUCCESS(createStatus)) {
                std::cerr << "Failed to create the target file\n";
                error::Display(RtlNtStatusToDosError(createStatus));
                report.Print(operation.description, std::nullopt);

                continue;
            }

            const std::wstring targetPath = std::format(L"{}\\{}-target", ntDirectory, operation.description.name);
            const std::optional<std::vector<std::byte>> infoData = MakeSetBuffer(operation.infoClass, operation.value, targetPath);

            if (!infoData.has_value()) {
                std::cerr << "No payload for the information class\n";
                report.Print(operation.description, std::nullopt);

                continue;
            }

            const NTSTATUS setStatus = Set(target, operation.infoClass, *infoData);
            report.Print(operation.description, setStatus);
        }

        return;
    }


    constexpr std::string_view QUERY_OPEN_MAJOR = "Query open";

    constexpr test::Description QUERY_OPEN_DESCRIPTIONS[]{
        { QUERY_OPEN_MAJOR, L"query-open-info", STATUS_SUCCESS },
    };

    NTSTATUS QueryByName(const test::Target& target) {
        constexpr uint32_t BUFFER_SIZE = 1024u;
        std::wstring path{ target.GetPath() };
        UNICODE_STRING objectName{};
        objectName.Length = static_cast<USHORT>(path.size() * sizeof(wchar_t));
        objectName.MaximumLength = objectName.Length;
        objectName.Buffer = path.data();

        OBJECT_ATTRIBUTES objectAttributes{};
        InitializeObjectAttributes(&objectAttributes, &objectName, OBJ_CASE_INSENSITIVE, nullptr, nullptr);

        std::array<std::byte, BUFFER_SIZE> buffer{};
        IO_STATUS_BLOCK ioStatus{};

        return NtQueryInformationByName(&objectAttributes, &ioStatus, buffer.data(), static_cast<ULONG>(buffer.size()), static_cast<ULONG>(kernel::FileStatInformation));
    }


    void RunQueryOpenOperations(std::wstring_view ntDirectory, test::Report& report) {

        for (const test::Description& description : QUERY_OPEN_DESCRIPTIONS) {
            test::Target target{ ntDirectory, description.name };
            const NTSTATUS createStatus = target.Create(FILE_READ_ATTRIBUTES);

            if (!NT_SUCCESS(createStatus)) {
                std::cerr << "Failed to create the target file\n";
                error::Display(RtlNtStatusToDosError(createStatus));
                report.Print(description, std::nullopt);

                continue;
            }

            const NTSTATUS queryStatus = QueryByName(target);
            report.Print(description, queryStatus);
        }

        return;
    }


    constexpr std::string_view NETWORK_QUERY_OPEN_MAJOR = "Network query open";

    constexpr test::Description NETWORK_QUERY_OPEN_DESCRIPTIONS[]{
        { NETWORK_QUERY_OPEN_MAJOR, L"network-query-open-info", STATUS_SUCCESS },
    };

    struct FILE_NETWORK_OPEN_INFORMATION {
        LARGE_INTEGER CreationTime;
        LARGE_INTEGER LastAccessTime;
        LARGE_INTEGER LastWriteTime;
        LARGE_INTEGER ChangeTime;
        LARGE_INTEGER AllocationSize;
        LARGE_INTEGER EndOfFile;
        ULONG FileAttributes;
    };

    NTSTATUS QueryFullAttributes(const test::Target& target) {
        std::wstring path{ target.GetPath() };
        UNICODE_STRING objectName{};
        objectName.Length = static_cast<USHORT>(path.size() * sizeof(wchar_t));
        objectName.MaximumLength = objectName.Length;
        objectName.Buffer = path.data();

        OBJECT_ATTRIBUTES objectAttributes{};
        InitializeObjectAttributes(&objectAttributes, &objectName, OBJ_CASE_INSENSITIVE, nullptr, nullptr);

        FILE_NETWORK_OPEN_INFORMATION networkOpen{};

        return NtQueryFullAttributesFile(&objectAttributes, &networkOpen);
    }


    void RunNetworkQueryOpenOperations(std::wstring_view ntDirectory, test::Report& report) {

        for (const test::Description& description : NETWORK_QUERY_OPEN_DESCRIPTIONS) {
            test::Target target{ ntDirectory, description.name };
            const NTSTATUS createStatus = target.Create(FILE_READ_ATTRIBUTES);

            if (!NT_SUCCESS(createStatus)) {
                std::cerr << "Failed to create the target file\n";
                error::Display(RtlNtStatusToDosError(createStatus));
                report.Print(description, std::nullopt);

                continue;
            }

            const NTSTATUS queryStatus = QueryFullAttributes(target);
            report.Print(description, queryStatus);
        }

        return;
    }

}

namespace mimo {

    namespace test {

        namespace operations {

            namespace info {

                void Run(std::wstring_view ntDirectory, Report& report) {
                    RunQueryOperations(ntDirectory, report);
                    RunSetOperations(ntDirectory, report);
                    RunQueryOpenOperations(ntDirectory, report);
                    RunNetworkQueryOpenOperations(ntDirectory, report);

                    return;
                }

            }

        }

    }

}
