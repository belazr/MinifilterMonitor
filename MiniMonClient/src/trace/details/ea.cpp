#include "ea.h"

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

    std::wstring RenderEaName(std::span<const uint8_t> nameData, uint8_t nameLength) {
        const std::string_view name{ reinterpret_cast<const char*>(nameData.data()), nameLength < nameData.size() ? nameLength : nameData.size() };

        return text::MarkTruncated(text::ConvertFromAscii(name), name.size() < nameLength);
    }


    std::wstring RenderNamesList(std::span<const uint8_t> list) {
        std::wstring result;
        size_t offset = 0u;
        bool terminated = false;

        while (true) {
            constexpr size_t NAME_OFFSET = offsetof(trace::kernel::FILE_GET_EA_INFORMATION, EaName);
            trace::kernel::FILE_GET_EA_INFORMATION entry;

            if (!trace::details::payload::ReadHeader(list, entry, NAME_OFFSET, offset)) break;

            result += RenderEaName(list.subspan(offset + NAME_OFFSET), entry.EaNameLength);
            result += L'|';

            if (!entry.NextEntryOffset) {
                terminated = true;

                break;
            }

            if (entry.NextEntryOffset > list.size()) break;

            offset += entry.NextEntryOffset;
        }

        if (result.empty()) return {};

        if (terminated) {
            result.resize(result.size() - 1u);
        }

        return text::MarkTruncated(result, !terminated);
    }


    std::wstring RenderList(const protocol::QueryEaSupplement& supplement) {

        if (!(supplement.captured & protocol::QUERY_EA_CAPTURED_LIST)) return {};

        const std::wstring names = RenderNamesList({ supplement.list, supplement.capturedListBytes });

        if (names.empty()) return {};

        return std::format(L"EaList: {}", names);
    }


    std::span<const uint8_t> ExtractPayload(const protocol::QueryEaSupplement& supplement) {

        if (!(supplement.captured & protocol::QUERY_EA_CAPTURED_PAYLOAD)) return {};

        return { supplement.payload, supplement.capturedPayloadBytes };
    }


    std::wstring RenderEntriesPayload(std::span<const uint8_t> payload) {
        std::wstring result;
        size_t offset = 0u;
        uint32_t index = 1u;
        bool terminated = false;

        while (true) {
            constexpr size_t NAME_OFFSET = offsetof(trace::kernel::FILE_FULL_EA_INFORMATION, EaName);
            trace::kernel::FILE_FULL_EA_INFORMATION entry;

            if (!trace::details::payload::ReadHeader(payload, entry, NAME_OFFSET, offset)) break;

            result += std::format(L"{}: EaName: {}, EaValueLength: {}", index, RenderEaName(payload.subspan(offset + NAME_OFFSET), entry.EaNameLength), entry.EaValueLength);
            index++;

            const std::wstring flags = trace::names::RenderEaFlags(entry.Flags);

            if (!flags.empty()) {
                result += std::format(L", Flags: {}", flags);
            }

            result += L", ";

            if (!entry.NextEntryOffset) {
                terminated = true;

                break;
            }

            if (entry.NextEntryOffset > payload.size()) break;

            offset += entry.NextEntryOffset;
        }

        if (result.empty()) return {};

        if (terminated) {
            result.resize(result.size() - 2u);
        }

        return text::MarkTruncated(result, !terminated);
    }


    std::span<const uint8_t> ExtractPayload(const protocol::SetEaSupplement& supplement) {

        if (!(supplement.captured & protocol::SET_EA_CAPTURED_PAYLOAD)) return {};

        return { supplement.payload, supplement.capturedBytes };
    }

}

namespace mimo {

    namespace trace {

        namespace details {

            namespace ea {

                std::wstring RenderQuery(const protocol::RecordData& data) {
                    const protocol::FltParameters& parameters = data.parameters;
                    std::wstring details = std::format(L"Length: {}, EaListLength: {}", parameters.queryEa.length, parameters.queryEa.eaListLength);

                    if ((data.operationFlags & kernel::SL_INDEX_SPECIFIED) || parameters.queryEa.eaIndex) {
                        details += std::format(L", EaIndex: {}", parameters.queryEa.eaIndex);
                    }

                    const std::wstring flags = names::RenderScanFlags(data.operationFlags);

                    if (!flags.empty()) {
                        details += L", ";
                        details += flags;
                    }

                    const protocol::QueryEaSupplement& queryEaSupplement = data.supplement.queryEa;
                    const std::wstring listText = RenderList(queryEaSupplement);

                    if (!listText.empty()) {
                        details += L", ";
                        details += listText;
                    }

                    const std::wstring payloadText = RenderEntriesPayload(ExtractPayload(queryEaSupplement));

                    if (!payloadText.empty()) {
                        details += L", ";
                        details += payloadText;
                    }

                    return details;
                }


                std::wstring RenderSet(const protocol::RecordData& data) {
                    std::wstring details = std::format(L"Length: {}", data.parameters.setEa.length);

                    const protocol::SetEaSupplement& setEaSupplement = data.supplement.setEa;
                    const std::wstring payloadText = RenderEntriesPayload(ExtractPayload(setEaSupplement));

                    if (!payloadText.empty()) {
                        details += L", ";
                        details += payloadText;
                    }

                    return details;
                }

            }

        }

    }

}
