#include "create.h"

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
#include <string_view>

using namespace mimo;

namespace {

    std::span<const uint8_t> ExtractEaBuffer(const protocol::CreateSupplement& supplement) {

        if (!(supplement.captured & protocol::CREATE_CAPTURED_EA_BUFFER)) return {};

        return { supplement.eaBuffer, supplement.capturedEaBufferSize };
    }


    std::wstring RenderEaValue(std::span<const uint8_t> valueData, uint16_t valueSize) {
        const size_t dataSize = valueSize < valueData.size() ? valueSize : valueData.size();

        return trace::values::RenderBytes(valueData.first(dataSize), dataSize < valueSize);
    }


    std::wstring RenderExtendedAttributes(std::span<const uint8_t> eaBuffer) {

        if (eaBuffer.empty()) return {};

        std::wstring result;
        size_t offset = 0u;
        uint32_t index = 1u;
        bool terminated = false;

        while (true) {
            constexpr size_t NAME_OFFSET = offsetof(trace::kernel::FILE_FULL_EA_INFORMATION, EaName);
            trace::kernel::FILE_FULL_EA_INFORMATION entry;

            if (!trace::details::payload::ReadHeader(eaBuffer, entry, NAME_OFFSET, offset)) break;

            result += std::format(L"{}: EaName: {}, EaValueLength: {}", index, trace::details::payload::RenderAsciiName(eaBuffer.subspan(offset + NAME_OFFSET), entry.EaNameLength), entry.EaValueLength);
            index++;

            if (entry.EaValueLength) {
                const size_t valueOffset = offset + NAME_OFFSET + entry.EaNameLength + 1u;
                const size_t valueStart = valueOffset < eaBuffer.size() ? valueOffset : eaBuffer.size();

                result += std::format(L", EaValue: {}", RenderEaValue(eaBuffer.subspan(valueStart), entry.EaValueLength));
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

            if (entry.NextEntryOffset > eaBuffer.size()) break;

            offset += entry.NextEntryOffset;
        }

        if (terminated) {
            result.resize(result.size() - 2u);
        }

        return text::MarkTruncated(result, !terminated);
    }

}

namespace mimo {

    namespace trace {

        namespace details {

            namespace create {

                std::wstring Render(const protocol::RecordData& data) {
                    const protocol::FltParameters& parameters = data.parameters;
                    const protocol::CreateSupplement& createSupplement = data.supplement.create;
                    std::wstring result;

                    if (createSupplement.captured & protocol::CREATE_CAPTURED_DESIRED_ACCESS) {
                        result += std::format(L"Desired Access: {}, ", names::RenderDesiredAccess(createSupplement.desiredAccess));
                    }

                    const uint32_t disposition = parameters.create.options >> 24;
                    const uint32_t createOptions = parameters.create.options & 0x00FFFFFFu;

                    result += std::format(L"Disposition: {}, ", names::RenderCreateDisposition(disposition));

                    const std::wstring options = names::RenderCreateOptions(createOptions);

                    if (!options.empty()) {
                        result += std::format(L"Options: {}, ", options);
                    }

                    result += std::format(L"Attributes: {}, ", names::RenderFileAttributes(parameters.create.fileAttributes));
                    result += std::format(L"ShareMode: {}, ", names::RenderShareAccess(parameters.create.shareAccess));

                    if (disposition == kernel::FILE_OPEN) {
                        result += L"AllocationSize: n/a, ";
                    }
                    else {
                        result += std::format(L"AllocationSize: {}, ", parameters.create.allocationSize);
                    }

                    if (parameters.create.eaLength) {
                        result += std::format(L"EaLength: {}, ", parameters.create.eaLength);
                    }

                    const std::wstring flags = names::RenderCreateFlags(data.operationFlags);

                    if (!flags.empty()) {
                        result += flags;
                        result += L", ";
                    }

                    if (createSupplement.captured & protocol::CREATE_CAPTURED_IMPERSONATED_SID) {
                        result += std::format(L"Impersonating: {}, ", values::RenderSid(createSupplement.impersonatedSid));
                    }

                    if (data.status == 0 || data.information == kernel::FILE_EXISTS || data.information == kernel::FILE_DOES_NOT_EXIST) {
                        const std::wstring openResult = names::RenderOpenResult(data.information);

                        if (!openResult.empty()) {
                            result += std::format(L"OpenResult: {}, ", openResult);
                        }
                    }

                    const std::wstring_view ecpText = text::Extract(createSupplement.ecpText);
                    const bool truncated = createSupplement.captured & protocol::CREATE_TRUNCATED_ECP_TEXT;

                    if (!ecpText.empty() || truncated) {
                        result += text::MarkTruncated(ecpText, truncated);
                        result += L", ";
                    }

                    const std::wstring eaBufferText = RenderExtendedAttributes(ExtractEaBuffer(createSupplement));

                    if (!eaBufferText.empty()) {
                        result += eaBufferText;
                        result += L", ";
                    }

                    if (!result.empty()) {
                        result.resize(result.size() - 2u);
                    }

                    return result;
                }

            }

        }

    }

}
