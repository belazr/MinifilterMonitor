#include "directory.h"

#include "payload.h"

#include "..\kernel.h"
#include "..\names.h"

#include "..\..\text.h"

#include "..\..\..\..\inc\protocol.h"

#include <Windows.h>

#include <cstddef>
#include <format>
#include <span>
#include <string>
#include <string_view>

using namespace mimo;

namespace {

    std::wstring RenderFileName(const protocol::QueryDirectorySupplement& supplement) {

        if (!(supplement.captured & protocol::QUERY_DIRECTORY_CAPTURED_FILE_NAME)) return {};

        const std::wstring_view fileName = text::Extract(supplement.fileName);

        if (fileName.empty()) return {};

        return std::format(L"Filter: {}", text::MarkTruncated(fileName, supplement.captured & protocol::QUERY_DIRECTORY_TRUNCATED_FILE_NAME));
    }


    std::span<const uint8_t> ExtractPayload(const protocol::QueryDirectorySupplement& supplement) {

        if (!(supplement.captured & protocol::QUERY_DIRECTORY_CAPTURED_PAYLOAD)) return {};

        return { supplement.payload, supplement.capturedBytes };
    }


    template <typename Entry>
    std::wstring RenderEntriesPayload(std::span<const uint8_t> payload) {
        std::wstring result;
        ULONG offset = 0u;
        ULONG index = 1u;

        while (true) {
            constexpr ULONG NAME_OFFSET = static_cast<ULONG>(offsetof(Entry, FileName));
            Entry entry;

            if (!trace::details::payload::ReadHeader(payload, entry, NAME_OFFSET, offset)) break;

            result += std::format(L"{}: {}, ", index, trace::details::payload::RenderName(payload.subspan(offset + NAME_OFFSET), entry.FileNameLength));
            index++;

            if (!entry.NextEntryOffset || entry.NextEntryOffset > payload.size()) break;

            offset += entry.NextEntryOffset;
        }

        if (!result.empty()) {
            result.resize(result.size() - 2u);
        }

        return result;
    }


    std::wstring RenderQueryDirectory(const protocol::RecordData& data) {
        const protocol::FltParameters& parameters = data.parameters;
        const protocol::QueryDirectorySupplement& queryDirectorySupplement = data.supplement.queryDirectory;
        std::wstring details = std::format(L"Class: {}, Length: {}", trace::names::RenderFileInformationClass(parameters.queryDirectory.fileInformationClass), parameters.queryDirectory.length);

        if ((data.operationFlags & trace::kernel::SL_INDEX_SPECIFIED) || parameters.queryDirectory.fileIndex) {
            details += std::format(L", FileIndex: {}", parameters.queryDirectory.fileIndex);
        }

        const std::wstring flags = trace::names::RenderQueryDirectoryFlags(data.operationFlags);

        if (!flags.empty()) {
            details += L", ";
            details += flags;
        }

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

        }

        if (!payloadText.empty()) {
            details += L", ";
            details += payloadText;
        }

        return details;
    }


    std::wstring RenderNotifyDirectory(const protocol::RecordData& data) {
        const protocol::FltParameters& parameters = data.parameters;
        std::wstring details;

        const std::wstring completionFilter = trace::names::RenderCompletionFilter(parameters.notifyDirectory.completionFilter);

        if (!completionFilter.empty()) {
            details += std::format(L"Filter: {}, ", completionFilter);
        }

        if (data.callbackMinorId == trace::kernel::IRP_MN_NOTIFY_CHANGE_DIRECTORY_EX) {
            details += std::format(L"Class: {}, ", trace::names::RenderDirectoryNotifyInformationClass(parameters.notifyDirectory.directoryNotifyInformationClass));
        }

        details += std::format(L"Length: {}", parameters.notifyDirectory.length);

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

                            return RenderQueryDirectory(data);

                        case kernel::IRP_MN_NOTIFY_CHANGE_DIRECTORY:
                        case kernel::IRP_MN_NOTIFY_CHANGE_DIRECTORY_EX:

                            return RenderNotifyDirectory(data);

                    }

                    return {};
                }

            }

        }

    }

}
