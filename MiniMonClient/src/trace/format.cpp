#include "format.h"

#include "details.h"
#include "names.h"

#include "..\text.h"

#include "..\..\..\inc\protocol.h"

#include <Windows.h>

#include <array>
#include <cstdint>
#include <format>
#include <string>
#include <string_view>

using namespace mimo;

namespace {

    enum Column {
        SEQ_NUM, ALTITUDE, OPERATION_ID, TOP_LEVEL_IRP,
        PRE_OP_TIME, POST_OP_TIME, PROCESS_ID, THREAD_ID, OPR, MAJOR, MINOR, NAME, STATUS, INFORMATION,
        DETAILS, IRP_FLAGS, DEV_OBJ, FILE_OBJ, ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, REPARSE_TAG,
        TRANSACTION, TRANSACTION_SEQ, TRANSACTION_NOTIFY,
        STACK_TRACE, COLUMN_COUNT
    };

    constexpr std::array<std::wstring_view, COLUMN_COUNT> MakeColumnLabels() {
        std::array<std::wstring_view, COLUMN_COUNT> labels{};

        labels[SEQ_NUM]            = L"SeqNum";
        labels[ALTITUDE]           = L"Altitude";
        labels[OPERATION_ID]       = L"OperationId";
        labels[TOP_LEVEL_IRP]      = L"TopLevelIrp";
        labels[PRE_OP_TIME]        = L"PreOpTime";
        labels[POST_OP_TIME]       = L"PostOpTime";
        labels[PROCESS_ID]         = L"ProcessId";
        labels[THREAD_ID]          = L"ThreadId";
        labels[OPR]                = L"Opr";
        labels[MAJOR]              = L"Major";
        labels[MINOR]              = L"Minor";
        labels[NAME]               = L"Name";
        labels[STATUS]             = L"Status";
        labels[INFORMATION]        = L"Information";
        labels[DETAILS]            = L"Details";
        labels[IRP_FLAGS]          = L"IrpFlags";
        labels[DEV_OBJ]            = L"DevObj";
        labels[FILE_OBJ]           = L"FileObj";
        labels[ARG1]               = L"Arg1";
        labels[ARG2]               = L"Arg2";
        labels[ARG3]               = L"Arg3";
        labels[ARG4]               = L"Arg4";
        labels[ARG5]               = L"Arg5";
        labels[ARG6]               = L"Arg6";
        labels[REPARSE_TAG]        = L"ReparseTag";
        labels[TRANSACTION]        = L"Transaction";
        labels[TRANSACTION_SEQ]    = L"TransactionSeq";
        labels[TRANSACTION_NOTIFY] = L"TransactionNotify";
        labels[STACK_TRACE]        = L"StackTrace";

        return labels;
    }


    constexpr std::array<std::wstring_view, COLUMN_COUNT> COLUMN_LABELS = MakeColumnLabels();

    constexpr bool AllColumnsLabeled() {

        for (const std::wstring_view label : COLUMN_LABELS) {

            if (label.empty()) return false;

        }

        return true;
    }


    static_assert(AllColumnsLabeled(), "every Column needs a header label in COLUMN_LABELS");

    constexpr int PTR_WIDTH = static_cast<int>(sizeof(protocol::ObjectId) * 2u);

    std::wstring RenderTime(int64_t kernelTime) {
        const uint64_t ticks = static_cast<uint64_t>(kernelTime);

        FILETIME fileTime{};
        fileTime.dwLowDateTime = static_cast<DWORD>(ticks);
        fileTime.dwHighDateTime = static_cast<DWORD>(ticks >> 32);

        FILETIME localTime{};

        if (!FileTimeToLocalFileTime(&fileTime, &localTime)) return L"TIME ERROR";

        SYSTEMTIME sysTime{};

        if (!FileTimeToSystemTime(&localTime, &sysTime)) return L"TIME ERROR";

        // 100 ns fraction within the second
        const uint32_t subSecond = static_cast<uint32_t>(ticks % 10000000ull);

        return std::format(L"{:02}:{:02}:{:02}.{:07}", sysTime.wHour, sysTime.wMinute, sysTime.wSecond, subSecond);
    }


    std::wstring RenderObject(protocol::ObjectId object) {

        if (object == 0u) return L"";

        return std::format(L"{:0{}X}", object, PTR_WIDTH);
    }


    std::wstring RenderStackTrace(const protocol::RecordData& data) {
        const uint32_t count = data.stackFrameCount < protocol::STACK_TRACE_FRAMES ? data.stackFrameCount : protocol::STACK_TRACE_FRAMES;

        if (count == 0u) return L"";

        std::wstring result;

        for (uint32_t i = 0u; i < count; i++) {

            if (i > 0u) result.push_back(L'|');

            const std::wstring_view modName = text::Extract(data.stackTrace[i].moduleName);

            if (!modName.empty()) {
                result.append(modName);
                result.push_back(L'+');
            }

            result.append(std::format(L"{:X}", data.stackTrace[i].offset));
        }

        return result;
    }


    std::wstring EscapeCsvField(std::wstring_view value) {
        std::wstring escaped;
        bool quoted = false;

        for (const wchar_t character : value) {

            if (character == L'"') {
                escaped.append(LR"("")");
                quoted = true;
            }
            else if (character == L';') {
                escaped.push_back(character);
                quoted = true;
            }
            else if (character < L' ' || character == L'\x7F') {
                escaped.append(std::format(LR"(\x{:02X})", static_cast<uint16_t>(character)));
            }
            else {
                escaped.push_back(character);
            }

        }

        if (quoted) return std::format(LR"("{}")", escaped);

        return escaped;
    }

}

namespace mimo {

    namespace trace {

        namespace format {

            std::wstring GetHeader() {
                std::wstring header;

                for (const std::wstring_view label : COLUMN_LABELS) {
                    header.append(label);
                    header.push_back(L';');
                }

                header.back() = L'\n';

                return header;
            }


            std::wstring Render(const protocol::Record& record) {
                // using enum because some labels (OPERATION_ID, a winbase.h typedef) collide with SDK globals
                using enum Column;
                const protocol::RecordData& data = record.data;
                std::array<std::wstring, COLUMN_COUNT> columns;

                columns[SEQ_NUM]         = std::format(L"{:08X}", record.sequenceNumber);
                columns[ALTITUDE]        = std::to_wstring(data.altitude);
                columns[PRE_OP_TIME]     = RenderTime(data.originatingTime);
                columns[PROCESS_ID]      = std::format(L"{:X}", data.processId);
                columns[THREAD_ID]       = std::format(L"{:X}", data.threadId);
                columns[DEV_OBJ]         = RenderObject(data.deviceObject);
                columns[TRANSACTION]     = RenderObject(data.transaction);
                columns[TRANSACTION_SEQ] = data.transactionSequence ? std::format(L"{:08X}", data.transactionSequence) : std::wstring{};
                columns[STACK_TRACE]     = EscapeCsvField(RenderStackTrace(data));

                if (data.transactionNotify) {
                    columns[OPR]                = L"TX";
                    columns[TRANSACTION_NOTIFY] = names::RenderTransactionNotify(data.transactionNotify);
                }
                else {
                    columns[OPR]           = names::RenderOperationCategory(data.flags);
                    columns[OPERATION_ID]  = std::format(L"{:0{}X}", data.operationId, PTR_WIDTH);
                    columns[TOP_LEVEL_IRP] = RenderObject(data.topLevelIrp);
                    columns[POST_OP_TIME]  = RenderTime(data.completionTime);
                    columns[MAJOR]         = names::RenderMajorFunction(data.callbackMajorId);
                    columns[MINOR]         = names::RenderMinorFunction(data.callbackMajorId, data.callbackMinorId);
                    columns[NAME]          = EscapeCsvField(text::MarkTruncated(text::Extract(data.name), data.truncated & protocol::TRUNCATED_NAME));
                    columns[STATUS]        = std::format(L"{:08X}", static_cast<uint32_t>(data.status));
                    columns[INFORMATION]   = std::format(L"{:0{}X}", data.information, PTR_WIDTH);
                    columns[DETAILS]       = EscapeCsvField(details::Render(data));
                    columns[IRP_FLAGS]     = std::format(L"{:08X}", data.irpFlags);
                    columns[FILE_OBJ]      = RenderObject(data.fileObject);
                    columns[ARG1]          = std::format(L"{:0{}X}", data.parameters.others.argument1, PTR_WIDTH);
                    columns[ARG2]          = std::format(L"{:0{}X}", data.parameters.others.argument2, PTR_WIDTH);
                    columns[ARG3]          = std::format(L"{:0{}X}", data.parameters.others.argument3, PTR_WIDTH);
                    columns[ARG4]          = std::format(L"{:0{}X}", data.parameters.others.argument4, PTR_WIDTH);
                    columns[ARG5]          = std::format(L"{:0{}X}", data.parameters.others.argument5, PTR_WIDTH);
                    columns[ARG6]          = std::format(L"{:0{}X}", static_cast<uint64_t>(data.parameters.others.argument6), PTR_WIDTH);
                    columns[REPARSE_TAG]   = names::RenderReparseTag(data.reparseTag);
                }

                std::wstring line;

                for (const std::wstring& column : columns) {
                    line.append(column);
                    line.push_back(L';');
                }

                line.back() = L'\n';

                return line;
            }

        }

    }

}
