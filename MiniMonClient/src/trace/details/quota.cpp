#include "quota.h"

#include "payload.h"

#include "..\kernel.h"
#include "..\names.h"
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

    std::wstring RenderSids(std::span<const uint8_t> sidList) {
        std::wstring result;
        size_t offset = 0u;
        bool terminated = false;

        while (true) {
            constexpr size_t SID_OFFSET = offsetof(trace::kernel::FILE_GET_QUOTA_INFORMATION, Sid);
            trace::kernel::FILE_GET_QUOTA_INFORMATION entry;

            if (!trace::details::payload::ReadHeader(sidList, entry, SID_OFFSET, offset)) break;

            const size_t sidStart = offset + SID_OFFSET;

            if (entry.SidLength > sidList.size() - sidStart) break;

            result += trace::values::RenderSid(sidList.subspan(sidStart, entry.SidLength));
            result += L'|';

            if (!entry.NextEntryOffset) {
                terminated = true;

                break;
            }

            if (entry.NextEntryOffset > sidList.size()) break;

            offset += entry.NextEntryOffset;
        }

        if (result.empty()) return {};

        if (terminated) {
            result.resize(result.size() - 1u);
        }

        return text::MarkTruncated(result, !terminated);
    }


    std::wstring RenderSidList(const protocol::QueryQuotaSupplement& supplement) {

        if (!(supplement.captured & protocol::QUERY_QUOTA_CAPTURED_SID_LIST)) return {};

        const std::wstring sids = RenderSids({ supplement.sidList, supplement.capturedSidListSize });

        if (sids.empty()) return {};

        return std::format(L"SidList: {}", sids);
    }


    std::span<const uint8_t> ExtractPayload(const protocol::QueryQuotaSupplement& supplement) {

        if (!(supplement.captured & protocol::QUERY_QUOTA_CAPTURED_PAYLOAD)) return {};

        return { supplement.payload, supplement.capturedPayloadSize };
    }


    std::wstring RenderQuotasPayload(std::span<const uint8_t> payload) {
        std::wstring result;
        size_t offset = 0u;
        uint32_t index = 1u;
        bool terminated = false;

        while (true) {
            constexpr size_t SID_OFFSET = offsetof(trace::kernel::FILE_QUOTA_INFORMATION, Sid);
            trace::kernel::FILE_QUOTA_INFORMATION entry;

            if (!trace::details::payload::ReadHeader(payload, entry, SID_OFFSET, offset)) break;

            const size_t sidStart = offset + SID_OFFSET;

            if (entry.SidLength > payload.size() - sidStart) break;

            result += std::format(L"{}: Sid: {}", index, trace::values::RenderSid(payload.subspan(sidStart, entry.SidLength)));
            index++;

            if (entry.ChangeTime) {
                result += std::format(L", ChangeTime: {}", trace::values::RenderFileTime(entry.ChangeTime));
            }

            result += std::format(L", QuotaUsed: {}, QuotaThreshold: {}, QuotaLimit: {}, ", entry.QuotaUsed, entry.QuotaThreshold, entry.QuotaLimit);

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
                    std::wstring details = std::format(L"Length: {}, SidListLength: {}", parameters.queryQuota.length, parameters.queryQuota.sidListLength);

                    const std::wstring flags = names::RenderScanFlags(data.operationFlags);

                    if (!flags.empty()) {
                        details += L", ";
                        details += flags;
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
                    std::wstring details = std::format(L"Length: {}", data.parameters.setQuota.length);

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
