#include "directory.h"

#include "..\kernel.h"
#include "..\names.h"
#include "..\payload.h"
#include "..\values.h"

#include "..\..\text.h"

#include "..\..\..\..\inc\protocol.h"

#include <cstddef>
#include <cstdint>
#include <format>
#include <span>
#include <string>
#include <string_view>

using namespace mimo;

namespace {

    std::wstring RenderQueryParameters(uint32_t infoClass, uint32_t size) {

        return std::format(L"Class: {}, Length: {}", trace::names::RenderFileInfoClass(infoClass), size);
    }


    std::wstring RenderFileName(const protocol::QueryDirectorySupplement& supplement) {

        if (!(supplement.captured & protocol::QUERY_DIRECTORY_CAPTURED_FILE_NAME)) return {};

        const std::wstring_view fileName = text::Extract(supplement.fileName);
        const bool truncated = supplement.captured & protocol::QUERY_DIRECTORY_TRUNCATED_FILE_NAME;

        if (fileName.empty() && !truncated) return {};

        return std::format(L"Filter: {}", text::MarkTruncated(fileName, truncated));
    }


    std::span<const uint8_t> ExtractPayload(const protocol::QueryDirectorySupplement& supplement) {

        if (!(supplement.captured & protocol::QUERY_DIRECTORY_CAPTURED_PAYLOAD)) return {};

        return { supplement.payload, supplement.capturedSize };
    }


    template <typename Entry>
    std::wstring RenderEntriesPayload(std::span<const uint8_t> payload) {

        if (payload.empty()) return {};

        std::wstring result;
        size_t offset = 0u;
        uint32_t index = 1u;
        bool terminated = false;

        while (true) {
            constexpr size_t NAME_OFFSET = offsetof(Entry, FileName);
            Entry entry{};

            if (!trace::payload::ReadHeader(payload, entry, NAME_OFFSET, offset)) break;

            result += std::format(L"{}: {}, ", index, trace::payload::RenderName(payload.subspan(offset + NAME_OFFSET), entry.FileNameLength));
            index++;

            if (!entry.NextEntryOffset) {
                terminated = true;

                break;
            }

            if (!trace::payload::Advance(payload, offset, entry.NextEntryOffset)) break;

        }

        if (terminated) {
            result.resize(result.size() - 2u);
        }

        return text::MarkTruncated(result, !terminated);
    }


    std::wstring RenderObjectIdsPayload(std::span<const uint8_t> payload, bool truncated) {

        if (payload.empty()) return {};

        std::wstring result;
        size_t offset = 0u;
        uint32_t index = 1u;
        trace::kernel::FILE_OBJECTID_INFORMATION objectId{};

        while (trace::payload::ReadValue(payload, objectId, offset)) {
            result += std::format(L"{}: FileReference: {}, ObjectId: {}, ", index, trace::values::RenderFileId(static_cast<uint64_t>(objectId.FileReference)), trace::values::RenderGuid(objectId.ObjectId));
            offset += sizeof(objectId);
            index++;
        }

        const bool marked = truncated || result.empty();

        if (!marked) {
            result.resize(result.size() - 2u);
        }

        return text::MarkTruncated(result, marked);
    }


    std::wstring RenderReparsePointsPayload(std::span<const uint8_t> payload, bool truncated) {

        if (payload.empty()) return {};

        std::wstring result;
        size_t offset = 0u;
        uint32_t index = 1u;
        trace::kernel::FILE_REPARSE_POINT_INFORMATION reparsePoint{};

        while (trace::payload::ReadValue(payload, reparsePoint, offset)) {
            result += std::format(L"{}: FileReference: {}, Tag: {}, ", index, trace::values::RenderFileId(static_cast<uint64_t>(reparsePoint.FileReference)), trace::names::RenderReparseTag(reparsePoint.Tag));
            offset += sizeof(reparsePoint);
            index++;
        }

        const bool marked = truncated || result.empty();

        if (!marked) {
            result.resize(result.size() - 2u);
        }

        return text::MarkTruncated(result, marked);
    }


    std::wstring RenderPayload(uint32_t infoClass, const protocol::QueryDirectorySupplement& supplement) {
        const std::span<const uint8_t> payload = ExtractPayload(supplement);
        const bool truncated = supplement.captured & protocol::QUERY_DIRECTORY_TRUNCATED_PAYLOAD;

        switch (infoClass) {

            case trace::kernel::FileDirectoryInformation:

                return RenderEntriesPayload<trace::kernel::FILE_DIRECTORY_INFORMATION>(payload);

            case trace::kernel::FileFullDirectoryInformation:

                return RenderEntriesPayload<trace::kernel::FILE_FULL_DIR_INFORMATION>(payload);

            case trace::kernel::FileBothDirectoryInformation:

                return RenderEntriesPayload<trace::kernel::FILE_BOTH_DIR_INFORMATION>(payload);

            case trace::kernel::FileNamesInformation:

                return RenderEntriesPayload<trace::kernel::FILE_NAMES_INFORMATION>(payload);

            case trace::kernel::FileObjectIdInformation:

                return RenderObjectIdsPayload(payload, truncated);

            case trace::kernel::FileReparsePointInformation:

                return RenderReparsePointsPayload(payload, truncated);

            case trace::kernel::FileIdBothDirectoryInformation:

                return RenderEntriesPayload<trace::kernel::FILE_ID_BOTH_DIR_INFORMATION>(payload);

            case trace::kernel::FileIdFullDirectoryInformation:

                return RenderEntriesPayload<trace::kernel::FILE_ID_FULL_DIR_INFORMATION>(payload);

            case trace::kernel::FileIdGlobalTxDirectoryInformation:

                return RenderEntriesPayload<trace::kernel::FILE_ID_GLOBAL_TX_DIR_INFORMATION>(payload);

            case trace::kernel::FileIdExtdDirectoryInformation:

                return RenderEntriesPayload<trace::kernel::FILE_ID_EXTD_DIR_INFORMATION>(payload);

            case trace::kernel::FileIdExtdBothDirectoryInformation:

                return RenderEntriesPayload<trace::kernel::FILE_ID_EXTD_BOTH_DIR_INFORMATION>(payload);

            case trace::kernel::FileId64ExtdDirectoryInformation:

                return RenderEntriesPayload<trace::kernel::FILE_ID_64_EXTD_DIR_INFORMATION>(payload);

            case trace::kernel::FileId64ExtdBothDirectoryInformation:

                return RenderEntriesPayload<trace::kernel::FILE_ID_64_EXTD_BOTH_DIR_INFORMATION>(payload);

            case trace::kernel::FileIdAllExtdDirectoryInformation:

                return RenderEntriesPayload<trace::kernel::FILE_ID_ALL_EXTD_DIR_INFORMATION>(payload);

            case trace::kernel::FileIdAllExtdBothDirectoryInformation:

                return RenderEntriesPayload<trace::kernel::FILE_ID_ALL_EXTD_BOTH_DIR_INFORMATION>(payload);

        }

        return {};
    }


    std::wstring RenderQuery(const protocol::RecordData& data) {
        const protocol::FltParameters& parameters = data.parameters;
        std::wstring details = RenderQueryParameters(parameters.queryDirectory.fileInformationClass, parameters.queryDirectory.length);

        if ((data.operationFlags & trace::kernel::SL_INDEX_SPECIFIED) || parameters.queryDirectory.fileIndex) {
            details += std::format(L", FileIndex: {}", parameters.queryDirectory.fileIndex);
        }

        if (data.operationFlags) {
            details += L", ";
            details += trace::names::RenderScanFlags(data.operationFlags);
        }

        const protocol::QueryDirectorySupplement& queryDirectorySupplement = data.supplement.queryDirectory;
        const std::wstring fileNameText = RenderFileName(queryDirectorySupplement);

        if (!fileNameText.empty()) {
            details += L", ";
            details += fileNameText;
        }

        const std::wstring payloadText = RenderPayload(parameters.queryDirectory.fileInformationClass, queryDirectorySupplement);

        if (!payloadText.empty()) {
            details += L", ";
            details += payloadText;
        }

        return details;
    }


    std::wstring RenderNotifyExParameters(uint32_t infoClass, uint32_t size) {

        return std::format(L"Class: {}, Length: {}", trace::names::RenderDirectoryNotifyInfoClass(infoClass), size);
    }


    std::wstring RenderNotifyParameters(uint32_t size) {

        return std::format(L"Length: {}", size);
    }


    std::span<const uint8_t> ExtractPayload(const protocol::NotifyDirectorySupplement& supplement) {

        if (!(supplement.captured & protocol::NOTIFY_DIRECTORY_CAPTURED_PAYLOAD)) return {};

        return { supplement.payload, supplement.capturedSize };
    }


    template <typename Entry>
    std::wstring RenderChangesPayload(std::span<const uint8_t> payload) {

        if (payload.empty()) return {};

        std::wstring result;
        size_t offset = 0u;
        uint32_t index = 1u;
        bool terminated = false;

        while (true) {
            constexpr size_t NAME_OFFSET = offsetof(Entry, FileName);
            Entry entry{};

            if (!trace::payload::ReadHeader(payload, entry, NAME_OFFSET, offset)) break;

            result += std::format(L"{}: Action: {}, FileName: {}, ", index, trace::names::RenderFileAction(entry.Action), trace::payload::RenderName(payload.subspan(offset + NAME_OFFSET), entry.FileNameLength));
            index++;

            if (!entry.NextEntryOffset) {
                terminated = true;

                break;
            }

            if (!trace::payload::Advance(payload, offset, entry.NextEntryOffset)) break;

        }

        if (terminated) {
            result.resize(result.size() - 2u);
        }

        return text::MarkTruncated(result, !terminated);
    }


    std::wstring RenderPayload(uint32_t infoClass, const protocol::NotifyDirectorySupplement& supplement) {
        const std::span<const uint8_t> payload = ExtractPayload(supplement);

        switch (infoClass) {

            case trace::kernel::DirectoryNotifyInformation:

                return RenderChangesPayload<trace::kernel::FILE_NOTIFY_INFORMATION>(payload);

            case trace::kernel::DirectoryNotifyExtendedInformation:

                return RenderChangesPayload<trace::kernel::FILE_NOTIFY_EXTENDED_INFORMATION>(payload);

            case trace::kernel::DirectoryNotifyFullInformation:

                return RenderChangesPayload<trace::kernel::FILE_NOTIFY_FULL_INFORMATION>(payload);

        }

        return {};
    }


    std::wstring RenderNotify(const protocol::RecordData& data) {
        const protocol::FltParameters& parameters = data.parameters;
        const bool notifyEx = data.callbackMinorId == trace::kernel::IRP_MN_NOTIFY_CHANGE_DIRECTORY_EX;
        std::wstring details = notifyEx ? RenderNotifyExParameters(parameters.notifyDirectory.directoryNotifyInformationClass, parameters.notifyDirectory.length) : RenderNotifyParameters(parameters.notifyDirectory.length);

        if (data.operationFlags) {
            details += L", ";
            details += trace::names::RenderNotifyFlags(data.operationFlags);
        }

        if (parameters.notifyDirectory.completionFilter) {
            details += std::format(L", Filter: {}", trace::names::RenderCompletionFilter(parameters.notifyDirectory.completionFilter));
        }

        const uint32_t directoryNotifyInformationClass = notifyEx ? parameters.notifyDirectory.directoryNotifyInformationClass : trace::kernel::DirectoryNotifyInformation;
        const std::wstring payloadText = RenderPayload(directoryNotifyInformationClass, data.supplement.notifyDirectory);

        if (!payloadText.empty()) {
            details += L", ";
            details += payloadText;
        }

        return details;
    }

}

namespace mimo {

    namespace trace {

        namespace details {

            namespace directory {

                std::wstring Render(const protocol::RecordData& data) {

                    switch (data.callbackMinorId) {

                        case kernel::IRP_MN_QUERY_DIRECTORY:

                            return RenderQuery(data);

                        case kernel::IRP_MN_NOTIFY_CHANGE_DIRECTORY:
                        case kernel::IRP_MN_NOTIFY_CHANGE_DIRECTORY_EX:

                            return RenderNotify(data);

                    }

                    return {};
                }

            }

        }

    }

}
