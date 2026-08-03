#include "info.h"

#include "payload.h"

#include "..\names.h"
#include "..\values.h"

#include "..\..\kernel.h"
#include "..\..\text.h"

#include "..\..\..\..\inc\protocol.h"

#include <Windows.h>

#include <cstddef>
#include <cstring>
#include <format>
#include <span>
#include <string>

using namespace mimo;

namespace {

    std::wstring RenderInformationParameters(ULONG fileInformationClass, ULONG length) {

        return std::format(L"Class: {}, Length: {}", log::names::RenderFileInformationClass(fileInformationClass), length);
    }


    std::span<const uint8_t> ExtractPayload(const protocol::QueryInfoSupplement& supplement) {

        if (!(supplement.captured & protocol::QUERY_INFO_CAPTURED_PAYLOAD)) return {};

        return { supplement.payload, supplement.capturedBytes };
    }


    std::span<const uint8_t> ExtractPayload(const protocol::SetInfoSupplement& supplement) {

        if (!(supplement.captured & protocol::SET_INFO_CAPTURED_PAYLOAD)) return {};

        return { supplement.payload, supplement.capturedBytes };
    }


    std::wstring RenderBasicPayload(const kernel::FILE_BASIC_INFORMATION& payload) {
        std::wstring result;

        if (payload.CreationTime) {
            result += std::format(L"CreationTime: {}, ", log::values::RenderFileTime(payload.CreationTime));
        }

        if (payload.LastAccessTime) {
            result += std::format(L"LastAccessTime: {}, ", log::values::RenderFileTime(payload.LastAccessTime));
        }

        if (payload.LastWriteTime) {
            result += std::format(L"LastWriteTime: {}, ", log::values::RenderFileTime(payload.LastWriteTime));
        }

        if (payload.ChangeTime) {
            result += std::format(L"ChangeTime: {}, ", log::values::RenderFileTime(payload.ChangeTime));
        }

        if (payload.FileAttributes) {
            result += std::format(L"FileAttributes: {}, ", log::names::RenderFileAttributes(payload.FileAttributes));
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


    std::wstring RenderNamePayload(const kernel::FILE_NAME_INFORMATION& payload, std::span<const uint8_t> nameData) {

        return std::format(L"Name: {}", log::details::payload::RenderFileName(nameData, payload.FileNameLength));
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
        result += std::format(L", {}, {}, Access: {}, {}", RenderInternalPayload(payload.InternalInformation), RenderEaPayload(payload.EaInformation), log::names::RenderDesiredAccess(payload.AccessInformation.AccessFlags), RenderPositionPayload(payload.PositionInformation));

        if (payload.ModeInformation.Mode) {
            result += std::format(L", Mode: {}", log::names::RenderCreateOptions(payload.ModeInformation.Mode));
        }

        result += std::format(L", AlignmentRequirement: {}, {}", payload.AlignmentInformation.AlignmentRequirement, RenderNamePayload(payload.NameInformation, nameData));

        return result;
    }


    std::wstring RenderStreamsPayload(std::span<const uint8_t> payload) {
        std::wstring result;
        ULONG offset = 0u;

        while (true) {
            kernel::FILE_STREAM_INFORMATION entry;

            if (!log::details::payload::ReadValue(payload, entry, offset)) break;

            constexpr ULONG NAME_OFFSET = static_cast<ULONG>(offsetof(kernel::FILE_STREAM_INFORMATION, StreamName));
            result += std::format(L"StreamName: {}, StreamSize: {}, StreamAllocationSize: {}, ", log::details::payload::RenderFileName(payload.subspan(offset + NAME_OFFSET), entry.StreamNameLength), entry.StreamSize, entry.StreamAllocationSize);

            if (!entry.NextEntryOffset || entry.NextEntryOffset > payload.size()) break;

            offset += entry.NextEntryOffset;
        }

        if (!result.empty()) {
            result.resize(result.size() - 2u);
        }

        return result;
    }


    std::wstring RenderCompressionPayload(const kernel::FILE_COMPRESSION_INFORMATION& payload) {
        std::wstring result = std::format(L"CompressedFileSize: {}, CompressionFormat: {}", payload.CompressedFileSize, log::names::RenderCompressionFormat(payload.CompressionFormat));

        if (payload.CompressionFormat != COMPRESSION_FORMAT_NONE) {
            result += std::format(L", CompressionUnitShift: {}, ChunkShift: {}, ClusterShift: {}", payload.CompressionUnitShift, payload.ChunkShift, payload.ClusterShift);
        }

        return result;
    }


    std::wstring RenderNetworkOpenPayload(const kernel::FILE_NETWORK_OPEN_INFORMATION& payload) {
        std::wstring result;

        if (payload.CreationTime) {
            result += std::format(L"CreationTime: {}, ", log::values::RenderFileTime(payload.CreationTime));
        }

        if (payload.LastAccessTime) {
            result += std::format(L"LastAccessTime: {}, ", log::values::RenderFileTime(payload.LastAccessTime));
        }

        if (payload.LastWriteTime) {
            result += std::format(L"LastWriteTime: {}, ", log::values::RenderFileTime(payload.LastWriteTime));
        }

        if (payload.ChangeTime) {
            result += std::format(L"ChangeTime: {}, ", log::values::RenderFileTime(payload.ChangeTime));
        }

        result += std::format(L"AllocationSize: {}, EndOfFile: {}", payload.AllocationSize, payload.EndOfFile);

        if (payload.FileAttributes) {
            result += std::format(L", FileAttributes: {}", log::names::RenderFileAttributes(payload.FileAttributes));
        }

        return result;
    }


    std::wstring RenderAttributeTagPayload(const kernel::FILE_ATTRIBUTE_TAG_INFORMATION& payload) {
        std::wstring result = std::format(L"Attributes: {}", log::names::RenderFileAttributes(payload.FileAttributes));

        if (payload.ReparseTag) {
            result += std::format(L", ReparseTag: {}", log::names::RenderReparseTag(payload.ReparseTag));
        }

        return result;
    }


    std::wstring RenderHardLinksPayload(std::span<const uint8_t> payload) {
        ULONG bytesNeeded;
        ULONG entriesReturned;

        if (!log::details::payload::ReadValue(payload, bytesNeeded) || !log::details::payload::ReadValue(payload, entriesReturned, offsetof(kernel::FILE_LINKS_INFORMATION, EntriesReturned))) return {};

        std::wstring result = std::format(L"BytesNeeded: {}, EntriesReturned: {}", bytesNeeded, entriesReturned);
        ULONG offset = static_cast<ULONG>(offsetof(kernel::FILE_LINKS_INFORMATION, Entry));

        while (true) {
            kernel::FILE_LINK_ENTRY_INFORMATION entry;

            if (!log::details::payload::ReadValue(payload, entry, offset)) break;

            constexpr ULONG NAME_OFFSET = static_cast<ULONG>(offsetof(kernel::FILE_LINK_ENTRY_INFORMATION, FileName));
            result += std::format(L", ParentFileId: 0x{:X}, FileName: {}", static_cast<ULONGLONG>(entry.ParentFileId), log::details::payload::RenderFileName(payload.subspan(offset + NAME_OFFSET), static_cast<uint32_t>(entry.FileNameLength * sizeof(wchar_t))));

            if (!entry.NextEntryOffset || entry.NextEntryOffset > payload.size()) break;

            offset += entry.NextEntryOffset;
        }

        return result;
    }


    std::wstring RenderRemoteProtocolPayload(const kernel::FILE_REMOTE_PROTOCOL_INFORMATION& payload) {
        std::wstring result = std::format(L"Protocol: {}", log::names::RenderRemoteProtocol(payload.Protocol));

        result += std::format(L", Version: {}.{}.{}", payload.ProtocolMajorVersion, payload.ProtocolMinorVersion, payload.ProtocolRevision);

        if (payload.Flags) {
            result += std::format(L", Flags: {}", log::names::RenderRemoteProtocolFlags(payload.Flags));
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
            result += std::format(L", CreationTime: {}", log::values::RenderFileTime(payload.CreationTime));
        }

        if (payload.LastAccessTime) {
            result += std::format(L", LastAccessTime: {}", log::values::RenderFileTime(payload.LastAccessTime));
        }

        if (payload.LastWriteTime) {
            result += std::format(L", LastWriteTime: {}", log::values::RenderFileTime(payload.LastWriteTime));
        }

        if (payload.ChangeTime) {
            result += std::format(L", ChangeTime: {}", log::values::RenderFileTime(payload.ChangeTime));
        }

        result += std::format(L", AllocationSize: {}, EndOfFile: {}", payload.AllocationSize, payload.EndOfFile);

        if (payload.FileAttributes) {
            result += std::format(L", FileAttributes: {}", log::names::RenderFileAttributes(payload.FileAttributes));
        }

        if (payload.ReparseTag) {
            result += std::format(L", ReparseTag: {}", log::names::RenderReparseTag(payload.ReparseTag));
        }

        result += std::format(L", NumberOfLinks: {}, EffectiveAccess: {}", payload.NumberOfLinks, log::names::RenderDesiredAccess(payload.EffectiveAccess));

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
            result += std::format(L", CreationTime: {}", log::values::RenderFileTime(payload.CreationTime));
        }

        if (payload.LastAccessTime) {
            result += std::format(L", LastAccessTime: {}", log::values::RenderFileTime(payload.LastAccessTime));
        }

        if (payload.LastWriteTime) {
            result += std::format(L", LastWriteTime: {}", log::values::RenderFileTime(payload.LastWriteTime));
        }

        if (payload.ChangeTime) {
            result += std::format(L", ChangeTime: {}", log::values::RenderFileTime(payload.ChangeTime));
        }

        result += std::format(L", AllocationSize: {}, EndOfFile: {}", payload.AllocationSize, payload.EndOfFile);

        if (payload.FileAttributes) {
            result += std::format(L", FileAttributes: {}", log::names::RenderFileAttributes(payload.FileAttributes));
        }

        if (payload.ReparseTag) {
            result += std::format(L", ReparseTag: {}", log::names::RenderReparseTag(payload.ReparseTag));
        }

        result += std::format(L", NumberOfLinks: {}, DeviceType: {}, DeviceCharacteristics: 0x{:X}, VolumeSerialNumber: 0x{:X}, FileId128: {}", payload.NumberOfLinks, payload.DeviceType, payload.DeviceCharacteristics, static_cast<ULONGLONG>(payload.VolumeSerialNumber), RenderFileId(payload.FileId128));

        return result;
    }


    std::wstring RenderRenameParameters(const protocol::FltParameters& parameters) {

        return std::format(L"ReplaceIfExists: {}", parameters.setFileInformation.flags.replaceIfExists ? L"True" : L"False");
    }


    std::wstring RenderTargetName(const protocol::SetInfoSupplement& supplement) {

        if (!(supplement.captured & protocol::SET_INFO_CAPTURED_TARGET_NAME)) return {};

        return std::format(L"FileName: {}", text::MarkTruncated(text::Extract(supplement.targetName), supplement.captured & protocol::SET_INFO_TRUNCATED_TARGET_NAME));
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

        return std::format(L"FileName: {}", log::details::payload::RenderFileName(nameData, payload.FileNameLength));
    }


    std::wstring RenderDispositionExPayload(const kernel::FILE_DISPOSITION_INFORMATION_EX& payload) {

        if (payload.Flags == kernel::FILE_DISPOSITION_DO_NOT_DELETE) return L"Flags: Do Not Delete";

        return std::format(L"Flags: {}", log::names::RenderDispositionFlags(payload.Flags));
    }


    std::wstring RenderRenameExPayload(const kernel::FILE_RENAME_INFORMATION_EX& payload) {

        if (!payload.Flags) return {};

        return std::format(L"Flags: {}", log::names::RenderRenameFlags(payload.Flags));
    }

}

namespace mimo {

    namespace log {

        namespace details {

            namespace info {

                std::wstring RenderQuery(const protocol::RecordData& data) {
                    const protocol::FltParameters& parameters = data.parameters;
                    std::wstring details = RenderInformationParameters(parameters.queryFileInformation.fileInformationClass, parameters.queryFileInformation.length);
                    const std::span<const uint8_t> payload = ExtractPayload(data.supplement.queryInfo);
                    std::wstring payloadText;

                    switch (parameters.queryFileInformation.fileInformationClass) {

                        case kernel::FileBasicInformation: {
                            kernel::FILE_BASIC_INFORMATION basic;

                            if (payload::ReadValue(payload, basic)) {
                                payloadText = RenderBasicPayload(basic);
                            }

                            break;
                        }

                        case kernel::FileStandardInformation: {
                            kernel::FILE_STANDARD_INFORMATION standard;

                            if (payload::ReadValue(payload, standard)) {
                                payloadText = RenderStandardPayload(standard);
                            }

                            break;
                        }

                        case kernel::FileInternalInformation: {
                            kernel::FILE_INTERNAL_INFORMATION internal;

                            if (payload::ReadValue(payload, internal)) {
                                payloadText = RenderInternalPayload(internal);
                            }

                            break;
                        }

                        case kernel::FileEaInformation: {
                            kernel::FILE_EA_INFORMATION ea;

                            if (payload::ReadValue(payload, ea)) {
                                payloadText = RenderEaPayload(ea);
                            }

                            break;
                        }

                        case kernel::FileNameInformation:
                        case kernel::FileAlternateNameInformation:
                        case kernel::FileNormalizedNameInformation:
                        case kernel::FileNetworkPhysicalNameInformation: {
                            kernel::FILE_NAME_INFORMATION name;

                            if (payload::ReadValue(payload, name)) {
                                constexpr ULONG NAME_OFFSET = static_cast<ULONG>(offsetof(kernel::FILE_NAME_INFORMATION, FileName));
                                payloadText = RenderNamePayload(name, payload.subspan(NAME_OFFSET));
                            }

                            break;
                        }

                        case kernel::FilePositionInformation: {
                            kernel::FILE_POSITION_INFORMATION position;

                            if (payload::ReadValue(payload, position)) {
                                payloadText = RenderPositionPayload(position);
                            }

                            break;
                        }

                        case kernel::FileAllInformation: {
                            kernel::FILE_ALL_INFORMATION all;

                            if (payload::ReadValue(payload, all)) {
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

                            if (payload::ReadValue(payload, compression)) {
                                payloadText = RenderCompressionPayload(compression);
                            }

                            break;
                        }

                        case kernel::FileNetworkOpenInformation: {
                            kernel::FILE_NETWORK_OPEN_INFORMATION networkOpen;

                            if (payload::ReadValue(payload, networkOpen)) {
                                payloadText = RenderNetworkOpenPayload(networkOpen);
                            }

                            break;
                        }

                        case kernel::FileAttributeTagInformation: {
                            kernel::FILE_ATTRIBUTE_TAG_INFORMATION attributeTag;

                            if (payload::ReadValue(payload, attributeTag)) {
                                payloadText = RenderAttributeTagPayload(attributeTag);
                            }

                            break;
                        }

                        case kernel::FileHardLinkInformation:
                            payloadText = RenderHardLinksPayload(payload);

                            break;

                        case kernel::FileRemoteProtocolInformation: {
                            kernel::FILE_REMOTE_PROTOCOL_INFORMATION remoteProtocol;

                            if (payload::ReadValue(payload, remoteProtocol)) {
                                payloadText = RenderRemoteProtocolPayload(remoteProtocol);
                            }

                            break;
                        }

                        case kernel::FileIdInformation: {
                            kernel::FILE_ID_INFORMATION id;

                            if (payload::ReadValue(payload, id)) {
                                payloadText = RenderIdPayload(id);
                            }

                            break;
                        }

                        case kernel::FileStatInformation: {
                            kernel::FILE_STAT_INFORMATION stat;

                            if (payload::ReadValue(payload, stat)) {
                                payloadText = RenderStatPayload(stat);
                            }

                            break;
                        }

                        case kernel::FileStatLxInformation: {
                            kernel::FILE_STAT_LX_INFORMATION statLx;

                            if (payload::ReadValue(payload, statLx)) {
                                payloadText = RenderStatLxPayload(statLx);
                            }

                            break;
                        }

                        case kernel::FileCaseSensitiveInformation: {
                            kernel::FILE_CASE_SENSITIVE_INFORMATION caseSensitive;

                            if (payload::ReadValue(payload, caseSensitive)) {
                                payloadText = RenderCaseSensitivePayload(caseSensitive);
                            }

                            break;
                        }

                        case kernel::FileStatBasicInformation: {
                            kernel::FILE_STAT_BASIC_INFORMATION statBasic;

                            if (payload::ReadValue(payload, statBasic)) {
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


                std::wstring RenderSet(const protocol::RecordData& data) {
                    const protocol::FltParameters& parameters = data.parameters;
                    const protocol::SetInfoSupplement& setInfoSupplement = data.supplement.setInfo;
                    std::wstring details = RenderInformationParameters(parameters.setFileInformation.fileInformationClass, parameters.setFileInformation.length);
                    const std::span<const uint8_t> payload = ExtractPayload(setInfoSupplement);
                    std::wstring parametersText;
                    std::wstring payloadText;
                    std::wstring targetText;

                    switch (parameters.setFileInformation.fileInformationClass) {

                        case kernel::FileBasicInformation: {
                            kernel::FILE_BASIC_INFORMATION basic;

                            if (payload::ReadValue(payload, basic)) {
                                payloadText = RenderBasicPayload(basic);
                            }

                            break;
                        }

                        case kernel::FileRenameInformation:
                        case kernel::FileLinkInformation:
                        case kernel::FileRenameInformationBypassAccessCheck:
                        case kernel::FileLinkInformationBypassAccessCheck:
                            parametersText = RenderRenameParameters(parameters);
                            targetText = RenderTargetName(setInfoSupplement);

                            break;

                        case kernel::FileDispositionInformation: {
                            kernel::FILE_DISPOSITION_INFORMATION disposition;

                            if (payload::ReadValue(payload, disposition)) {
                                payloadText = RenderDispositionPayload(disposition);
                            }

                            break;
                        }

                        case kernel::FileAllocationInformation: {
                            kernel::FILE_ALLOCATION_INFORMATION allocation;

                            if (payload::ReadValue(payload, allocation)) {
                                payloadText = RenderAllocationPayload(allocation);
                            }

                            break;
                        }

                        case kernel::FileEndOfFileInformation: {
                            parametersText = RenderEndOfFileParameters(parameters);

                            kernel::FILE_END_OF_FILE_INFORMATION endOfFile;

                            if (payload::ReadValue(payload, endOfFile)) {
                                payloadText = RenderEndOfFilePayload(endOfFile);
                            }

                            break;
                        }

                        case kernel::FileMoveClusterInformation:
                            parametersText = RenderMoveClusterParameters(parameters);

                            break;

                        case kernel::FileValidDataLengthInformation: {
                            kernel::FILE_VALID_DATA_LENGTH_INFORMATION validDataLength;

                            if (payload::ReadValue(payload, validDataLength)) {
                                payloadText = RenderValidDataLengthPayload(validDataLength);
                            }

                            break;
                        }

                        case kernel::FileShortNameInformation: {
                            kernel::FILE_NAME_INFORMATION shortName;

                            if (payload::ReadValue(payload, shortName)) {
                                constexpr ULONG NAME_OFFSET = static_cast<ULONG>(offsetof(kernel::FILE_NAME_INFORMATION, FileName));
                                payloadText = RenderShortNamePayload(shortName, payload.subspan(NAME_OFFSET));
                            }

                            break;
                        }

                        case kernel::FileDispositionInformationEx: {
                            kernel::FILE_DISPOSITION_INFORMATION_EX dispositionEx;

                            if (payload::ReadValue(payload, dispositionEx)) {
                                payloadText = RenderDispositionExPayload(dispositionEx);
                            }

                            break;
                        }

                        case kernel::FileRenameInformationEx:
                        case kernel::FileRenameInformationExBypassAccessCheck:
                        case kernel::FileLinkInformationEx:
                        case kernel::FileLinkInformationExBypassAccessCheck: {
                            kernel::FILE_RENAME_INFORMATION_EX renameEx;

                            if (payload::ReadValue(payload, renameEx)) {
                                payloadText = RenderRenameExPayload(renameEx);
                            }

                            targetText = RenderTargetName(setInfoSupplement);

                            break;
                        }

                        case kernel::FileCaseSensitiveInformation:
                        case kernel::FileCaseSensitiveInformationForceAccessCheck: {
                            kernel::FILE_CASE_SENSITIVE_INFORMATION caseSensitive;

                            if (payload::ReadValue(payload, caseSensitive)) {
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

                    if (!targetText.empty()) {
                        details += L", ";
                        details += targetText;
                    }

                    return details;
                }

            }

        }

    }

}
