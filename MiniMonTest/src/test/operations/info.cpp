#include "info.h"

#include "..\report.h"
#include "..\target.h"

#include "..\..\error.h"

#include "..\..\..\..\inc\kernel.h"

#include <Windows.h>
#undef WIN32_NO_STATUS
#include <ntstatus.h>
#include <winternl.h>

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

extern "C" NTSYSAPI NTSTATUS NTAPI NtQueryFullAttributesFile(OBJECT_ATTRIBUTES* pObjectAttributes, void* pBuffer);

using namespace mimo;

namespace {

    struct QueryOperation {
        test::report::Description description;
        uint32_t infoClass;
        uint32_t size;
        bool asynchronous;
    };

    constexpr QueryOperation QUERY_OPERATIONS[]{
        { { L"query-basic",         STATUS_SUCCESS         }, kernel::FileBasicInformation,               1024u, false },
        { { L"query-standard",      STATUS_SUCCESS         }, kernel::FileStandardInformation,            1024u, false },
        { { L"query-internal",      STATUS_SUCCESS         }, kernel::FileInternalInformation,            1024u, false },
        { { L"query-ea",            STATUS_SUCCESS         }, kernel::FileEaInformation,                  1024u, false },
        { { L"query-name",          STATUS_SUCCESS         }, kernel::FileNameInformation,                1024u, false },
        { { L"query-name-overflow", STATUS_BUFFER_OVERFLOW }, kernel::FileNameInformation,                8u,    false },
        { { L"query-position",      STATUS_SUCCESS         }, kernel::FilePositionInformation,            1024u, true  },
        { { L"query-all",           STATUS_SUCCESS         }, kernel::FileAllInformation,                 1024u, false },
        { { L"query-stream",        STATUS_SUCCESS         }, kernel::FileStreamInformation,              1024u, false },
        { { L"query-compression",   STATUS_SUCCESS         }, kernel::FileCompressionInformation,         1024u, false },
        { { L"query-network-open",  STATUS_SUCCESS         }, kernel::FileNetworkOpenInformation,         1024u, false },
        { { L"query-attribute-tag", STATUS_SUCCESS         }, kernel::FileAttributeTagInformation,        1024u, false },
        { { L"query-hard-link",     STATUS_SUCCESS         }, kernel::FileHardLinkInformation,            1024u, false },
        { { L"query-process-ids",   STATUS_SUCCESS         }, kernel::FileProcessIdsUsingFileInformation, 1024u, false },
        { { L"query-standard-link", STATUS_SUCCESS         }, kernel::FileStandardLinkInformation,        1024u, false },
        { { L"query-volume-name",   STATUS_SUCCESS         }, kernel::FileVolumeNameInformation,          1024u, false },
        { { L"query-id",            STATUS_SUCCESS         }, kernel::FileIdInformation,                  1024u, false },
        { { L"query-stat",          STATUS_SUCCESS         }, kernel::FileStatInformation,                1024u, false },
        { { L"query-stat-lx",       STATUS_SUCCESS         }, kernel::FileStatLxInformation,              1024u, false },
    };

    NTSTATUS Query(
        const test::Target& target,
        uint32_t infoClass,
        std::span<std::byte> data
    ) {
        IO_STATUS_BLOCK ioStatus{};

        return NtQueryInformationFile(target.Get(), &ioStatus, data.data(), static_cast<ULONG>(data.size()), static_cast<ULONG>(infoClass));
    }


    constexpr std::string_view QUERY_MAJOR = "Query information";

    void RunQueryOperations(std::wstring_view ntDirectory, test::report::Counts& counts) {

        for (const QueryOperation& operation : QUERY_OPERATIONS) {
            test::Target target{ ntDirectory, operation.description.name };
            const NTSTATUS createStatus = target.Create(FILE_GENERIC_READ, operation.asynchronous);

            if (!NT_SUCCESS(createStatus)) {
                std::cerr << "Failed to create the target file\n";
                error::Display(RtlNtStatusToDosError(createStatus));
                test::report::Add(counts, QUERY_MAJOR, operation.description, std::nullopt);

                continue;
            }

            std::vector<std::byte> data(operation.size);
            const NTSTATUS queryStatus = Query(target, operation.infoClass, data);
            test::report::Add(counts, QUERY_MAJOR, operation.description, queryStatus);
        }

        return;
    }


    struct SetOperation {
        test::report::Description description;
        uint32_t infoClass;
        uint32_t value;
        bool asynchronous;
    };

    constexpr SetOperation SET_OPERATIONS[]{
        { { L"set-basic",                 STATUS_SUCCESS            }, kernel::FileBasicInformation,               FILE_ATTRIBUTE_HIDDEN,                               false },
        { { L"set-rename",                STATUS_SUCCESS            }, kernel::FileRenameInformation,              1u,                                                  false },
        { { L"set-link",                  STATUS_SUCCESS            }, kernel::FileLinkInformation,                1u,                                                  false },
        { { L"set-disposition",           STATUS_SUCCESS            }, kernel::FileDispositionInformation,         1u,                                                  false },
        { { L"set-position",              STATUS_SUCCESS            }, kernel::FilePositionInformation,            8u,                                                  true  },
        { { L"set-allocation",            STATUS_SUCCESS            }, kernel::FileAllocationInformation,          4096u,                                               false },
        { { L"set-end-of-file",           STATUS_SUCCESS            }, kernel::FileEndOfFileInformation,           16u,                                                 false },
        { { L"set-valid-data-length",     STATUS_PRIVILEGE_NOT_HELD }, kernel::FileValidDataLengthInformation,     16u,                                                 false },
        { { L"set-short-name",            STATUS_SUCCESS            }, kernel::FileShortNameInformation,           0u,                                                  false },
        { { L"set-disposition-ex",        STATUS_SUCCESS            }, kernel::FileDispositionInformationEx,       kernel::FILE_DISPOSITION_DELETE,                     false },
        { { L"set-rename-ex",             STATUS_SUCCESS            }, kernel::FileRenameInformationEx,            kernel::FILE_RENAME_POSIX_SEMANTICS,                 false },
        { { L"set-desired-storage-class", STATUS_SUCCESS            }, kernel::FileDesiredStorageClassInformation, static_cast<uint32_t>(FileStorageTierClassCapacity), false },
        { { L"set-case-sensitive",        STATUS_INVALID_PARAMETER  }, kernel::FileCaseSensitiveInformation,       FILE_CS_FLAG_CASE_SENSITIVE_DIR,                     false },
        { { L"set-link-ex",               STATUS_SUCCESS            }, kernel::FileLinkInformationEx,              kernel::FILE_LINK_POSIX_SEMANTICS,                   false },
        { { L"set-storage-reserve-id",    STATUS_SUCCESS            }, kernel::FileStorageReserveIdInformation,    static_cast<uint32_t>(StorageReserveIdSoft),         false },
        { { L"set-known-folder",          STATUS_INVALID_PARAMETER  }, kernel::FileKnownFolderInformation,         kernel::KnownFolderDocuments,                        false },
    };

    template <typename Info>
    std::vector<std::byte> MakeBuffer(const Info& payload) {
        std::vector<std::byte> buffer(sizeof(payload));
        std::memcpy(buffer.data(), &payload, sizeof(payload));

        return buffer;
    }


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

                return MakeBuffer(basic);
            }

            case kernel::FileRenameInformation:
            case kernel::FileLinkInformation:
            case kernel::FileRenameInformationEx:
            case kernel::FileLinkInformationEx:

                return MakeNameBuffer(value, targetPath);

            case kernel::FileDispositionInformation: {
                FILE_DISPOSITION_INFO disposition{};
                disposition.DeleteFile = static_cast<BOOLEAN>(value);

                return MakeBuffer(disposition);
            }

            case kernel::FilePositionInformation: {
                FILE_POSITION_INFORMATION position{};
                position.CurrentByteOffset.QuadPart = static_cast<LONGLONG>(value);

                return MakeBuffer(position);
            }

            case kernel::FileAllocationInformation: {
                FILE_ALLOCATION_INFO allocation{};
                allocation.AllocationSize.QuadPart = static_cast<LONGLONG>(value);

                return MakeBuffer(allocation);
            }

            case kernel::FileEndOfFileInformation: {
                FILE_END_OF_FILE_INFO endOfFile{};
                endOfFile.EndOfFile.QuadPart = static_cast<LONGLONG>(value);

                return MakeBuffer(endOfFile);
            }

            case kernel::FileValidDataLengthInformation: {
                FILE_VALID_DATA_LENGTH_INFORMATION validDataLength{};
                validDataLength.ValidDataLength.QuadPart = static_cast<LONGLONG>(value);

                return MakeBuffer(validDataLength);
            }

            case kernel::FileShortNameInformation:

                return MakeShortNameBuffer();

            case kernel::FileDispositionInformationEx: {
                FILE_DISPOSITION_INFO_EX dispositionEx{};
                dispositionEx.Flags = static_cast<DWORD>(value);

                return MakeBuffer(dispositionEx);
            }

            case kernel::FileDesiredStorageClassInformation: {
                FILE_DESIRED_STORAGE_CLASS_INFORMATION desiredStorageClass{};
                desiredStorageClass.Class = static_cast<FILE_STORAGE_TIER_CLASS>(value);

                return MakeBuffer(desiredStorageClass);
            }

            case kernel::FileCaseSensitiveInformation: {
                FILE_CASE_SENSITIVE_INFO caseSensitive{};
                caseSensitive.Flags = static_cast<ULONG>(value);

                return MakeBuffer(caseSensitive);
            }

            case kernel::FileStorageReserveIdInformation: {
                FILE_STORAGE_RESERVE_ID_INFORMATION storageReserveId{};
                storageReserveId.StorageReserveId = static_cast<STORAGE_RESERVE_ID>(value);

                return MakeBuffer(storageReserveId);
            }

            case kernel::FileKnownFolderInformation: {
                FILE_KNOWN_FOLDER_INFORMATION knownFolder{};
                knownFolder.Type = static_cast<ULONG>(value);

                return MakeBuffer(knownFolder);
            }

        }

        return std::nullopt;
    }


    NTSTATUS Set(
        const test::Target& target,
        uint32_t infoClass,
        std::span<const std::byte> data
    ) {
        std::vector<std::byte> buffer(data.begin(), data.end());
        IO_STATUS_BLOCK ioStatus{};

        return NtSetInformationFile(target.Get(), &ioStatus, buffer.data(), static_cast<ULONG>(buffer.size()), static_cast<ULONG>(infoClass));
    }


    constexpr std::string_view SET_MAJOR = "Set information";

    void RunSetOperations(std::wstring_view ntDirectory, test::report::Counts& counts) {

        for (const SetOperation& operation : SET_OPERATIONS) {
            test::Target target{ ntDirectory, operation.description.name };
            const NTSTATUS createStatus = target.Create(FILE_WRITE_ATTRIBUTES | FILE_WRITE_DATA | DELETE, operation.asynchronous);

            if (!NT_SUCCESS(createStatus)) {
                std::cerr << "Failed to create the target file\n";
                error::Display(RtlNtStatusToDosError(createStatus));
                test::report::Add(counts, SET_MAJOR, operation.description, std::nullopt);

                continue;
            }

            const std::wstring targetPath = std::format(L"{}\\{}-target", ntDirectory, operation.description.name);
            const std::optional<std::vector<std::byte>> data = MakeSetBuffer(operation.infoClass, operation.value, targetPath);

            if (!data.has_value()) {
                std::cerr << "No payload for the information class\n";
                test::report::Add(counts, SET_MAJOR, operation.description, std::nullopt);

                continue;
            }

            const NTSTATUS setStatus = Set(target, operation.infoClass, *data);
            test::report::Add(counts, SET_MAJOR, operation.description, setStatus);
        }

        return;
    }


    constexpr test::report::Description NETWORK_QUERY_OPEN_DESCRIPTIONS[]{
        { L"network-query-open", STATUS_SUCCESS },
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


    constexpr std::string_view NETWORK_QUERY_OPEN_MAJOR = "Network query open";

    void RunNetworkQueryOpenOperations(std::wstring_view ntDirectory, test::report::Counts& counts) {

        for (const test::report::Description& description : NETWORK_QUERY_OPEN_DESCRIPTIONS) {
            test::Target target{ ntDirectory, description.name };
            const NTSTATUS createStatus = target.Create(FILE_READ_ATTRIBUTES);

            if (!NT_SUCCESS(createStatus)) {
                std::cerr << "Failed to create the target file\n";
                error::Display(RtlNtStatusToDosError(createStatus));
                test::report::Add(counts, NETWORK_QUERY_OPEN_MAJOR, description, std::nullopt);

                continue;
            }

            const NTSTATUS queryStatus = QueryFullAttributes(target);
            test::report::Add(counts, NETWORK_QUERY_OPEN_MAJOR, description, queryStatus);
        }

        return;
    }

}

namespace mimo {

    namespace test {

        namespace operations {

            namespace info {

                bool Run(std::wstring_view ntDirectory, report::Counts& counts) {
                    RunQueryOperations(ntDirectory, counts);
                    RunSetOperations(ntDirectory, counts);
                    RunNetworkQueryOpenOperations(ntDirectory, counts);

                    return true;
                }

            }

        }

    }

}
