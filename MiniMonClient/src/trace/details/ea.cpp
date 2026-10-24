#include "ea.h"

#include "..\eas.h"
#include "..\kernel.h"
#include "..\names.h"
#include "..\payload.h"

#include "..\..\text.h"

#include "..\..\..\..\inc\protocol.h"

#include <cstddef>
#include <cstdint>
#include <format>
#include <span>
#include <string>

using namespace mimo;

namespace {

    std::wstring RenderQueryParameters(uint32_t size, uint32_t eaListSize) {

        return std::format(L"Length: {}, EaListLength: {}", size, eaListSize);
    }


    std::wstring RenderEaNamesPayload(std::span<const uint8_t> payload) {

        if (payload.empty()) return {};

        std::wstring result;
        size_t offset = 0u;
        bool terminated = false;

        while (true) {
            constexpr size_t NAME_OFFSET = offsetof(trace::kernel::FILE_GET_EA_INFORMATION, EaName);
            trace::kernel::FILE_GET_EA_INFORMATION entry{};

            if (!trace::payload::ReadHeader(payload, entry, NAME_OFFSET, offset)) break;

            result += trace::payload::RenderAsciiName(payload.subspan(offset + NAME_OFFSET), entry.EaNameLength);
            result += L'|';

            if (!entry.NextEntryOffset) {
                terminated = true;

                break;
            }

            if (!trace::payload::Advance(payload, offset, entry.NextEntryOffset)) break;

        }

        if (terminated) {
            result.resize(result.size() - 1u);
        }

        return text::MarkTruncated(result, !terminated);
    }


    std::wstring RenderEaList(const protocol::QueryEaSupplement& supplement) {

        if (!(supplement.captured & protocol::QUERY_EA_CAPTURED_LIST)) return {};

        const std::wstring eaListText = RenderEaNamesPayload({ supplement.eaList, supplement.capturedEaListSize });

        if (eaListText.empty()) return {};

        return std::format(L"EaList: {}", eaListText);
    }


    std::span<const uint8_t> ExtractPayload(const protocol::QueryEaSupplement& supplement) {

        if (!(supplement.captured & protocol::QUERY_EA_CAPTURED_PAYLOAD)) return {};

        return { supplement.payload, supplement.capturedPayloadSize };
    }


    std::wstring RenderSetParameters(uint32_t size) {

        return std::format(L"Length: {}", size);
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
                    std::wstring details = RenderQueryParameters(parameters.queryEa.length, parameters.queryEa.eaListLength);

                    if ((data.operationFlags & kernel::SL_INDEX_SPECIFIED) || parameters.queryEa.eaIndex) {
                        details += std::format(L", EaIndex: {}", parameters.queryEa.eaIndex);
                    }

                    if (data.operationFlags) {
                        details += L", ";
                        details += names::RenderQueryFlags(data.operationFlags);
                    }

                    const protocol::QueryEaSupplement& queryEaSupplement = data.supplement.queryEa;
                    const std::wstring eaListText = RenderEaList(queryEaSupplement);

                    if (!eaListText.empty()) {
                        details += L", ";
                        details += eaListText;
                    }

                    const std::wstring payloadText = trace::eas::RenderEasPayload(ExtractPayload(queryEaSupplement));

                    if (!payloadText.empty()) {
                        details += L", ";
                        details += payloadText;
                    }

                    return details;
                }


                std::wstring RenderSet(const protocol::RecordData& data) {
                    std::wstring details = RenderSetParameters(data.parameters.setEa.length);

                    const std::wstring payloadText = trace::eas::RenderEasPayload(ExtractPayload(data.supplement.setEa));

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
