#include "format.h"

#include "..\kernel.h"
#include "..\text.h"
#include "details.h"

#include "..\..\..\inc\protocol.h"

#include <array>
#include <cstddef>
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
        const ULONGLONG ticks = static_cast<ULONGLONG>(kernelTime);

        FILETIME fileTime{};
        fileTime.dwLowDateTime = static_cast<DWORD>(ticks);
        fileTime.dwHighDateTime = static_cast<DWORD>(ticks >> 32);

        FILETIME localTime{};

        if (!FileTimeToLocalFileTime(&fileTime, &localTime)) return L"TIME ERROR";

        SYSTEMTIME sysTime{};

        if (!FileTimeToSystemTime(&localTime, &sysTime)) return L"TIME ERROR";

        // 100 ns fraction within the second
        const ULONG subSecond = static_cast<ULONG>(ticks % 10000000ull);

        return std::format(L"{:02}:{:02}:{:02}.{:07}", sysTime.wHour, sysTime.wMinute, sysTime.wSecond, subSecond);
    }


    std::wstring RenderObject(protocol::ObjectId object) {

        if (object == 0u) return L"";

        return std::format(L"{:0{}X}", object, PTR_WIDTH);
    }


    std::wstring RenderStackTrace(const protocol::RecordData& data) {
        const ULONG count = data.stackFrameCount < protocol::STACK_TRACE_FRAMES ? data.stackFrameCount : protocol::STACK_TRACE_FRAMES;

        if (count == 0u) return L"";

        std::wstring result;

        for (ULONG i = 0u; i < count; i++) {

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
        constexpr std::wstring_view SPECIALS = L";\"\n\r";

        if (value.find_first_of(SPECIALS) == std::wstring_view::npos) return std::wstring{ value };

        std::wstring inner;

        while (true) {
            const size_t quote = value.find(L'"');

            if (quote == std::wstring_view::npos) {
                inner.append(value);

                break;
            }

            inner.append(value.substr(0u, quote));
            inner.append(LR"("")");
            value.remove_prefix(quote + 1u);
        }

        return std::format(LR"("{}")", inner);
    }


    std::wstring RenderTransactionNotify(ULONG notification) {
        const wchar_t* const name = kernel::TransactionNotifyName(notification);

        if (*name) return name;

        return std::format(L"{:08X}", notification);
    }


    std::wstring RenderOperationCategory(ULONG flags) {

        if (flags & kernel::FLT_CALLBACK_DATA_IRP_OPERATION)       return L"IRP";

        if (flags & kernel::FLT_CALLBACK_DATA_FAST_IO_OPERATION)   return L"FASTIO";

        if (flags & kernel::FLT_CALLBACK_DATA_FS_FILTER_OPERATION) return L"FSFILTER";

        return L"OTHER";
    }


    std::wstring RenderMajor(UCHAR major) {
        const wchar_t* const name = kernel::MajorFunctionName(major);

        if (*name) return name;

        return std::format(L"{:02X}", static_cast<ULONG>(major));
    }


    std::wstring RenderMinor(UCHAR major, UCHAR minor) {
        const wchar_t* const name = kernel::MinorFunctionName(major, minor);

        if (*name) return name;

        // a zero minor with no name is an operation that has no minor function
        if (minor == 0u) return L"";

        return std::format(L"{:02X}", static_cast<ULONG>(minor));
    }


    std::wstring RenderReparseTag(ULONG tag) {

        if (tag == 0u) return L"";

        const wchar_t* const name = kernel::ReparseTagName(tag);

        if (*name) return name;

        return std::format(L"{:08X}", tag);
    }

}

namespace mimo {

    namespace log {

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
                // some column names (e.g. OPERATION_ID, a winbase.h typedef) collide with Windows SDK globals
                // without this they'd be ambiguous here
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
                    columns[TRANSACTION_NOTIFY] = RenderTransactionNotify(data.transactionNotify);
                }
                else {
                    columns[OPR]           = RenderOperationCategory(data.flags);
                    columns[OPERATION_ID]  = std::format(L"{:0{}X}", data.operationId, PTR_WIDTH);
                    columns[TOP_LEVEL_IRP] = RenderObject(data.topLevelIrp);
                    columns[POST_OP_TIME]  = RenderTime(data.completionTime);
                    columns[MAJOR]         = RenderMajor(data.callbackMajorId);
                    columns[MINOR]         = RenderMinor(data.callbackMajorId, data.callbackMinorId);
                    columns[NAME]          = EscapeCsvField(text::Extract(data.name));
                    columns[STATUS]        = std::format(L"{:08X}", static_cast<ULONG>(data.status));
                    columns[INFORMATION]   = std::format(L"{:0{}X}", data.information, PTR_WIDTH);
                    columns[DETAILS]       = EscapeCsvField(details::Render(data));
                    columns[IRP_FLAGS]     = std::format(L"{:08X}", data.irpFlags);
                    columns[FILE_OBJ]      = RenderObject(data.fileObject);
                    columns[ARG1]          = std::format(L"{:0{}X}", data.parameters.others.argument1, PTR_WIDTH);
                    columns[ARG2]          = std::format(L"{:0{}X}", data.parameters.others.argument2, PTR_WIDTH);
                    columns[ARG3]          = std::format(L"{:0{}X}", data.parameters.others.argument3, PTR_WIDTH);
                    columns[ARG4]          = std::format(L"{:0{}X}", data.parameters.others.argument4, PTR_WIDTH);
                    columns[ARG5]          = std::format(L"{:0{}X}", data.parameters.others.argument5, PTR_WIDTH);
                    columns[ARG6]          = std::format(L"{:0{}X}", static_cast<ULONGLONG>(data.parameters.others.argument6), PTR_WIDTH);
                    columns[REPARSE_TAG]   = RenderReparseTag(data.reparseTag);
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
