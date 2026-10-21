#include "quota.h"

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

using namespace mimo;

namespace {

    std::wstring RenderQueryParameters(uint32_t size, uint32_t sidListSize) {

        return std::format(L"Length: {}, SidListLength: {}", size, sidListSize);
    }


    std::wstring RenderSidsPayload(std::span<const uint8_t> payload) {

        if (payload.empty()) return {};

        std::wstring result;
        size_t offset = 0u;
        bool terminated = false;

        while (true) {
            constexpr size_t SID_OFFSET = offsetof(trace::kernel::FILE_GET_QUOTA_INFORMATION, Sid);
            trace::kernel::FILE_GET_QUOTA_INFORMATION entry{};

            if (!trace::payload::ReadHeader(payload, entry, SID_OFFSET, offset)) break;

            const size_t sidStart = offset + SID_OFFSET;

            if (entry.SidLength > payload.size() - sidStart) break;

            result += trace::values::RenderSid(payload.subspan(sidStart, entry.SidLength));
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


    std::wstring RenderSidList(const protocol::QueryQuotaSupplement& supplement) {

        if (!(supplement.captured & protocol::QUERY_QUOTA_CAPTURED_SID_LIST)) return {};

        const std::wstring sidListText = RenderSidsPayload({ supplement.sidList, supplement.capturedSidListSize });

        if (sidListText.empty()) return {};

        return std::format(L"SidList: {}", sidListText);
    }


    std::span<const uint8_t> ExtractPayload(const protocol::QueryQuotaSupplement& supplement) {

        if (!(supplement.captured & protocol::QUERY_QUOTA_CAPTURED_PAYLOAD)) return {};

        return { supplement.payload, supplement.capturedPayloadSize };
    }


    std::wstring RenderQuotasPayload(std::span<const uint8_t> payload) {

        if (payload.empty()) return {};

        std::wstring result;
        size_t offset = 0u;
        uint32_t index = 1u;
        bool terminated = false;

        while (true) {
            constexpr size_t SID_OFFSET = offsetof(trace::kernel::FILE_QUOTA_INFORMATION, Sid);
            trace::kernel::FILE_QUOTA_INFORMATION entry{};

            if (!trace::payload::ReadHeader(payload, entry, SID_OFFSET, offset)) break;

            const size_t sidStart = offset + SID_OFFSET;

            if (entry.SidLength > payload.size() - sidStart) break;

            result += std::format(L"{}: Sid: {}", index, trace::values::RenderSid(payload.subspan(sidStart, entry.SidLength)));
            index++;

            if (entry.ChangeTime) {
                result += std::format(L", ChangeTime: {}", trace::values::RenderTime(entry.ChangeTime));
            }

            result += std::format(L", QuotaUsed: {}, QuotaThreshold: {}, QuotaLimit: {}, ", entry.QuotaUsed, entry.QuotaThreshold, entry.QuotaLimit);

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


    std::wstring RenderSetParameters(uint32_t size) {

        return std::format(L"Length: {}", size);
    }


    std::span<const uint8_t> ExtractPayload(const protocol::SetQuotaSupplement& supplement) {

        if (!(supplement.captured & protocol::SET_QUOTA_CAPTURED_PAYLOAD)) return {};

        return { supplement.payload, supplement.capturedSize };
    }

}

namespace mimo {

    namespace trace {

        namespace details {

            namespace quota {

                std::wstring RenderQuery(const protocol::RecordData& data) {
                    const protocol::FltParameters& parameters = data.parameters;
                    std::wstring details = RenderQueryParameters(parameters.queryQuota.length, parameters.queryQuota.sidListLength);

                    if (data.operationFlags) {
                        details += L", ";
                        details += names::RenderScanFlags(data.operationFlags);
                    }

                    const protocol::QueryQuotaSupplement& queryQuotaSupplement = data.supplement.queryQuota;
                    const std::wstring sidListText = RenderSidList(queryQuotaSupplement);

                    if (!sidListText.empty()) {
                        details += L", ";
                        details += sidListText;
                    }

                    const std::wstring payloadText = RenderQuotasPayload(ExtractPayload(queryQuotaSupplement));

                    if (!payloadText.empty()) {
                        details += L", ";
                        details += payloadText;
                    }

                    return details;
                }


                std::wstring RenderSet(const protocol::RecordData& data) {
                    std::wstring details = RenderSetParameters(data.parameters.setQuota.length);

                    const std::wstring payloadText = RenderQuotasPayload(ExtractPayload(data.supplement.setQuota));

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
