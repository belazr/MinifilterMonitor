#include "directory.h"

#include "payload.h"

#include "..\kernel.h"
#include "..\names.h"

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

    std::wstring RenderQueryParameters(uint32_t fileInformationClass, uint32_t length) {

        return std::format(L"Class: {}, Length: {}", trace::names::RenderFileInformationClass(fileInformationClass), length);
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
            Entry entry;

            if (!trace::details::payload::ReadHeader(payload, entry, NAME_OFFSET, offset)) break;

            result += std::format(L"{}: {}, ", index, trace::details::payload::RenderName(payload.subspan(offset + NAME_OFFSET), entry.FileNameLength));
            index++;

            if (!entry.NextEntryOffset) {
                terminated = true;

                break;
            }

            if (entry.NextEntryOffset > payload.size()) break;

            offset += entry.NextEntryOffset;
        }

        if (terminated) {
            result.resize(result.size() - 2u);
        }

        return text::MarkTruncated(result, !terminated);
    }


    std::wstring RenderQuery(const protocol::RecordData& data) {
        const protocol::FltParameters& parameters = data.parameters;
        std::wstring details = RenderQueryParameters(parameters.queryDirectory.fileInformationClass, parameters.queryDirectory.length);

        if ((data.operationFlags & trace::kernel::SL_INDEX_SPECIFIED) || parameters.queryDirectory.fileIndex) {
            details += std::format(L", FileIndex: {}", parameters.queryDirectory.fileIndex);
        }

        const std::wstring flags = trace::names::RenderScanFlags(data.operationFlags);

        if (!flags.empty()) {
            details += L", ";
            details += flags;
        }

        const protocol::QueryDirectorySupplement& queryDirectorySupplement = data.supplement.queryDirectory;
        const std::wstring fileNameText = RenderFileName(queryDirectorySupplement);

        if (!fileNameText.empty()) {
            details += L", ";
            details += fileNameText;
        }

        const std::span<const uint8_t> payload = ExtractPayload(queryDirectorySupplement);
        std::wstring payloadText;

        switch (parameters.queryDirectory.fileInformationClass) {

            case trace::kernel::FileDirectoryInformation:
                payloadText = RenderEntriesPayload<trace::kernel::FILE_DIRECTORY_INFORMATION>(payload);

                break;

            case trace::kernel::FileFullDirectoryInformation:
                payloadText = RenderEntriesPayload<trace::kernel::FILE_FULL_DIR_INFORMATION>(payload);

                break;

            case trace::kernel::FileBothDirectoryInformation:
                payloadText = RenderEntriesPayload<trace::kernel::FILE_BOTH_DIR_INFORMATION>(payload);

                break;

            case trace::kernel::FileNamesInformation:
                payloadText = RenderEntriesPayload<trace::kernel::FILE_NAMES_INFORMATION>(payload);

                break;

            case trace::kernel::FileIdBothDirectoryInformation:
                payloadText = RenderEntriesPayload<trace::kernel::FILE_ID_BOTH_DIR_INFORMATION>(payload);

                break;

            case trace::kernel::FileIdFullDirectoryInformation:
                payloadText = RenderEntriesPayload<trace::kernel::FILE_ID_FULL_DIR_INFORMATION>(payload);

                break;

            case trace::kernel::FileIdGlobalTxDirectoryInformation:
                payloadText = RenderEntriesPayload<trace::kernel::FILE_ID_GLOBAL_TX_DIR_INFORMATION>(payload);

                break;

            case trace::kernel::FileIdExtdDirectoryInformation:
                payloadText = RenderEntriesPayload<trace::kernel::FILE_ID_EXTD_DIR_INFORMATION>(payload);

                break;

            case trace::kernel::FileIdExtdBothDirectoryInformation:
                payloadText = RenderEntriesPayload<trace::kernel::FILE_ID_EXTD_BOTH_DIR_INFORMATION>(payload);

                break;

            case trace::kernel::FileId64ExtdDirectoryInformation:
                payloadText = RenderEntriesPayload<trace::kernel::FILE_ID_64_EXTD_DIR_INFORMATION>(payload);

                break;

            case trace::kernel::FileId64ExtdBothDirectoryInformation:
                payloadText = RenderEntriesPayload<trace::kernel::FILE_ID_64_EXTD_BOTH_DIR_INFORMATION>(payload);

                break;

            case trace::kernel::FileIdAllExtdDirectoryInformation:
                payloadText = RenderEntriesPayload<trace::kernel::FILE_ID_ALL_EXTD_DIR_INFORMATION>(payload);

                break;

            case trace::kernel::FileIdAllExtdBothDirectoryInformation:
                payloadText = RenderEntriesPayload<trace::kernel::FILE_ID_ALL_EXTD_BOTH_DIR_INFORMATION>(payload);

                break;

        }

        if (!payloadText.empty()) {
            details += L", ";
            details += payloadText;
        }

        return details;
    }


    std::wstring RenderNotifyParameters(uint32_t length) {

        return std::format(L"Length: {}", length);
    }


    std::wstring RenderNotifyExParameters(uint32_t directoryNotifyInformationClass, uint32_t length) {

        return std::format(L"Class: {}, Length: {}", trace::names::RenderDirectoryNotifyInformationClass(directoryNotifyInformationClass), length);
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
            Entry entry;

            if (!trace::details::payload::ReadHeader(payload, entry, NAME_OFFSET, offset)) break;

            result += std::format(L"{}: Action: {}, FileName: {}, ", index, trace::names::RenderFileAction(entry.Action), trace::details::payload::RenderName(payload.subspan(offset + NAME_OFFSET), entry.FileNameLength));
            index++;

            if (!entry.NextEntryOffset) {
                terminated = true;

                break;
            }

            if (entry.NextEntryOffset > payload.size()) break;

            offset += entry.NextEntryOffset;
        }

        if (terminated) {
            result.resize(result.size() - 2u);
        }

        return text::MarkTruncated(result, !terminated);
    }


    std::wstring RenderNotify(const protocol::RecordData& data) {
        const protocol::FltParameters& parameters = data.parameters;
        std::wstring details;

        if (data.callbackMinorId == trace::kernel::IRP_MN_NOTIFY_CHANGE_DIRECTORY) {
            details = RenderNotifyParameters(parameters.notifyDirectory.length);
        }
        else {
            details = RenderNotifyExParameters(parameters.notifyDirectory.directoryNotifyInformationClass, parameters.notifyDirectory.length);
        }

        const std::wstring flags = trace::names::RenderNotifyFlags(data.operationFlags);

        if (!flags.empty()) {
            details += L", ";
            details += flags;
        }

        const std::wstring completionFilter = trace::names::RenderCompletionFilter(parameters.notifyDirectory.completionFilter);

        if (!completionFilter.empty()) {
            details += std::format(L", Filter: {}", completionFilter);
        }

        const std::span<const uint8_t> payload = ExtractPayload(data.supplement.notifyDirectory);
        const uint32_t directoryNotifyInformationClass = data.callbackMinorId == trace::kernel::IRP_MN_NOTIFY_CHANGE_DIRECTORY ? trace::kernel::DirectoryNotifyInformation : parameters.notifyDirectory.directoryNotifyInformationClass;
        std::wstring payloadText;

        switch (directoryNotifyInformationClass) {

            case trace::kernel::DirectoryNotifyInformation:
                payloadText = RenderChangesPayload<trace::kernel::FILE_NOTIFY_INFORMATION>(payload);

                break;

            case trace::kernel::DirectoryNotifyExtendedInformation:
                payloadText = RenderChangesPayload<trace::kernel::FILE_NOTIFY_EXTENDED_INFORMATION>(payload);

                break;

            case trace::kernel::DirectoryNotifyFullInformation:
                payloadText = RenderChangesPayload<trace::kernel::FILE_NOTIFY_FULL_INFORMATION>(payload);

                break;

        }

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
