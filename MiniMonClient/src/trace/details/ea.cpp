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

    std::wstring RenderEaName(std::span<const uint8_t> nameData, uint8_t nameSize) {
        const std::string_view name{ reinterpret_cast<const char*>(nameData.data()), nameSize < nameData.size() ? nameSize : nameData.size() };

        return text::MarkTruncated(text::ConvertFromAscii(name), name.size() < nameSize);
    }


    std::wstring RenderEaNames(std::span<const uint8_t> eaList) {
        std::wstring result;
        size_t offset = 0u;
        bool terminated = false;

        while (true) {
            constexpr size_t NAME_OFFSET = offsetof(trace::kernel::FILE_GET_EA_INFORMATION, EaName);
            trace::kernel::FILE_GET_EA_INFORMATION entry;

            if (!trace::details::payload::ReadHeader(eaList, entry, NAME_OFFSET, offset)) break;

            result += RenderEaName(eaList.subspan(offset + NAME_OFFSET), entry.EaNameLength);
            result += L'|';

            if (!entry.NextEntryOffset) {
                terminated = true;

                break;
            }

            if (entry.NextEntryOffset > eaList.size()) break;

            offset += entry.NextEntryOffset;
        }

        if (result.empty()) return {};

        if (terminated) {
            result.resize(result.size() - 1u);
        }

        return text::MarkTruncated(result, !terminated);
    }


    std::wstring RenderEaList(const protocol::QueryEaSupplement& supplement) {

        if (!(supplement.captured & protocol::QUERY_EA_CAPTURED_LIST)) return {};

        const std::wstring names = RenderEaNames({ supplement.eaList, supplement.capturedEaListSize });

        if (names.empty()) return {};

        return std::format(L"EaList: {}", names);
    }


    std::span<const uint8_t> ExtractPayload(const protocol::QueryEaSupplement& supplement) {

        if (!(supplement.captured & protocol::QUERY_EA_CAPTURED_PAYLOAD)) return {};

        return { supplement.payload, supplement.capturedPayloadSize };
    }


    std::wstring RenderEaValue(std::span<const uint8_t> valueData, uint16_t valueSize) {
        constexpr size_t PREVIEW_SIZE = 32u;
        const size_t dataSize = valueSize < valueData.size() ? valueSize : valueData.size();
        const size_t copySize = dataSize < PREVIEW_SIZE ? dataSize : PREVIEW_SIZE;
        std::wstring result;

        for (const uint8_t byte : valueData.first(copySize)) {
            result += std::format(L"{:02X}", byte);
        }

        return text::MarkTruncated(result, copySize < valueSize);
    }


    std::wstring RenderExtendedAttributesPayload(std::span<const uint8_t> payload) {
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

            if (entry.EaValueLength) {
                const size_t valueOffset = offset + NAME_OFFSET + entry.EaNameLength + 1u;
                const size_t valueStart = valueOffset < payload.size() ? valueOffset : payload.size();

                result += std::format(L", EaValue: {}", RenderEaValue(payload.subspan(valueStart), entry.EaValueLength));
            }

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

        return { supplement.payload, supplement.capturedSize };
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
                    const std::wstring eaListText = RenderEaList(queryEaSupplement);

                    if (!eaListText.empty()) {
                        details += L", ";
                        details += eaListText;
                    }

                    const std::wstring payloadText = RenderExtendedAttributesPayload(ExtractPayload(queryEaSupplement));

                    if (!payloadText.empty()) {
                        details += L", ";
                        details += payloadText;
                    }

                    return details;
                }


                std::wstring RenderSet(const protocol::RecordData& data) {
                    std::wstring details = std::format(L"Length: {}", data.parameters.setEa.length);

                    const std::wstring payloadText = RenderExtendedAttributesPayload(ExtractPayload(data.supplement.setEa));

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
