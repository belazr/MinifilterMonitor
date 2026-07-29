#include "details.h"

#include "..\kernel.h"
#include "..\text.h"

#include "..\..\..\inc\protocol.h"

#include <Windows.h>
#include <sddl.h>

#include <cstddef>
#include <cstring>
#include <format>
#include <map>
#include <span>
#include <string>
#include <string_view>
#include <utility>

using namespace mimo;

namespace {

    constexpr DWORD ACCOUNT_NAME_CHARS = 256u;

    std::map<std::wstring, std::wstring> accountNames;

    std::wstring RenderFlags(ULONG flags, std::span<const kernel::FlagName> names, std::wstring_view separator) {
        std::wstring result;

        for (const kernel::FlagName& entry : names) {

            if ((flags & entry.flag) != entry.flag) continue;

            result += entry.name;
            result += separator;
            flags &= ~entry.flag;
        }

        if (flags) {
            result += std::format(L"0x{:X}", flags);
            result += separator;
        }

        if (!result.empty()) {
            result.resize(result.size() - separator.size());
        }

        return result;
    }


    std::wstring RenderDisposition(ULONG disposition) {
        const wchar_t* const name = kernel::CreateDispositionName(disposition);

        if (*name) return name;

        return std::to_wstring(disposition);
    }


    std::wstring RenderAttributes(ULONG fileAttributes) {

        if (!fileAttributes) return L"n/a";

        return RenderFlags(fileAttributes, kernel::FILE_ATTRIBUTE_LETTERS, L"");
    }


    std::wstring RenderShareMode(ULONG shareAccess) {

        if (!shareAccess) return L"None";

        return RenderFlags(shareAccess, kernel::SHARE_ACCESS_NAMES, L"|");
    }


    std::wstring RenderImpersonated(const protocol::CreateSupplement& createSupplement) {
        const PSID pSid = const_cast<uint8_t*>(createSupplement.impersonatedSid);
        wchar_t* pSidText = nullptr;

        if (!ConvertSidToStringSidW(pSid, &pSidText)) return L"[invalid sid]";

        std::wstring sidText = pSidText;
        LocalFree(pSidText);

        const auto [it, inserted] = accountNames.try_emplace(std::move(sidText));

        if (!inserted) return it->second;

        wchar_t name[ACCOUNT_NAME_CHARS]{};
        wchar_t domain[ACCOUNT_NAME_CHARS]{};
        DWORD nameChars = ACCOUNT_NAME_CHARS;
        DWORD domainChars = ACCOUNT_NAME_CHARS;
        SID_NAME_USE use{};

        if (LookupAccountSidW(nullptr, pSid, name, &nameChars, domain, &domainChars, &use)) {
            it->second = *domain ? std::format(L"{}\\{}", domain, name) : name;
        }
        else {
            it->second = it->first;
        }

        return it->second;
    }


    std::wstring RenderCreate(const protocol::RecordData& data) {
        const protocol::FltParameters& parameters = data.parameters;
        const protocol::CreateSupplement& createSupplement = data.supplement.create;
        std::wstring result;

        if (createSupplement.captured & protocol::CREATE_CAPTURED_DESIRED_ACCESS) {
            result += std::format(L"Desired Access: {}, ", RenderFlags(createSupplement.desiredAccess, kernel::DESIRED_ACCESS_NAMES, L"|"));
        }

        const ULONG disposition = parameters.create.options >> 24;
        const ULONG createOptions = parameters.create.options & 0x00FFFFFFu;

        result += std::format(L"Disposition: {}, ", RenderDisposition(disposition));

        const std::wstring options = RenderFlags(createOptions, kernel::CREATE_OPTION_NAMES, L"|");

        if (!options.empty()) {
            result += std::format(L"Options: {}, ", options);
        }

        result += std::format(L"Attributes: {}, ", RenderAttributes(parameters.create.fileAttributes));
        result += std::format(L"ShareMode: {}, ", RenderShareMode(parameters.create.shareAccess));

        if (disposition == kernel::FILE_OPEN) {
            result += L"AllocationSize: n/a, ";
        }
        else {
            result += std::format(L"AllocationSize: {}, ", parameters.create.allocationSize);
        }

        if (parameters.create.eaLength) {
            result += std::format(L"EaLength: {}, ", parameters.create.eaLength);
        }

        if (createSupplement.captured & protocol::CREATE_CAPTURED_IMPERSONATED_SID) {
            result += std::format(L"Impersonating: {}, ", RenderImpersonated(createSupplement));
        }

        if (data.status == 0 || data.information == kernel::FILE_EXISTS || data.information == kernel::FILE_DOES_NOT_EXIST) {
            const wchar_t* const openResult = kernel::OpenResultName(data.information);

            if (*openResult) {
                result += std::format(L"OpenResult: {}, ", openResult);
            }
        }

        const std::wstring_view ecpText = text::Extract(createSupplement.ecpText);

        if (!ecpText.empty()) {
            result += ecpText;
            result += L", ";
        }

        if (!result.empty()) {
            result.resize(result.size() - 2u);
        }

        return result;
    }


    std::wstring RenderOffset(int64_t byteOffset) {

        if (byteOffset == kernel::FILE_WRITE_TO_END_OF_FILE) return L"EOF";

        if (byteOffset == kernel::FILE_USE_FILE_POINTER_POSITION) return L"Current";

        return std::to_wstring(byteOffset);
    }


    std::wstring RenderReadWriteIoFlags(const protocol::RecordData& data) {
        std::wstring ioFlags;

        if (data.irpFlags & kernel::IRP_NOCACHE) {
            ioFlags += L"Non-cached|";
        }

        if (data.irpFlags & kernel::IRP_PAGING_IO) {
            ioFlags += L"Paging I/O|";
        }

        if ((data.irpFlags & kernel::IRP_PAGING_IO) && (data.irpFlags & kernel::IRP_SYNCHRONOUS_PAGING_IO)) {
            ioFlags += L"Synchronous Paging I/O|";
        }
        else if (data.irpFlags & kernel::IRP_SYNCHRONOUS_API) {
            ioFlags += L"Synchronous|";
        }

        if (data.operationFlags & kernel::SL_WRITE_THROUGH) {
            ioFlags += L"Write Through|";
        }

        if (!ioFlags.empty()) {
            ioFlags.resize(ioFlags.size() - 1u);
        }

        return ioFlags;
    }


    std::wstring RenderReadWrite(const protocol::RecordData& data) {
        const protocol::FltParameters& parameters = data.parameters;

        if (data.callbackMinorId & kernel::IRP_MN_COMPLETE) {

            if (!parameters.readWrite.mdlAddress) return {};

            return std::format(L"Mdl: 0x{:X}", parameters.readWrite.mdlAddress);
        }

        std::wstring details = std::format(L"Offset: {}, Length: {}", RenderOffset(parameters.readWrite.byteOffset), parameters.readWrite.length);

        if ((data.operationFlags & kernel::SL_KEY_SPECIFIED) || parameters.readWrite.key) {
            details += std::format(L", Key: 0x{:X}", parameters.readWrite.key);
        }

        const std::wstring ioFlags = RenderReadWriteIoFlags(data);

        if (!ioFlags.empty()) {
            details += L", ";
            details += ioFlags;
        }

        if (data.status >= 0 && data.information != parameters.readWrite.length) {
            details += std::format(L", Transferred: {}", data.information);
        }

        return details;
    }


    std::wstring RenderFileInformationClass(ULONG fileInformationClass) {
        const wchar_t* const name = kernel::FileInformationClassName(fileInformationClass);

        if (*name) return name;

        return std::to_wstring(fileInformationClass);
    }


    std::wstring RenderInformationParameters(ULONG fileInformationClass, ULONG length) {

        return std::format(L"Class: {}, Length: {}", RenderFileInformationClass(fileInformationClass), length);
    }


    std::span<const uint8_t> ExtractPayload(const protocol::InfoSupplement& info) {

        if (!(info.captured & protocol::INFO_CAPTURED_PAYLOAD)) return {};

        return { info.payload, info.capturedBytes };
    }


    template <typename T>
    bool ReadValue(std::span<const uint8_t> payload, T& value, size_t offset = 0u) {

        if (payload.size() < offset + sizeof(T)) return false;

        std::memcpy(&value, payload.data() + offset, sizeof(T));

        return true;
    }


    std::wstring RenderFileTime(LONGLONG fileTime) {

        if (fileTime < 0) return std::to_wstring(fileTime);

        const ULONGLONG ticks = static_cast<ULONGLONG>(fileTime);

        FILETIME systemFileTime{};
        systemFileTime.dwLowDateTime = static_cast<DWORD>(ticks);
        systemFileTime.dwHighDateTime = static_cast<DWORD>(ticks >> 32);

        FILETIME localFileTime{};

        if (!FileTimeToLocalFileTime(&systemFileTime, &localFileTime)) return L"TIME ERROR";

        SYSTEMTIME localTime{};

        if (!FileTimeToSystemTime(&localFileTime, &localTime)) return L"TIME ERROR";

        return std::format(L"{:04}-{:02}-{:02} {:02}:{:02}:{:02}", localTime.wYear, localTime.wMonth, localTime.wDay, localTime.wHour, localTime.wMinute, localTime.wSecond);
    }


    std::wstring RenderBasicPayload(const kernel::FILE_BASIC_INFORMATION& payload) {
        std::wstring result;

        if (payload.CreationTime) {
            result += std::format(L"CreationTime: {}, ", RenderFileTime(payload.CreationTime));
        }

        if (payload.LastAccessTime) {
            result += std::format(L"LastAccessTime: {}, ", RenderFileTime(payload.LastAccessTime));
        }

        if (payload.LastWriteTime) {
            result += std::format(L"LastWriteTime: {}, ", RenderFileTime(payload.LastWriteTime));
        }

        if (payload.ChangeTime) {
            result += std::format(L"ChangeTime: {}, ", RenderFileTime(payload.ChangeTime));
        }

        if (payload.FileAttributes) {
            result += std::format(L"FileAttributes: {}, ", RenderAttributes(payload.FileAttributes));
        }

        if (!result.empty()) {
            result.resize(result.size() - 2u);
        }

        return result;
    }


    std::wstring RenderStandardPayload(const kernel::FILE_STANDARD_INFORMATION& payload) {

        return std::format(L"AllocationSize: {}, EndOfFile: {}, NumberOfLinks: {}, DeletePending: {}, Directory: {}", payload.AllocationSize, payload.EndOfFile, payload.NumberOfLinks, payload.DeletePending ? L"True" : L"False", payload.Directory ? L"True" : L"False");
    }


    std::wstring RenderInternalPayload(const kernel::FILE_INTERNAL_INFORMATION& payload) {

        return std::format(L"IndexNumber: 0x{:X}", static_cast<ULONGLONG>(payload.IndexNumber));
    }


    std::wstring RenderEaPayload(const kernel::FILE_EA_INFORMATION& payload) {

        return std::format(L"EaSize: {}", payload.EaSize);
    }


    std::wstring RenderPayloadFileName(std::span<const uint8_t> nameData, ULONG fileNameLength) {
        const size_t nameBytes = fileNameLength < nameData.size() ? fileNameLength : nameData.size();

        std::wstring name(nameBytes / sizeof(wchar_t), L'\0');
        std::memcpy(name.data(), nameData.data(), name.size() * sizeof(wchar_t));

        if (nameBytes < fileNameLength) {
            // ellipsis
            name += L'\x2026';
        }

        return name;
    }


    std::wstring RenderNamePayload(const kernel::FILE_NAME_INFORMATION& payload, std::span<const uint8_t> nameData) {

        return std::format(L"Name: {}", RenderPayloadFileName(nameData, payload.FileNameLength));
    }


    std::wstring RenderPositionPayload(const kernel::FILE_POSITION_INFORMATION& payload) {

        return std::format(L"CurrentByteOffset: {}", payload.CurrentByteOffset);
    }


    std::wstring RenderAllPayload(const kernel::FILE_ALL_INFORMATION& payload, std::span<const uint8_t> nameData) {
        std::wstring result = RenderBasicPayload(payload.BasicInformation);

        if (!result.empty()) {
            result += L", ";
        }

        result += RenderStandardPayload(payload.StandardInformation);
        result += std::format(L", {}, {}, Access: {}, {}", RenderInternalPayload(payload.InternalInformation), RenderEaPayload(payload.EaInformation), RenderFlags(payload.AccessInformation.AccessFlags, kernel::DESIRED_ACCESS_NAMES, L"|"), RenderPositionPayload(payload.PositionInformation));

        if (payload.ModeInformation.Mode) {
            result += std::format(L", Mode: {}", RenderFlags(payload.ModeInformation.Mode, kernel::CREATE_OPTION_NAMES, L"|"));
        }

        result += std::format(L", AlignmentRequirement: {}, {}", payload.AlignmentInformation.AlignmentRequirement, RenderNamePayload(payload.NameInformation, nameData));

        return result;
    }


    std::wstring RenderStreamsPayload(std::span<const uint8_t> payload) {
        std::wstring result;
        ULONG offset = 0u;

        while (true) {
            kernel::FILE_STREAM_INFORMATION entry;

            if (!ReadValue(payload, entry, offset)) break;

            constexpr ULONG NAME_OFFSET = static_cast<ULONG>(offsetof(kernel::FILE_STREAM_INFORMATION, StreamName));
            result += std::format(L"StreamName: {}, StreamSize: {}, StreamAllocationSize: {}, ", RenderPayloadFileName(payload.subspan(offset + NAME_OFFSET), entry.StreamNameLength), entry.StreamSize, entry.StreamAllocationSize);

            if (!entry.NextEntryOffset || entry.NextEntryOffset > payload.size()) break;

            offset += entry.NextEntryOffset;
        }

        if (!result.empty()) {
            result.resize(result.size() - 2u);
        }

        return result;
    }


    std::wstring RenderCompressionPayload(const kernel::FILE_COMPRESSION_INFORMATION& payload) {
        const wchar_t* const formatName = kernel::CompressionFormatName(payload.CompressionFormat);
        std::wstring result = std::format(L"CompressedFileSize: {}", payload.CompressedFileSize);

        if (*formatName) {
            result += std::format(L", CompressionFormat: {}", formatName);
        }
        else {
            result += std::format(L", CompressionFormat: {}", payload.CompressionFormat);
        }

        if (payload.CompressionFormat != COMPRESSION_FORMAT_NONE) {
            result += std::format(L", CompressionUnitShift: {}, ChunkShift: {}, ClusterShift: {}", payload.CompressionUnitShift, payload.ChunkShift, payload.ClusterShift);
        }

        return result;
    }


    std::wstring RenderNetworkOpenPayload(const kernel::FILE_NETWORK_OPEN_INFORMATION& payload) {
        std::wstring result;

        if (payload.CreationTime) {
            result += std::format(L"CreationTime: {}, ", RenderFileTime(payload.CreationTime));
        }

        if (payload.LastAccessTime) {
            result += std::format(L"LastAccessTime: {}, ", RenderFileTime(payload.LastAccessTime));
        }

        if (payload.LastWriteTime) {
            result += std::format(L"LastWriteTime: {}, ", RenderFileTime(payload.LastWriteTime));
        }

        if (payload.ChangeTime) {
            result += std::format(L"ChangeTime: {}, ", RenderFileTime(payload.ChangeTime));
        }

        result += std::format(L"AllocationSize: {}, EndOfFile: {}", payload.AllocationSize, payload.EndOfFile);

        if (payload.FileAttributes) {
            result += std::format(L", FileAttributes: {}", RenderAttributes(payload.FileAttributes));
        }

        return result;
    }


    std::wstring RenderReparseTag(ULONG reparseTag) {
        const wchar_t* const tagName = kernel::ReparseTagName(reparseTag);

        if (*tagName) return tagName;

        return std::format(L"{:08X}", reparseTag);
    }


    std::wstring RenderAttributeTagPayload(const kernel::FILE_ATTRIBUTE_TAG_INFORMATION& payload) {
        std::wstring result = std::format(L"Attributes: {}", RenderAttributes(payload.FileAttributes));

        if (payload.ReparseTag) {
            result += std::format(L", ReparseTag: {}", RenderReparseTag(payload.ReparseTag));
        }

        return result;
    }


    std::wstring RenderHardLinksPayload(std::span<const uint8_t> payload) {
        ULONG bytesNeeded;
        ULONG entriesReturned;

        if (!ReadValue(payload, bytesNeeded) || !ReadValue(payload, entriesReturned, offsetof(kernel::FILE_LINKS_INFORMATION, EntriesReturned))) return {};

        std::wstring result = std::format(L"BytesNeeded: {}, EntriesReturned: {}", bytesNeeded, entriesReturned);
        ULONG offset = static_cast<ULONG>(offsetof(kernel::FILE_LINKS_INFORMATION, Entry));

        while (true) {
            kernel::FILE_LINK_ENTRY_INFORMATION entry;

            if (!ReadValue(payload, entry, offset)) break;

            constexpr ULONG NAME_OFFSET = static_cast<ULONG>(offsetof(kernel::FILE_LINK_ENTRY_INFORMATION, FileName));
            result += std::format(L", ParentFileId: 0x{:X}, FileName: {}", static_cast<ULONGLONG>(entry.ParentFileId), RenderPayloadFileName(payload.subspan(offset + NAME_OFFSET), static_cast<ULONG>(entry.FileNameLength * sizeof(wchar_t))));

            if (!entry.NextEntryOffset || entry.NextEntryOffset > payload.size()) break;

            offset += entry.NextEntryOffset;
        }

        return result;
    }


    std::wstring RenderRemoteProtocolPayload(const kernel::FILE_REMOTE_PROTOCOL_INFORMATION& payload) {
        const wchar_t* const protocolName = kernel::RemoteProtocolName(payload.Protocol);
        std::wstring result;

        if (*protocolName) {
            result = std::format(L"Protocol: {}", protocolName);
        }
        else {
            result = std::format(L"Protocol: 0x{:X}", payload.Protocol);
        }

        result += std::format(L", Version: {}.{}.{}", payload.ProtocolMajorVersion, payload.ProtocolMinorVersion, payload.ProtocolRevision);

        if (payload.Flags) {
            result += std::format(L", Flags: {}", RenderFlags(payload.Flags, kernel::REMOTE_PROTOCOL_FLAG_NAMES, L"|"));
        }

        return result;
    }


    std::wstring RenderFileId(std::span<const UCHAR, 16u> fileId) {
        ULONGLONG low;
        ULONGLONG high;

        std::memcpy(&low, fileId.data(), sizeof(low));
        std::memcpy(&high, fileId.data() + sizeof(low), sizeof(high));

        if (!high) return std::format(L"0x{:X}", low);

        return std::format(L"0x{:X}{:016X}", high, low);
    }


    std::wstring RenderIdPayload(const kernel::FILE_ID_INFORMATION& payload) {

        return std::format(L"VolumeSerialNumber: 0x{:X}, FileId: {}", payload.VolumeSerialNumber, RenderFileId(payload.FileId));
    }


    std::wstring RenderStatPayload(const kernel::FILE_STAT_INFORMATION& payload) {
        std::wstring result = std::format(L"FileId: 0x{:X}", static_cast<ULONGLONG>(payload.FileId));

        if (payload.CreationTime) {
            result += std::format(L", CreationTime: {}", RenderFileTime(payload.CreationTime));
        }

        if (payload.LastAccessTime) {
            result += std::format(L", LastAccessTime: {}", RenderFileTime(payload.LastAccessTime));
        }

        if (payload.LastWriteTime) {
            result += std::format(L", LastWriteTime: {}", RenderFileTime(payload.LastWriteTime));
        }

        if (payload.ChangeTime) {
            result += std::format(L", ChangeTime: {}", RenderFileTime(payload.ChangeTime));
        }

        result += std::format(L", AllocationSize: {}, EndOfFile: {}", payload.AllocationSize, payload.EndOfFile);

        if (payload.FileAttributes) {
            result += std::format(L", FileAttributes: {}", RenderAttributes(payload.FileAttributes));
        }

        if (payload.ReparseTag) {
            result += std::format(L", ReparseTag: {}", RenderReparseTag(payload.ReparseTag));
        }

        result += std::format(L", NumberOfLinks: {}, EffectiveAccess: {}", payload.NumberOfLinks, RenderFlags(payload.EffectiveAccess, kernel::DESIRED_ACCESS_NAMES, L"|"));

        return result;
    }


    std::wstring RenderStatLxPayload(const kernel::FILE_STAT_LX_INFORMATION& payload) {
        kernel::FILE_STAT_INFORMATION stat;
        std::memcpy(&stat, &payload, sizeof(stat));

        std::wstring result = RenderStatPayload(stat);

        if (payload.LxFlags & LX_FILE_METADATA_HAS_UID) {
            result += std::format(L", LxUid: {}", payload.LxUid);
        }

        if (payload.LxFlags & LX_FILE_METADATA_HAS_GID) {
            result += std::format(L", LxGid: {}", payload.LxGid);
        }

        if (payload.LxFlags & LX_FILE_METADATA_HAS_MODE) {
            result += std::format(L", LxMode: 0{:o}", payload.LxMode);
        }

        if (payload.LxFlags & LX_FILE_METADATA_HAS_DEVICE_ID) {
            result += std::format(L", LxDeviceId: {}:{}", payload.LxDeviceIdMajor, payload.LxDeviceIdMinor);
        }

        if (payload.LxFlags & LX_FILE_CASE_SENSITIVE_DIR) {
            result += L", CaseSensitiveDir: True";
        }

        return result;
    }


    std::wstring RenderCaseSensitivePayload(const kernel::FILE_CASE_SENSITIVE_INFORMATION& payload) {

        return std::format(L"CaseSensitiveDir: {}", (payload.Flags & FILE_CS_FLAG_CASE_SENSITIVE_DIR) ? L"True" : L"False");
    }


    std::wstring RenderStatBasicPayload(const kernel::FILE_STAT_BASIC_INFORMATION& payload) {
        std::wstring result = std::format(L"FileId: 0x{:X}", static_cast<ULONGLONG>(payload.FileId));

        if (payload.CreationTime) {
            result += std::format(L", CreationTime: {}", RenderFileTime(payload.CreationTime));
        }

        if (payload.LastAccessTime) {
            result += std::format(L", LastAccessTime: {}", RenderFileTime(payload.LastAccessTime));
        }

        if (payload.LastWriteTime) {
            result += std::format(L", LastWriteTime: {}", RenderFileTime(payload.LastWriteTime));
        }

        if (payload.ChangeTime) {
            result += std::format(L", ChangeTime: {}", RenderFileTime(payload.ChangeTime));
        }

        result += std::format(L", AllocationSize: {}, EndOfFile: {}", payload.AllocationSize, payload.EndOfFile);

        if (payload.FileAttributes) {
            result += std::format(L", FileAttributes: {}", RenderAttributes(payload.FileAttributes));
        }

        if (payload.ReparseTag) {
            result += std::format(L", ReparseTag: {}", RenderReparseTag(payload.ReparseTag));
        }

        result += std::format(L", NumberOfLinks: {}, DeviceType: {}, DeviceCharacteristics: 0x{:X}, VolumeSerialNumber: 0x{:X}, FileId128: {}", payload.NumberOfLinks, payload.DeviceType, payload.DeviceCharacteristics, static_cast<ULONGLONG>(payload.VolumeSerialNumber), RenderFileId(payload.FileId128));

        return result;
    }


    std::wstring RenderQueryInformation(const protocol::RecordData& data) {
        const protocol::FltParameters& parameters = data.parameters;
        std::wstring details = RenderInformationParameters(parameters.queryFileInformation.fileInformationClass, parameters.queryFileInformation.length);
        const std::span<const uint8_t> payload = ExtractPayload(data.supplement.info);
        std::wstring payloadText;

        switch (parameters.queryFileInformation.fileInformationClass) {

            case kernel::FileBasicInformation: {
                kernel::FILE_BASIC_INFORMATION basic;

                if (ReadValue(payload, basic)) {
                    payloadText = RenderBasicPayload(basic);
                }

                break;
            }

            case kernel::FileStandardInformation: {
                kernel::FILE_STANDARD_INFORMATION standard;

                if (ReadValue(payload, standard)) {
                    payloadText = RenderStandardPayload(standard);
                }

                break;
            }

            case kernel::FileInternalInformation: {
                kernel::FILE_INTERNAL_INFORMATION internal;

                if (ReadValue(payload, internal)) {
                    payloadText = RenderInternalPayload(internal);
                }

                break;
            }

            case kernel::FileEaInformation: {
                kernel::FILE_EA_INFORMATION ea;

                if (ReadValue(payload, ea)) {
                    payloadText = RenderEaPayload(ea);
                }

                break;
            }

            case kernel::FileNameInformation:
            case kernel::FileAlternateNameInformation:
            case kernel::FileNormalizedNameInformation:
            case kernel::FileNetworkPhysicalNameInformation: {
                kernel::FILE_NAME_INFORMATION name;

                if (ReadValue(payload, name)) {
                    constexpr ULONG NAME_OFFSET = static_cast<ULONG>(offsetof(kernel::FILE_NAME_INFORMATION, FileName));
                    payloadText = RenderNamePayload(name, payload.subspan(NAME_OFFSET));
                }

                break;
            }

            case kernel::FilePositionInformation: {
                kernel::FILE_POSITION_INFORMATION position;

                if (ReadValue(payload, position)) {
                    payloadText = RenderPositionPayload(position);
                }

                break;
            }

            case kernel::FileAllInformation: {
                kernel::FILE_ALL_INFORMATION all;

                if (ReadValue(payload, all)) {
                    constexpr ULONG NAME_OFFSET = static_cast<ULONG>(offsetof(kernel::FILE_ALL_INFORMATION, NameInformation.FileName));
                    payloadText = RenderAllPayload(all, payload.subspan(NAME_OFFSET));
                }

                break;
            }

            case kernel::FileStreamInformation:
                payloadText = RenderStreamsPayload(payload);

                break;

            case kernel::FileCompressionInformation: {
                kernel::FILE_COMPRESSION_INFORMATION compression;

                if (ReadValue(payload, compression)) {
                    payloadText = RenderCompressionPayload(compression);
                }

                break;
            }

            case kernel::FileNetworkOpenInformation: {
                kernel::FILE_NETWORK_OPEN_INFORMATION networkOpen;

                if (ReadValue(payload, networkOpen)) {
                    payloadText = RenderNetworkOpenPayload(networkOpen);
                }

                break;
            }

            case kernel::FileAttributeTagInformation: {
                kernel::FILE_ATTRIBUTE_TAG_INFORMATION attributeTag;

                if (ReadValue(payload, attributeTag)) {
                    payloadText = RenderAttributeTagPayload(attributeTag);
                }

                break;
            }

            case kernel::FileHardLinkInformation:
                payloadText = RenderHardLinksPayload(payload);

                break;

            case kernel::FileRemoteProtocolInformation: {
                kernel::FILE_REMOTE_PROTOCOL_INFORMATION remoteProtocol;

                if (ReadValue(payload, remoteProtocol)) {
                    payloadText = RenderRemoteProtocolPayload(remoteProtocol);
                }

                break;
            }

            case kernel::FileIdInformation: {
                kernel::FILE_ID_INFORMATION id;

                if (ReadValue(payload, id)) {
                    payloadText = RenderIdPayload(id);
                }

                break;
            }

            case kernel::FileStatInformation: {
                kernel::FILE_STAT_INFORMATION stat;

                if (ReadValue(payload, stat)) {
                    payloadText = RenderStatPayload(stat);
                }

                break;
            }

            case kernel::FileStatLxInformation: {
                kernel::FILE_STAT_LX_INFORMATION statLx;

                if (ReadValue(payload, statLx)) {
                    payloadText = RenderStatLxPayload(statLx);
                }

                break;
            }

            case kernel::FileCaseSensitiveInformation: {
                kernel::FILE_CASE_SENSITIVE_INFORMATION caseSensitive;

                if (ReadValue(payload, caseSensitive)) {
                    payloadText = RenderCaseSensitivePayload(caseSensitive);
                }

                break;
            }

            case kernel::FileStatBasicInformation: {
                kernel::FILE_STAT_BASIC_INFORMATION statBasic;

                if (ReadValue(payload, statBasic)) {
                    payloadText = RenderStatBasicPayload(statBasic);
                }

                break;
            }

        }

        if (!payloadText.empty()) {
            details += L", ";
            details += payloadText;
        }

        return details;
    }


    std::wstring RenderRenameParameters(const protocol::FltParameters& parameters) {

        return std::format(L"ReplaceIfExists: {}", parameters.setFileInformation.flags.replaceIfExists ? L"True" : L"False");
    }


    std::wstring RenderRenamePayload(const kernel::FILE_RENAME_INFORMATION& payload, std::span<const uint8_t> nameData) {

        return std::format(L"FileName: {}", RenderPayloadFileName(nameData, payload.FileNameLength));
    }


    std::wstring RenderDispositionPayload(const kernel::FILE_DISPOSITION_INFORMATION& payload) {

        return std::format(L"Delete: {}", payload.DeletePending ? L"True" : L"False");
    }


    std::wstring RenderAllocationPayload(const kernel::FILE_ALLOCATION_INFORMATION& payload) {

        return std::format(L"AllocationSize: {}", payload.AllocationSize);
    }


    std::wstring RenderEndOfFileParameters(const protocol::FltParameters& parameters) {

        if (!parameters.setFileInformation.flags.advanceOnly) return {};

        return L"AdvanceOnly: True";
    }


    std::wstring RenderEndOfFilePayload(const kernel::FILE_END_OF_FILE_INFORMATION& payload) {

        return std::format(L"EndOfFile: {}", payload.EndOfFile);
    }


    std::wstring RenderMoveClusterParameters(const protocol::FltParameters& parameters) {

        return std::format(L"ClusterCount: {}", parameters.setFileInformation.clusterCount);
    }


    std::wstring RenderValidDataLengthPayload(const kernel::FILE_VALID_DATA_LENGTH_INFORMATION& payload) {

        return std::format(L"ValidDataLength: {}", payload.ValidDataLength);
    }


    std::wstring RenderShortNamePayload(const kernel::FILE_NAME_INFORMATION& payload, std::span<const uint8_t> nameData) {

        return std::format(L"FileName: {}", RenderPayloadFileName(nameData, payload.FileNameLength));
    }


    std::wstring RenderDispositionExPayload(const kernel::FILE_DISPOSITION_INFORMATION_EX& payload) {

        if (payload.Flags == kernel::FILE_DISPOSITION_DO_NOT_DELETE) return L"Flags: Do Not Delete";

        return std::format(L"Flags: {}", RenderFlags(payload.Flags, kernel::DISPOSITION_FLAG_NAMES, L"|"));
    }


    std::wstring RenderRenameExPayload(const kernel::FILE_RENAME_INFORMATION_EX& payload, std::span<const uint8_t> nameData) {
        std::wstring result;

        if (payload.Flags) {
            result = std::format(L"Flags: {}, ", RenderFlags(payload.Flags, kernel::RENAME_FLAG_NAMES, L"|"));
        }

        result += std::format(L"FileName: {}", RenderPayloadFileName(nameData, payload.FileNameLength));

        return result;
    }


    std::wstring RenderSetInformation(const protocol::RecordData& data) {
        const protocol::FltParameters& parameters = data.parameters;
        std::wstring details = RenderInformationParameters(parameters.setFileInformation.fileInformationClass, parameters.setFileInformation.length);
        const std::span<const uint8_t> payload = ExtractPayload(data.supplement.info);
        std::wstring parametersText;
        std::wstring payloadText;

        switch (parameters.setFileInformation.fileInformationClass) {

            case kernel::FileBasicInformation: {
                kernel::FILE_BASIC_INFORMATION basic;

                if (ReadValue(payload, basic)) {
                    payloadText = RenderBasicPayload(basic);
                }

                break;
            }

            case kernel::FileRenameInformation:
            case kernel::FileLinkInformation:
            case kernel::FileRenameInformationBypassAccessCheck:
            case kernel::FileLinkInformationBypassAccessCheck: {
                parametersText = RenderRenameParameters(parameters);

                kernel::FILE_RENAME_INFORMATION rename;

                if (ReadValue(payload, rename)) {
                    constexpr ULONG NAME_OFFSET = static_cast<ULONG>(offsetof(kernel::FILE_RENAME_INFORMATION, FileName));
                    payloadText = RenderRenamePayload(rename, payload.subspan(NAME_OFFSET));
                }

                break;
            }

            case kernel::FileDispositionInformation: {
                kernel::FILE_DISPOSITION_INFORMATION disposition;

                if (ReadValue(payload, disposition)) {
                    payloadText = RenderDispositionPayload(disposition);
                }

                break;
            }

            case kernel::FileAllocationInformation: {
                kernel::FILE_ALLOCATION_INFORMATION allocation;

                if (ReadValue(payload, allocation)) {
                    payloadText = RenderAllocationPayload(allocation);
                }

                break;
            }

            case kernel::FileEndOfFileInformation: {
                parametersText = RenderEndOfFileParameters(parameters);

                kernel::FILE_END_OF_FILE_INFORMATION endOfFile;

                if (ReadValue(payload, endOfFile)) {
                    payloadText = RenderEndOfFilePayload(endOfFile);
                }

                break;
            }

            case kernel::FileMoveClusterInformation:
                parametersText = RenderMoveClusterParameters(parameters);

                break;

            case kernel::FileValidDataLengthInformation: {
                kernel::FILE_VALID_DATA_LENGTH_INFORMATION validDataLength;

                if (ReadValue(payload, validDataLength)) {
                    payloadText = RenderValidDataLengthPayload(validDataLength);
                }

                break;
            }

            case kernel::FileShortNameInformation: {
                kernel::FILE_NAME_INFORMATION shortName;

                if (ReadValue(payload, shortName)) {
                    constexpr ULONG NAME_OFFSET = static_cast<ULONG>(offsetof(kernel::FILE_NAME_INFORMATION, FileName));
                    payloadText = RenderShortNamePayload(shortName, payload.subspan(NAME_OFFSET));
                }

                break;
            }

            case kernel::FileDispositionInformationEx: {
                kernel::FILE_DISPOSITION_INFORMATION_EX dispositionEx;

                if (ReadValue(payload, dispositionEx)) {
                    payloadText = RenderDispositionExPayload(dispositionEx);
                }

                break;
            }

            case kernel::FileRenameInformationEx:
            case kernel::FileRenameInformationExBypassAccessCheck:
            case kernel::FileLinkInformationEx:
            case kernel::FileLinkInformationExBypassAccessCheck: {
                kernel::FILE_RENAME_INFORMATION_EX renameEx;

                if (ReadValue(payload, renameEx)) {
                    constexpr ULONG NAME_OFFSET = static_cast<ULONG>(offsetof(kernel::FILE_RENAME_INFORMATION_EX, FileName));
                    payloadText = RenderRenameExPayload(renameEx, payload.subspan(NAME_OFFSET));
                }

                break;
            }

            case kernel::FileCaseSensitiveInformation:
            case kernel::FileCaseSensitiveInformationForceAccessCheck: {
                kernel::FILE_CASE_SENSITIVE_INFORMATION caseSensitive;

                if (ReadValue(payload, caseSensitive)) {
                    payloadText = RenderCaseSensitivePayload(caseSensitive);
                }

                break;
            }

        }

        if (!parametersText.empty()) {
            details += L", ";
            details += parametersText;
        }

        if (!payloadText.empty()) {
            details += L", ";
            details += payloadText;
        }

        return details;
    }

}

namespace mimo {

    namespace log {

        namespace details {

            std::wstring Render(const protocol::RecordData& data) {

                if (data.transactionNotify) return {};

                switch (data.callbackMajorId) {

                    case kernel::IRP_MJ_CREATE:

                        return RenderCreate(data);

                    case kernel::IRP_MJ_READ:
                    case kernel::IRP_MJ_WRITE:

                        return RenderReadWrite(data);

                    case kernel::IRP_MJ_QUERY_INFORMATION:

                        return RenderQueryInformation(data);

                    case kernel::IRP_MJ_SET_INFORMATION:

                        return RenderSetInformation(data);

                }

                return {};
            }

        }

    }

}
