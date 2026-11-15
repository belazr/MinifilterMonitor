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

using namespace mimo;

namespace {

    struct QueryOperation {
        test::report::Description description;
        uint32_t infoClass;
        uint32_t size;
    };

    constexpr QueryOperation QUERY_OPERATIONS[]{
        { { L"query-basic",         STATUS_SUCCESS         }, kernel::FileBasicInformation,               1024u },
        { { L"query-standard",      STATUS_SUCCESS         }, kernel::FileStandardInformation,            1024u },
        { { L"query-internal",      STATUS_SUCCESS         }, kernel::FileInternalInformation,            1024u },
        { { L"query-ea",            STATUS_SUCCESS         }, kernel::FileEaInformation,                  1024u },
        { { L"query-name",          STATUS_SUCCESS         }, kernel::FileNameInformation,                1024u },
        { { L"query-name-overflow", STATUS_BUFFER_OVERFLOW }, kernel::FileNameInformation,                8u    },
        { { L"query-position",      STATUS_SUCCESS         }, kernel::FilePositionInformation,            1024u },
        { { L"query-all",           STATUS_SUCCESS         }, kernel::FileAllInformation,                 1024u },
        { { L"query-stream",        STATUS_SUCCESS         }, kernel::FileStreamInformation,              1024u },
        { { L"query-compression",   STATUS_SUCCESS         }, kernel::FileCompressionInformation,         1024u },
        { { L"query-network-open",  STATUS_SUCCESS         }, kernel::FileNetworkOpenInformation,         1024u },
        { { L"query-attribute-tag", STATUS_SUCCESS         }, kernel::FileAttributeTagInformation,        1024u },
        { { L"query-hard-link",     STATUS_SUCCESS         }, kernel::FileHardLinkInformation,            1024u },
        { { L"query-process-ids",   STATUS_SUCCESS         }, kernel::FileProcessIdsUsingFileInformation, 1024u },
        { { L"query-standard-link", STATUS_SUCCESS         }, kernel::FileStandardLinkInformation,        1024u },
        { { L"query-volume-name",   STATUS_SUCCESS         }, kernel::FileVolumeNameInformation,          1024u },
        { { L"query-id",            STATUS_SUCCESS         }, kernel::FileIdInformation,                  1024u },
        { { L"query-stat",          STATUS_SUCCESS         }, kernel::FileStatInformation,                1024u },
        { { L"query-stat-lx",       STATUS_SUCCESS         }, kernel::FileStatLxInformation,              1024u },
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
            const NTSTATUS createStatus = target.Create(FILE_GENERIC_READ);

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
    };

    constexpr SetOperation SET_OPERATIONS[]{
        { { L"set-basic",                 STATUS_SUCCESS            }, kernel::FileBasicInformation,               FILE_ATTRIBUTE_HIDDEN                               },
        { { L"set-rename",                STATUS_SUCCESS            }, kernel::FileRenameInformation,              1u                                                  },
        { { L"set-link",                  STATUS_SUCCESS            }, kernel::FileLinkInformation,                1u                                                  },
        { { L"set-disposition",           STATUS_SUCCESS            }, kernel::FileDispositionInformation,         1u                                                  },
        { { L"set-allocation",            STATUS_SUCCESS            }, kernel::FileAllocationInformation,          4096u                                               },
        { { L"set-end-of-file",           STATUS_SUCCESS            }, kernel::FileEndOfFileInformation,           16u                                                 },
        { { L"set-valid-data-length",     STATUS_PRIVILEGE_NOT_HELD }, kernel::FileValidDataLengthInformation,     16u                                                 },
        { { L"set-short-name",            STATUS_SUCCESS            }, kernel::FileShortNameInformation,           0u                                                  },
        { { L"set-disposition-ex",        STATUS_SUCCESS            }, kernel::FileDispositionInformationEx,       kernel::FILE_DISPOSITION_DELETE                     },
        { { L"set-rename-ex",             STATUS_SUCCESS            }, kernel::FileRenameInformationEx,            kernel::FILE_RENAME_POSIX_SEMANTICS                 },
        { { L"set-desired-storage-class", STATUS_SUCCESS            }, kernel::FileDesiredStorageClassInformation, static_cast<uint32_t>(FileStorageTierClassCapacity) },
        { { L"set-case-sensitive",        STATUS_INVALID_PARAMETER  }, kernel::FileCaseSensitiveInformation,       FILE_CS_FLAG_CASE_SENSITIVE_DIR                     },
        { { L"set-link-ex",               STATUS_SUCCESS            }, kernel::FileLinkInformationEx,              kernel::FILE_LINK_POSIX_SEMANTICS                   },
        { { L"set-storage-reserve-id",    STATUS_SUCCESS            }, kernel::FileStorageReserveIdInformation,    static_cast<uint32_t>(StorageReserveIdSoft)         },
        { { L"set-known-folder",          STATUS_INVALID_PARAMETER  }, kernel::FileKnownFolderInformation,         kernel::KnownFolderDocuments                        },
    };

    template <typename Info>
    std::vector<std::byte> MakeBuffer(const Info& info) {
        std::vector<std::byte> buffer(sizeof(info));
        std::memcpy(buffer.data(), &info, sizeof(info));

        return buffer;
    }


    std::vector<std::byte> MakeNameBuffer(uint32_t flags, std::wstring_view targetPath) {
        constexpr size_t NAME_OFFSET = offsetof(FILE_RENAME_INFO, FileName);
        const size_t nameSize = targetPath.size() * sizeof(wchar_t);
        FILE_RENAME_INFO info{};
        info.Flags = static_cast<DWORD>(flags);
        info.FileNameLength = static_cast<DWORD>(nameSize);

        std::vector<std::byte> buffer(NAME_OFFSET + nameSize);
        std::memcpy(buffer.data(), &info, NAME_OFFSET);
        std::memcpy(buffer.data() + NAME_OFFSET, targetPath.data(), nameSize);

        return buffer;
    }


    std::vector<std::byte> MakeShortNameBuffer() {
        constexpr std::wstring_view NAME = L"MIMO~1";
        constexpr size_t NAME_OFFSET = offsetof(FILE_NAME_INFO, FileName);
        constexpr size_t NAME_SIZE = NAME.size() * sizeof(wchar_t);
        FILE_NAME_INFO info{};
        info.FileNameLength = static_cast<DWORD>(NAME_SIZE);

        std::vector<std::byte> buffer(NAME_OFFSET + NAME_SIZE);
        std::memcpy(buffer.data(), &info, NAME_OFFSET);
        std::memcpy(buffer.data() + NAME_OFFSET, NAME.data(), NAME_SIZE);

        return buffer;
    }


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
        IO_STATUS_BLOCK ioStatus{};

        return NtSetInformationFile(target.Get(), &ioStatus, const_cast<std::byte*>(data.data()), static_cast<ULONG>(data.size()), static_cast<ULONG>(infoClass));
    }


    constexpr std::string_view SET_MAJOR = "Set information";

    void RunSetOperations(std::wstring_view ntDirectory, test::report::Counts& counts) {

        for (const SetOperation& operation : SET_OPERATIONS) {
            test::Target target{ ntDirectory, operation.description.name };
            const NTSTATUS createStatus = target.Create(FILE_WRITE_ATTRIBUTES | FILE_WRITE_DATA | DELETE);

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

}

namespace mimo {

    namespace test {

        namespace operations {

            namespace info {

                bool Run(std::wstring_view ntDirectory, report::Counts& counts) {
                    RunQueryOperations(ntDirectory, counts);
                    RunSetOperations(ntDirectory, counts);

                    return true;
                }

            }

        }

    }

}
