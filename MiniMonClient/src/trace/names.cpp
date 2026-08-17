#include "names.h"

#include "kernel.h"

#include <Windows.h>
#include <winioctl.h>
#include <wnnc.h>

#include <cstddef>
#include <cstdint>
#include <format>
#include <span>
#include <string>
#include <string_view>

using namespace mimo;

namespace {

    struct FlagName {
        uint32_t flag;
        const wchar_t* name;
    };

    // an entry that is a subset of a later entry would make the later one unreachable in flag rendering
    template <size_t count>
    constexpr bool CompositesPrecedeComponents(const FlagName (&names)[count]) noexcept {

        for (size_t i = 0u; i < count; i++) {

            for (size_t j = i + 1u; j < count; j++) {
                if ((names[i].flag & names[j].flag) == names[i].flag && names[i].flag != names[j].flag) return false;
            }
        }

        return true;
    }


    std::wstring RenderFlags(
        uint32_t flags,
        std::span<const FlagName> names,
        std::wstring_view separator
    ) {
        std::wstring result;

        for (const FlagName& entry : names) {

            if ((flags & entry.flag) != entry.flag) continue;

            result += entry.name;
            result += separator;
            flags &= ~entry.flag;
        }

        if (flags) {
            result += std::format(L"0x{:X}", flags);
            result += separator;
        }

        if (!result.empty()) {
            result.resize(result.size() - separator.size());
        }

        return result;
    }


    constexpr FlagName DESIRED_ACCESS_NAMES[]{
        { FILE_ALL_ACCESS,                                               L"All Access" },
        { FILE_GENERIC_READ | FILE_GENERIC_WRITE | FILE_GENERIC_EXECUTE, L"Generic Read/Write/Execute" },
        { FILE_GENERIC_READ | FILE_GENERIC_WRITE,                        L"Generic Read/Write" },
        { FILE_GENERIC_READ | FILE_GENERIC_EXECUTE,                      L"Generic Read/Execute" },
        { FILE_GENERIC_WRITE | FILE_GENERIC_EXECUTE,                     L"Generic Write/Execute" },
        { FILE_GENERIC_READ,                                             L"Generic Read" },
        { FILE_GENERIC_WRITE,                                            L"Generic Write" },
        { FILE_GENERIC_EXECUTE,                                          L"Generic Execute" },
        { GENERIC_READ,           L"Generic Read" },
        { GENERIC_WRITE,          L"Generic Write" },
        { GENERIC_EXECUTE,        L"Generic Execute" },
        { GENERIC_ALL,            L"Generic All" },
        { MAXIMUM_ALLOWED,        L"Maximum Allowed" },
        { FILE_READ_DATA,         L"Read Data/List Directory" },
        { FILE_WRITE_DATA,        L"Write Data/Add File" },
        { FILE_APPEND_DATA,       L"Append Data/Add Subdirectory/Create Pipe Instance" },
        { FILE_READ_EA,           L"Read EA" },
        { FILE_WRITE_EA,          L"Write EA" },
        { FILE_EXECUTE,           L"Execute/Traverse" },
        { FILE_DELETE_CHILD,      L"Delete Child" },
        { FILE_READ_ATTRIBUTES,   L"Read Attributes" },
        { FILE_WRITE_ATTRIBUTES,  L"Write Attributes" },
        { DELETE,                 L"Delete" },
        { READ_CONTROL,           L"Read Control" },
        { WRITE_DAC,              L"Write DAC" },
        { WRITE_OWNER,            L"Write Owner" },
        { SYNCHRONIZE,            L"Synchronize" },
        { ACCESS_SYSTEM_SECURITY, L"Access System Security" },
    };

    static_assert(CompositesPrecedeComponents(DESIRED_ACCESS_NAMES), "composite entry after its components in DESIRED_ACCESS_NAMES");

    constexpr FlagName CREATE_OPTION_NAMES[]{
        { trace::kernel::FILE_DIRECTORY_FILE,            L"Directory" },
        { trace::kernel::FILE_WRITE_THROUGH,             L"Write Through" },
        { trace::kernel::FILE_SEQUENTIAL_ONLY,           L"Sequential Access" },
        { trace::kernel::FILE_NO_INTERMEDIATE_BUFFERING, L"No Buffering" },
        { trace::kernel::FILE_SYNCHRONOUS_IO_ALERT,      L"Synchronous IO Alert" },
        { trace::kernel::FILE_SYNCHRONOUS_IO_NONALERT,   L"Synchronous IO Non-Alert" },
        { trace::kernel::FILE_NON_DIRECTORY_FILE,        L"Non-Directory File" },
        { trace::kernel::FILE_CREATE_TREE_CONNECTION,    L"Create Tree Connection" },
        { trace::kernel::FILE_COMPLETE_IF_OPLOCKED,      L"Complete If Oplocked" },
        { trace::kernel::FILE_NO_EA_KNOWLEDGE,           L"No EA Knowledge" },
        { trace::kernel::FILE_OPEN_REMOTE_INSTANCE,      L"Open Remote Instance" },
        { trace::kernel::FILE_RANDOM_ACCESS,             L"Random Access" },
        { trace::kernel::FILE_DELETE_ON_CLOSE,           L"Delete On Close" },
        { trace::kernel::FILE_OPEN_BY_FILE_ID,           L"Open By ID" },
        { trace::kernel::FILE_OPEN_FOR_BACKUP_INTENT,    L"Open For Backup" },
        { trace::kernel::FILE_NO_COMPRESSION,            L"No Compression" },
        { trace::kernel::FILE_OPEN_REQUIRING_OPLOCK,     L"Open Requiring Oplock" },
        { trace::kernel::FILE_DISALLOW_EXCLUSIVE,        L"Disallow Exclusive" },
        { trace::kernel::FILE_SESSION_AWARE,             L"Session Aware" },
        { trace::kernel::FILE_RESERVE_OPFILTER,          L"Reserve OpFilter" },
        { trace::kernel::FILE_OPEN_REPARSE_POINT,        L"Open Reparse Point" },
        { trace::kernel::FILE_OPEN_NO_RECALL,            L"Open No Recall" },
        { trace::kernel::FILE_OPEN_FOR_FREE_SPACE_QUERY, L"Open For Free Space Query" },
    };

    static_assert(CompositesPrecedeComponents(CREATE_OPTION_NAMES), "composite entry after its components in CREATE_OPTION_NAMES");

    constexpr FlagName SHARE_ACCESS_NAMES[]{
        { FILE_SHARE_READ,   L"Read" },
        { FILE_SHARE_WRITE,  L"Write" },
        { FILE_SHARE_DELETE, L"Delete" },
    };

    static_assert(CompositesPrecedeComponents(SHARE_ACCESS_NAMES), "composite entry after its components in SHARE_ACCESS_NAMES");

    constexpr FlagName FILE_ATTRIBUTE_LETTERS[]{
        { FILE_ATTRIBUTE_READONLY,            L"R" },
        { FILE_ATTRIBUTE_HIDDEN,              L"H" },
        { FILE_ATTRIBUTE_SYSTEM,              L"S" },
        { FILE_ATTRIBUTE_DIRECTORY,           L"D" },
        { FILE_ATTRIBUTE_ARCHIVE,             L"A" },
        { FILE_ATTRIBUTE_DEVICE,              L"DE" },
        { FILE_ATTRIBUTE_NORMAL,              L"N" },
        { FILE_ATTRIBUTE_TEMPORARY,           L"T" },
        { FILE_ATTRIBUTE_SPARSE_FILE,         L"SP" },
        { FILE_ATTRIBUTE_REPARSE_POINT,       L"RP" },
        { FILE_ATTRIBUTE_COMPRESSED,          L"C" },
        { FILE_ATTRIBUTE_OFFLINE,             L"O" },
        { FILE_ATTRIBUTE_NOT_CONTENT_INDEXED, L"NCI" },
        { FILE_ATTRIBUTE_ENCRYPTED,           L"E" },
        { FILE_ATTRIBUTE_INTEGRITY_STREAM,    L"IS" },
        { FILE_ATTRIBUTE_NO_SCRUB_DATA,       L"NSD" },
    };

    static_assert(CompositesPrecedeComponents(FILE_ATTRIBUTE_LETTERS), "composite entry after its components in FILE_ATTRIBUTE_LETTERS");

    constexpr FlagName REMOTE_PROTOCOL_FLAG_NAMES[]{
        { trace::kernel::REMOTE_PROTOCOL_FLAG_LOOPBACK,          L"Loopback" },
        { trace::kernel::REMOTE_PROTOCOL_FLAG_OFFLINE,           L"Offline" },
        { trace::kernel::REMOTE_PROTOCOL_FLAG_PERSISTENT_HANDLE, L"Persistent Handle" },
        { trace::kernel::REMOTE_PROTOCOL_FLAG_PRIVACY,           L"Privacy" },
        { trace::kernel::REMOTE_PROTOCOL_FLAG_INTEGRITY,         L"Integrity" },
        { trace::kernel::REMOTE_PROTOCOL_FLAG_MUTUAL_AUTH,       L"Mutual Auth" },
    };

    static_assert(CompositesPrecedeComponents(REMOTE_PROTOCOL_FLAG_NAMES), "composite entry after its components in REMOTE_PROTOCOL_FLAG_NAMES");

    constexpr FlagName DISPOSITION_FLAG_NAMES[]{
        { trace::kernel::FILE_DISPOSITION_DELETE,                    L"Delete" },
        { trace::kernel::FILE_DISPOSITION_POSIX_SEMANTICS,           L"POSIX Semantics" },
        { trace::kernel::FILE_DISPOSITION_FORCE_IMAGE_SECTION_CHECK, L"Force Image Section Check" },
        { trace::kernel::FILE_DISPOSITION_ON_CLOSE,                  L"On Close" },
        { trace::kernel::FILE_DISPOSITION_IGNORE_READONLY_ATTRIBUTE, L"Ignore Readonly Attribute" },
    };

    static_assert(CompositesPrecedeComponents(DISPOSITION_FLAG_NAMES), "composite entry after its components in DISPOSITION_FLAG_NAMES");

    constexpr FlagName RENAME_FLAG_NAMES[]{
        { trace::kernel::FILE_RENAME_FORCE_RESIZE_SR,                      L"Force Resize SR" },
        { trace::kernel::FILE_RENAME_PRESERVE_AVAILABLE_SPACE,             L"Preserve Available Space" },
        { trace::kernel::FILE_RENAME_REPLACE_IF_EXISTS,                    L"Replace If Exists" },
        { trace::kernel::FILE_RENAME_POSIX_SEMANTICS,                      L"POSIX Semantics" },
        { trace::kernel::FILE_RENAME_SUPPRESS_PIN_STATE_INHERITANCE,       L"Suppress Pin State Inheritance" },
        { trace::kernel::FILE_RENAME_SUPPRESS_STORAGE_RESERVE_INHERITANCE, L"Suppress Storage Reserve Inheritance" },
        { trace::kernel::FILE_RENAME_NO_INCREASE_AVAILABLE_SPACE,          L"No Increase Available Space" },
        { trace::kernel::FILE_RENAME_NO_DECREASE_AVAILABLE_SPACE,          L"No Decrease Available Space" },
        { trace::kernel::FILE_RENAME_IGNORE_READONLY_ATTRIBUTE,            L"Ignore Readonly Attribute" },
        { trace::kernel::FILE_RENAME_FORCE_RESIZE_TARGET_SR,               L"Force Resize Target SR" },
        { trace::kernel::FILE_RENAME_FORCE_RESIZE_SOURCE_SR,               L"Force Resize Source SR" },
    };

    static_assert(CompositesPrecedeComponents(RENAME_FLAG_NAMES), "composite entry after its components in RENAME_FLAG_NAMES");

    constexpr FlagName FILE_SYSTEM_ATTRIBUTE_NAMES[]{
        { FILE_CASE_SENSITIVE_SEARCH,        L"FILE_CASE_SENSITIVE_SEARCH" },
        { FILE_CASE_PRESERVED_NAMES,         L"FILE_CASE_PRESERVED_NAMES" },
        { FILE_UNICODE_ON_DISK,              L"FILE_UNICODE_ON_DISK" },
        { FILE_PERSISTENT_ACLS,              L"FILE_PERSISTENT_ACLS" },
        { FILE_FILE_COMPRESSION,             L"FILE_FILE_COMPRESSION" },
        { FILE_VOLUME_QUOTAS,                L"FILE_VOLUME_QUOTAS" },
        { FILE_SUPPORTS_SPARSE_FILES,        L"FILE_SUPPORTS_SPARSE_FILES" },
        { FILE_SUPPORTS_REPARSE_POINTS,      L"FILE_SUPPORTS_REPARSE_POINTS" },
        { FILE_SUPPORTS_REMOTE_STORAGE,      L"FILE_SUPPORTS_REMOTE_STORAGE" },
        { FILE_RETURNS_CLEANUP_RESULT_INFO,  L"FILE_RETURNS_CLEANUP_RESULT_INFO" },
        { FILE_SUPPORTS_POSIX_UNLINK_RENAME, L"FILE_SUPPORTS_POSIX_UNLINK_RENAME" },
        { FILE_SUPPORTS_BYPASS_IO,           L"FILE_SUPPORTS_BYPASS_IO" },
        { FILE_SUPPORTS_STREAM_SNAPSHOTS,    L"FILE_SUPPORTS_STREAM_SNAPSHOTS" },
        { FILE_SUPPORTS_CASE_SENSITIVE_DIRS, L"FILE_SUPPORTS_CASE_SENSITIVE_DIRS" },
        { FILE_VOLUME_IS_COMPRESSED,         L"FILE_VOLUME_IS_COMPRESSED" },
        { FILE_SUPPORTS_OBJECT_IDS,          L"FILE_SUPPORTS_OBJECT_IDS" },
        { FILE_SUPPORTS_ENCRYPTION,          L"FILE_SUPPORTS_ENCRYPTION" },
        { FILE_NAMED_STREAMS,                L"FILE_NAMED_STREAMS" },
        { FILE_READ_ONLY_VOLUME,             L"FILE_READ_ONLY_VOLUME" },
        { FILE_SEQUENTIAL_WRITE_ONCE,        L"FILE_SEQUENTIAL_WRITE_ONCE" },
        { FILE_SUPPORTS_TRANSACTIONS,        L"FILE_SUPPORTS_TRANSACTIONS" },
        { FILE_SUPPORTS_HARD_LINKS,          L"FILE_SUPPORTS_HARD_LINKS" },
        { FILE_SUPPORTS_EXTENDED_ATTRIBUTES, L"FILE_SUPPORTS_EXTENDED_ATTRIBUTES" },
        { FILE_SUPPORTS_OPEN_BY_FILE_ID,     L"FILE_SUPPORTS_OPEN_BY_FILE_ID" },
        { FILE_SUPPORTS_USN_JOURNAL,         L"FILE_SUPPORTS_USN_JOURNAL" },
        { FILE_SUPPORTS_INTEGRITY_STREAMS,   L"FILE_SUPPORTS_INTEGRITY_STREAMS" },
        { FILE_SUPPORTS_BLOCK_REFCOUNTING,   L"FILE_SUPPORTS_BLOCK_REFCOUNTING" },
        { FILE_SUPPORTS_SPARSE_VDL,          L"FILE_SUPPORTS_SPARSE_VDL" },
        { FILE_DAX_VOLUME,                   L"FILE_DAX_VOLUME" },
        { FILE_SUPPORTS_GHOSTING,            L"FILE_SUPPORTS_GHOSTING" },
    };

    static_assert(CompositesPrecedeComponents(FILE_SYSTEM_ATTRIBUTE_NAMES), "composite entry after its components in FILE_SYSTEM_ATTRIBUTE_NAMES");

    constexpr FlagName QUERY_DIRECTORY_FLAG_NAMES[]{
        { trace::kernel::SL_RESTART_SCAN,        L"Restart Scan" },
        { trace::kernel::SL_RETURN_SINGLE_ENTRY, L"Return Single Entry" },
        { trace::kernel::SL_INDEX_SPECIFIED,     L"Index Specified" },
    };

    static_assert(CompositesPrecedeComponents(QUERY_DIRECTORY_FLAG_NAMES), "composite entry after its components in QUERY_DIRECTORY_FLAG_NAMES");

    constexpr FlagName COMPLETION_FILTER_NAMES[]{
        { FILE_NOTIFY_CHANGE_FILE_NAME,                   L"FILE_NOTIFY_CHANGE_FILE_NAME" },
        { FILE_NOTIFY_CHANGE_DIR_NAME,                    L"FILE_NOTIFY_CHANGE_DIR_NAME" },
        { FILE_NOTIFY_CHANGE_ATTRIBUTES,                  L"FILE_NOTIFY_CHANGE_ATTRIBUTES" },
        { FILE_NOTIFY_CHANGE_SIZE,                        L"FILE_NOTIFY_CHANGE_SIZE" },
        { FILE_NOTIFY_CHANGE_LAST_WRITE,                  L"FILE_NOTIFY_CHANGE_LAST_WRITE" },
        { FILE_NOTIFY_CHANGE_LAST_ACCESS,                 L"FILE_NOTIFY_CHANGE_LAST_ACCESS" },
        { FILE_NOTIFY_CHANGE_CREATION,                    L"FILE_NOTIFY_CHANGE_CREATION" },
        { trace::kernel::FILE_NOTIFY_CHANGE_EA,           L"FILE_NOTIFY_CHANGE_EA" },
        { FILE_NOTIFY_CHANGE_SECURITY,                    L"FILE_NOTIFY_CHANGE_SECURITY" },
        { trace::kernel::FILE_NOTIFY_CHANGE_STREAM_NAME,  L"FILE_NOTIFY_CHANGE_STREAM_NAME" },
        { trace::kernel::FILE_NOTIFY_CHANGE_STREAM_SIZE,  L"FILE_NOTIFY_CHANGE_STREAM_SIZE" },
        { trace::kernel::FILE_NOTIFY_CHANGE_STREAM_WRITE, L"FILE_NOTIFY_CHANGE_STREAM_WRITE" },
    };

    static_assert(CompositesPrecedeComponents(COMPLETION_FILTER_NAMES), "composite entry after its components in COMPLETION_FILTER_NAMES");

    constexpr const wchar_t* CONTROL_METHOD_NAMES[]{ L"METHOD_BUFFERED", L"METHOD_IN_DIRECT", L"METHOD_OUT_DIRECT", L"METHOD_NEITHER" };

    constexpr const wchar_t* CONTROL_ACCESS_NAMES[]{ L"FILE_ANY_ACCESS", L"FILE_READ_ACCESS", L"FILE_WRITE_ACCESS", L"FILE_READ_ACCESS|FILE_WRITE_ACCESS" };

    std::wstring RenderControlCode(uint32_t controlCode) {

        return std::format(L"0x{:X} (Device: {}, Function: {}, Method: {}, Access: {})", controlCode, trace::names::RenderDeviceType(controlCode >> 16), (controlCode >> 2) & 0xFFFu, CONTROL_METHOD_NAMES[controlCode & 3u], CONTROL_ACCESS_NAMES[(controlCode >> 14) & 3u]);
    }


    constexpr FlagName SYMLINK_FLAG_NAMES[]{
        { trace::kernel::SYMLINK_FLAG_RELATIVE, L"SYMLINK_FLAG_RELATIVE" },
    };

    static_assert(CompositesPrecedeComponents(SYMLINK_FLAG_NAMES), "composite entry after its components in SYMLINK_FLAG_NAMES");

    constexpr FlagName OPLOCK_LEVEL_NAMES[]{
        { OPLOCK_LEVEL_CACHE_READ,   L"OPLOCK_LEVEL_CACHE_READ" },
        { OPLOCK_LEVEL_CACHE_HANDLE, L"OPLOCK_LEVEL_CACHE_HANDLE" },
        { OPLOCK_LEVEL_CACHE_WRITE,  L"OPLOCK_LEVEL_CACHE_WRITE" },
    };

    static_assert(CompositesPrecedeComponents(OPLOCK_LEVEL_NAMES), "composite entry after its components in OPLOCK_LEVEL_NAMES");

    constexpr FlagName OPLOCK_INPUT_FLAG_NAMES[]{
        { REQUEST_OPLOCK_INPUT_FLAG_REQUEST,                   L"REQUEST_OPLOCK_INPUT_FLAG_REQUEST" },
        { REQUEST_OPLOCK_INPUT_FLAG_ACK,                       L"REQUEST_OPLOCK_INPUT_FLAG_ACK" },
        { REQUEST_OPLOCK_INPUT_FLAG_COMPLETE_ACK_ON_CLOSE,     L"REQUEST_OPLOCK_INPUT_FLAG_COMPLETE_ACK_ON_CLOSE" },
        { REQUEST_OPLOCK_INPUT_FLAG_RH_ALWAYS_BLOCK_UNTIL_ACK, L"REQUEST_OPLOCK_INPUT_FLAG_RH_ALWAYS_BLOCK_UNTIL_ACK" },
        { REQUEST_OPLOCK_INPUT_FLAG_RH_IGNORE_WRITES,          L"REQUEST_OPLOCK_INPUT_FLAG_RH_IGNORE_WRITES" },
        { REQUEST_OPLOCK_INPUT_FLAG_RH_NO_NON_CACHED_IO,       L"REQUEST_OPLOCK_INPUT_FLAG_RH_NO_NON_CACHED_IO" },
    };

    static_assert(CompositesPrecedeComponents(OPLOCK_INPUT_FLAG_NAMES), "composite entry after its components in OPLOCK_INPUT_FLAG_NAMES");

    constexpr FlagName OPLOCK_OUTPUT_FLAG_NAMES[]{
        { REQUEST_OPLOCK_OUTPUT_FLAG_ACK_REQUIRED,   L"REQUEST_OPLOCK_OUTPUT_FLAG_ACK_REQUIRED" },
        { REQUEST_OPLOCK_OUTPUT_FLAG_MODES_PROVIDED, L"REQUEST_OPLOCK_OUTPUT_FLAG_MODES_PROVIDED" },
    };

    static_assert(CompositesPrecedeComponents(OPLOCK_OUTPUT_FLAG_NAMES), "composite entry after its components in OPLOCK_OUTPUT_FLAG_NAMES");

    constexpr FlagName FILE_REGION_USAGE_NAMES[]{
        { FILE_REGION_USAGE_VALID_CACHED_DATA,    L"FILE_REGION_USAGE_VALID_CACHED_DATA" },
        { FILE_REGION_USAGE_VALID_NONCACHED_DATA, L"FILE_REGION_USAGE_VALID_NONCACHED_DATA" },
        { FILE_REGION_USAGE_OTHER_PAGE_ALIGNMENT, L"FILE_REGION_USAGE_OTHER_PAGE_ALIGNMENT" },
        { FILE_REGION_USAGE_LARGE_PAGE_ALIGNMENT, L"FILE_REGION_USAGE_LARGE_PAGE_ALIGNMENT" },
        // the macro is _WIN64-only in winioctl.h; spelled by value so the x86 client renders x64 records identically
        { 0x00000010u,                            L"FILE_REGION_USAGE_HUGE_PAGE_ALIGNMENT" },
    };

    static_assert(CompositesPrecedeComponents(FILE_REGION_USAGE_NAMES), "composite entry after its components in FILE_REGION_USAGE_NAMES");

    constexpr FlagName USN_REASON_NAMES[]{
        { USN_REASON_DATA_OVERWRITE,               L"USN_REASON_DATA_OVERWRITE" },
        { USN_REASON_DATA_EXTEND,                  L"USN_REASON_DATA_EXTEND" },
        { USN_REASON_DATA_TRUNCATION,              L"USN_REASON_DATA_TRUNCATION" },
        { USN_REASON_NAMED_DATA_OVERWRITE,         L"USN_REASON_NAMED_DATA_OVERWRITE" },
        { USN_REASON_NAMED_DATA_EXTEND,            L"USN_REASON_NAMED_DATA_EXTEND" },
        { USN_REASON_NAMED_DATA_TRUNCATION,        L"USN_REASON_NAMED_DATA_TRUNCATION" },
        { USN_REASON_FILE_CREATE,                  L"USN_REASON_FILE_CREATE" },
        { USN_REASON_FILE_DELETE,                  L"USN_REASON_FILE_DELETE" },
        { USN_REASON_EA_CHANGE,                    L"USN_REASON_EA_CHANGE" },
        { USN_REASON_SECURITY_CHANGE,              L"USN_REASON_SECURITY_CHANGE" },
        { USN_REASON_RENAME_OLD_NAME,              L"USN_REASON_RENAME_OLD_NAME" },
        { USN_REASON_RENAME_NEW_NAME,              L"USN_REASON_RENAME_NEW_NAME" },
        { USN_REASON_INDEXABLE_CHANGE,             L"USN_REASON_INDEXABLE_CHANGE" },
        { USN_REASON_BASIC_INFO_CHANGE,            L"USN_REASON_BASIC_INFO_CHANGE" },
        { USN_REASON_HARD_LINK_CHANGE,             L"USN_REASON_HARD_LINK_CHANGE" },
        { USN_REASON_COMPRESSION_CHANGE,           L"USN_REASON_COMPRESSION_CHANGE" },
        { USN_REASON_ENCRYPTION_CHANGE,            L"USN_REASON_ENCRYPTION_CHANGE" },
        { USN_REASON_OBJECT_ID_CHANGE,             L"USN_REASON_OBJECT_ID_CHANGE" },
        { USN_REASON_REPARSE_POINT_CHANGE,         L"USN_REASON_REPARSE_POINT_CHANGE" },
        { USN_REASON_STREAM_CHANGE,                L"USN_REASON_STREAM_CHANGE" },
        { USN_REASON_TRANSACTED_CHANGE,            L"USN_REASON_TRANSACTED_CHANGE" },
        { USN_REASON_INTEGRITY_CHANGE,             L"USN_REASON_INTEGRITY_CHANGE" },
        { USN_REASON_DESIRED_STORAGE_CLASS_CHANGE, L"USN_REASON_DESIRED_STORAGE_CLASS_CHANGE" },
        { USN_REASON_CLOSE,                        L"USN_REASON_CLOSE" },
    };

    static_assert(CompositesPrecedeComponents(USN_REASON_NAMES), "composite entry after its components in USN_REASON_NAMES");

    constexpr FlagName SECURITY_INFORMATION_NAMES[]{
        { OWNER_SECURITY_INFORMATION,               L"Owner" },
        { GROUP_SECURITY_INFORMATION,               L"Group" },
        { DACL_SECURITY_INFORMATION,                L"DACL" },
        { SACL_SECURITY_INFORMATION,                L"SACL" },
        { LABEL_SECURITY_INFORMATION,               L"Label" },
        { ATTRIBUTE_SECURITY_INFORMATION,           L"Attribute" },
        { SCOPE_SECURITY_INFORMATION,               L"Scope" },
        { PROCESS_TRUST_LABEL_SECURITY_INFORMATION, L"Process Trust Label" },
        { ACCESS_FILTER_SECURITY_INFORMATION,       L"Access Filter" },
        { BACKUP_SECURITY_INFORMATION,              L"Backup" },
        { PROTECTED_DACL_SECURITY_INFORMATION,      L"Protected DACL" },
        { PROTECTED_SACL_SECURITY_INFORMATION,      L"Protected SACL" },
        { UNPROTECTED_DACL_SECURITY_INFORMATION,    L"Unprotected DACL" },
        { UNPROTECTED_SACL_SECURITY_INFORMATION,    L"Unprotected SACL" },
    };

    static_assert(CompositesPrecedeComponents(SECURITY_INFORMATION_NAMES), "composite entry after its components in SECURITY_INFORMATION_NAMES");

}

namespace mimo {

    namespace trace {

        namespace names {

            std::wstring RenderOperationCategory(uint32_t flags) {

                if (flags & kernel::FLT_CALLBACK_DATA_IRP_OPERATION)       return L"IRP";

                if (flags & kernel::FLT_CALLBACK_DATA_FAST_IO_OPERATION)   return L"FASTIO";

                if (flags & kernel::FLT_CALLBACK_DATA_FS_FILTER_OPERATION) return L"FSFILTER";

                return L"OTHER";
            }


            std::wstring RenderMajorFunction(uint8_t major) {

                switch (major) {
                    case kernel::IRP_MJ_CREATE:                              return L"IRP_MJ_CREATE";
                    case kernel::IRP_MJ_CREATE_NAMED_PIPE:                   return L"IRP_MJ_CREATE_NAMED_PIPE";
                    case kernel::IRP_MJ_CLOSE:                               return L"IRP_MJ_CLOSE";
                    case kernel::IRP_MJ_READ:                                return L"IRP_MJ_READ";
                    case kernel::IRP_MJ_WRITE:                               return L"IRP_MJ_WRITE";
                    case kernel::IRP_MJ_QUERY_INFORMATION:                   return L"IRP_MJ_QUERY_INFORMATION";
                    case kernel::IRP_MJ_SET_INFORMATION:                     return L"IRP_MJ_SET_INFORMATION";
                    case kernel::IRP_MJ_QUERY_EA:                            return L"IRP_MJ_QUERY_EA";
                    case kernel::IRP_MJ_SET_EA:                              return L"IRP_MJ_SET_EA";
                    case kernel::IRP_MJ_FLUSH_BUFFERS:                       return L"IRP_MJ_FLUSH_BUFFERS";
                    case kernel::IRP_MJ_QUERY_VOLUME_INFORMATION:            return L"IRP_MJ_QUERY_VOLUME_INFORMATION";
                    case kernel::IRP_MJ_SET_VOLUME_INFORMATION:              return L"IRP_MJ_SET_VOLUME_INFORMATION";
                    case kernel::IRP_MJ_DIRECTORY_CONTROL:                   return L"IRP_MJ_DIRECTORY_CONTROL";
                    case kernel::IRP_MJ_FILE_SYSTEM_CONTROL:                 return L"IRP_MJ_FILE_SYSTEM_CONTROL";
                    case kernel::IRP_MJ_DEVICE_CONTROL:                      return L"IRP_MJ_DEVICE_CONTROL";
                    case kernel::IRP_MJ_INTERNAL_DEVICE_CONTROL:             return L"IRP_MJ_INTERNAL_DEVICE_CONTROL";
                    case kernel::IRP_MJ_SHUTDOWN:                            return L"IRP_MJ_SHUTDOWN";
                    case kernel::IRP_MJ_LOCK_CONTROL:                        return L"IRP_MJ_LOCK_CONTROL";
                    case kernel::IRP_MJ_CLEANUP:                             return L"IRP_MJ_CLEANUP";
                    case kernel::IRP_MJ_CREATE_MAILSLOT:                     return L"IRP_MJ_CREATE_MAILSLOT";
                    case kernel::IRP_MJ_QUERY_SECURITY:                      return L"IRP_MJ_QUERY_SECURITY";
                    case kernel::IRP_MJ_SET_SECURITY:                        return L"IRP_MJ_SET_SECURITY";
                    case kernel::IRP_MJ_POWER:                               return L"IRP_MJ_POWER";
                    case kernel::IRP_MJ_SYSTEM_CONTROL:                      return L"IRP_MJ_SYSTEM_CONTROL";
                    case kernel::IRP_MJ_DEVICE_CHANGE:                       return L"IRP_MJ_DEVICE_CHANGE";
                    case kernel::IRP_MJ_QUERY_QUOTA:                         return L"IRP_MJ_QUERY_QUOTA";
                    case kernel::IRP_MJ_SET_QUOTA:                           return L"IRP_MJ_SET_QUOTA";
                    case kernel::IRP_MJ_PNP:                                 return L"IRP_MJ_PNP";
                    case kernel::IRP_MJ_ACQUIRE_FOR_SECTION_SYNCHRONIZATION: return L"IRP_MJ_ACQUIRE_FOR_SECTION_SYNCHRONIZATION";
                    case kernel::IRP_MJ_RELEASE_FOR_SECTION_SYNCHRONIZATION: return L"IRP_MJ_RELEASE_FOR_SECTION_SYNCHRONIZATION";
                    case kernel::IRP_MJ_ACQUIRE_FOR_MOD_WRITE:               return L"IRP_MJ_ACQUIRE_FOR_MOD_WRITE";
                    case kernel::IRP_MJ_RELEASE_FOR_MOD_WRITE:               return L"IRP_MJ_RELEASE_FOR_MOD_WRITE";
                    case kernel::IRP_MJ_ACQUIRE_FOR_CC_FLUSH:                return L"IRP_MJ_ACQUIRE_FOR_CC_FLUSH";
                    case kernel::IRP_MJ_RELEASE_FOR_CC_FLUSH:                return L"IRP_MJ_RELEASE_FOR_CC_FLUSH";
                    case kernel::IRP_MJ_QUERY_OPEN:                          return L"IRP_MJ_QUERY_OPEN";
                    case kernel::IRP_MJ_FAST_IO_CHECK_IF_POSSIBLE:           return L"IRP_MJ_FAST_IO_CHECK_IF_POSSIBLE";
                    case kernel::IRP_MJ_NETWORK_QUERY_OPEN:                  return L"IRP_MJ_NETWORK_QUERY_OPEN";
                    case kernel::IRP_MJ_MDL_READ:                            return L"IRP_MJ_MDL_READ";
                    case kernel::IRP_MJ_MDL_READ_COMPLETE:                   return L"IRP_MJ_MDL_READ_COMPLETE";
                    case kernel::IRP_MJ_PREPARE_MDL_WRITE:                   return L"IRP_MJ_PREPARE_MDL_WRITE";
                    case kernel::IRP_MJ_MDL_WRITE_COMPLETE:                  return L"IRP_MJ_MDL_WRITE_COMPLETE";
                    case kernel::IRP_MJ_VOLUME_MOUNT:                        return L"IRP_MJ_VOLUME_MOUNT";
                    case kernel::IRP_MJ_VOLUME_DISMOUNT:                     return L"IRP_MJ_VOLUME_DISMOUNT";
                }

                return std::format(L"{:02X}", static_cast<uint32_t>(major));
            }


            std::wstring RenderMinorFunction(uint8_t major, uint8_t minor) {

                switch (major) {
                    case kernel::IRP_MJ_DIRECTORY_CONTROL:
                        switch (minor) {
                            case kernel::IRP_MN_QUERY_DIRECTORY:            return L"IRP_MN_QUERY_DIRECTORY";
                            case kernel::IRP_MN_NOTIFY_CHANGE_DIRECTORY:    return L"IRP_MN_NOTIFY_CHANGE_DIRECTORY";
                            case kernel::IRP_MN_NOTIFY_CHANGE_DIRECTORY_EX: return L"IRP_MN_NOTIFY_CHANGE_DIRECTORY_EX";
                        }
                        break;

                    case kernel::IRP_MJ_FILE_SYSTEM_CONTROL:
                        switch (minor) {
                            case kernel::IRP_MN_USER_FS_REQUEST:  return L"IRP_MN_USER_FS_REQUEST";
                            case kernel::IRP_MN_MOUNT_VOLUME:     return L"IRP_MN_MOUNT_VOLUME";
                            case kernel::IRP_MN_VERIFY_VOLUME:    return L"IRP_MN_VERIFY_VOLUME";
                            case kernel::IRP_MN_LOAD_FILE_SYSTEM: return L"IRP_MN_LOAD_FILE_SYSTEM";
                            case kernel::IRP_MN_KERNEL_CALL:      return L"IRP_MN_KERNEL_CALL";
                        }
                        break;

                    case kernel::IRP_MJ_LOCK_CONTROL:
                        switch (minor) {
                            case kernel::IRP_MN_LOCK:              return L"IRP_MN_LOCK";
                            case kernel::IRP_MN_UNLOCK_SINGLE:     return L"IRP_MN_UNLOCK_SINGLE";
                            case kernel::IRP_MN_UNLOCK_ALL:        return L"IRP_MN_UNLOCK_ALL";
                            case kernel::IRP_MN_UNLOCK_ALL_BY_KEY: return L"IRP_MN_UNLOCK_ALL_BY_KEY";
                        }
                        break;

                    case kernel::IRP_MJ_READ:
                    case kernel::IRP_MJ_WRITE:
                        switch (minor) {
                            case kernel::IRP_MN_NORMAL:           return L"IRP_MN_NORMAL";
                            case kernel::IRP_MN_DPC:              return L"IRP_MN_DPC";
                            case kernel::IRP_MN_MDL:              return L"IRP_MN_MDL";
                            case kernel::IRP_MN_COMPLETE:         return L"IRP_MN_COMPLETE";
                            case kernel::IRP_MN_COMPRESSED:       return L"IRP_MN_COMPRESSED";
                            case kernel::IRP_MN_MDL_DPC:          return L"IRP_MN_MDL_DPC";
                            case kernel::IRP_MN_COMPLETE_MDL:     return L"IRP_MN_COMPLETE_MDL";
                            case kernel::IRP_MN_COMPLETE_MDL_DPC: return L"IRP_MN_COMPLETE_MDL_DPC";
                        }
                        break;

                    case kernel::IRP_MJ_DEVICE_CONTROL:
                    case kernel::IRP_MJ_INTERNAL_DEVICE_CONTROL:
                        switch (minor) {
                            case kernel::IRP_MN_SCSI_CLASS: return L"IRP_MN_SCSI_CLASS";
                        }
                        break;

                    case kernel::IRP_MJ_PNP:
                        switch (minor) {
                            case kernel::IRP_MN_START_DEVICE:                 return L"IRP_MN_START_DEVICE";
                            case kernel::IRP_MN_QUERY_REMOVE_DEVICE:          return L"IRP_MN_QUERY_REMOVE_DEVICE";
                            case kernel::IRP_MN_REMOVE_DEVICE:                return L"IRP_MN_REMOVE_DEVICE";
                            case kernel::IRP_MN_CANCEL_REMOVE_DEVICE:         return L"IRP_MN_CANCEL_REMOVE_DEVICE";
                            case kernel::IRP_MN_STOP_DEVICE:                  return L"IRP_MN_STOP_DEVICE";
                            case kernel::IRP_MN_QUERY_STOP_DEVICE:            return L"IRP_MN_QUERY_STOP_DEVICE";
                            case kernel::IRP_MN_CANCEL_STOP_DEVICE:           return L"IRP_MN_CANCEL_STOP_DEVICE";
                            case kernel::IRP_MN_QUERY_DEVICE_RELATIONS:       return L"IRP_MN_QUERY_DEVICE_RELATIONS";
                            case kernel::IRP_MN_QUERY_INTERFACE:              return L"IRP_MN_QUERY_INTERFACE";
                            case kernel::IRP_MN_QUERY_CAPABILITIES:           return L"IRP_MN_QUERY_CAPABILITIES";
                            case kernel::IRP_MN_QUERY_RESOURCES:              return L"IRP_MN_QUERY_RESOURCES";
                            case kernel::IRP_MN_QUERY_RESOURCE_REQUIREMENTS:  return L"IRP_MN_QUERY_RESOURCE_REQUIREMENTS";
                            case kernel::IRP_MN_QUERY_DEVICE_TEXT:            return L"IRP_MN_QUERY_DEVICE_TEXT";
                            case kernel::IRP_MN_FILTER_RESOURCE_REQUIREMENTS: return L"IRP_MN_FILTER_RESOURCE_REQUIREMENTS";
                            case kernel::IRP_MN_READ_CONFIG:                  return L"IRP_MN_READ_CONFIG";
                            case kernel::IRP_MN_WRITE_CONFIG:                 return L"IRP_MN_WRITE_CONFIG";
                            case kernel::IRP_MN_EJECT:                        return L"IRP_MN_EJECT";
                            case kernel::IRP_MN_SET_LOCK:                     return L"IRP_MN_SET_LOCK";
                            case kernel::IRP_MN_QUERY_ID:                     return L"IRP_MN_QUERY_ID";
                            case kernel::IRP_MN_QUERY_PNP_DEVICE_STATE:       return L"IRP_MN_QUERY_PNP_DEVICE_STATE";
                            case kernel::IRP_MN_QUERY_BUS_INFORMATION:        return L"IRP_MN_QUERY_BUS_INFORMATION";
                            case kernel::IRP_MN_DEVICE_USAGE_NOTIFICATION:    return L"IRP_MN_DEVICE_USAGE_NOTIFICATION";
                            case kernel::IRP_MN_SURPRISE_REMOVAL:             return L"IRP_MN_SURPRISE_REMOVAL";
                            case kernel::IRP_MN_QUERY_LEGACY_BUS_INFORMATION: return L"IRP_MN_QUERY_LEGACY_BUS_INFORMATION";
                        }
                        break;

                    case kernel::IRP_MJ_POWER:
                        switch (minor) {
                            case kernel::IRP_MN_WAIT_WAKE:      return L"IRP_MN_WAIT_WAKE";
                            case kernel::IRP_MN_POWER_SEQUENCE: return L"IRP_MN_POWER_SEQUENCE";
                            case kernel::IRP_MN_SET_POWER:      return L"IRP_MN_SET_POWER";
                            case kernel::IRP_MN_QUERY_POWER:    return L"IRP_MN_QUERY_POWER";
                        }
                        break;

                    case kernel::IRP_MJ_SYSTEM_CONTROL:
                        switch (minor) {
                            case kernel::IRP_MN_QUERY_ALL_DATA:         return L"IRP_MN_QUERY_ALL_DATA";
                            case kernel::IRP_MN_QUERY_SINGLE_INSTANCE:  return L"IRP_MN_QUERY_SINGLE_INSTANCE";
                            case kernel::IRP_MN_CHANGE_SINGLE_INSTANCE: return L"IRP_MN_CHANGE_SINGLE_INSTANCE";
                            case kernel::IRP_MN_CHANGE_SINGLE_ITEM:     return L"IRP_MN_CHANGE_SINGLE_ITEM";
                            case kernel::IRP_MN_ENABLE_EVENTS:          return L"IRP_MN_ENABLE_EVENTS";
                            case kernel::IRP_MN_DISABLE_EVENTS:         return L"IRP_MN_DISABLE_EVENTS";
                            case kernel::IRP_MN_ENABLE_COLLECTION:      return L"IRP_MN_ENABLE_COLLECTION";
                            case kernel::IRP_MN_DISABLE_COLLECTION:     return L"IRP_MN_DISABLE_COLLECTION";
                            case kernel::IRP_MN_REGINFO:                return L"IRP_MN_REGINFO";
                            case kernel::IRP_MN_EXECUTE_METHOD:         return L"IRP_MN_EXECUTE_METHOD";
                        }
                        break;
                }

                // a zero minor with no name is an operation that has no minor function
                if (minor == 0u) return L"";

                return std::format(L"{:02X}", static_cast<uint32_t>(minor));
            }


            std::wstring RenderReparseTag(uint32_t tag) {

                if (tag == 0u) return L"";

                switch (tag) {
                    case IO_REPARSE_TAG_HSM2:               return L"HSM2";
                    case IO_REPARSE_TAG_SIS:                return L"SIS";
                    case IO_REPARSE_TAG_WIM:                return L"WIM";
                    case IO_REPARSE_TAG_CSV:                return L"CSV";
                    case IO_REPARSE_TAG_DFS:                return L"DFS";
                    case IO_REPARSE_TAG_DFSR:               return L"DFSR";
                    case IO_REPARSE_TAG_DEDUP:              return L"DEDUP";
                    case IO_REPARSE_TAG_NFS:                return L"NFS";
                    case IO_REPARSE_TAG_WOF:                return L"WOF";
                    case IO_REPARSE_TAG_WCI:                return L"WCI";
                    case IO_REPARSE_TAG_APPEXECLINK:        return L"APPEXECLINK";
                    case IO_REPARSE_TAG_STORAGE_SYNC:       return L"STORAGE_SYNC";
                    case IO_REPARSE_TAG_AF_UNIX:            return L"AF_UNIX";
                    case kernel::IO_REPARSE_TAG_LX_FIFO:    return L"LX_FIFO";
                    case kernel::IO_REPARSE_TAG_LX_CHR:     return L"LX_CHR";
                    case kernel::IO_REPARSE_TAG_LX_BLK:     return L"LX_BLK";
                    case IO_REPARSE_TAG_PROJFS:             return L"PROJFS";
                    case IO_REPARSE_TAG_MOUNT_POINT:        return L"MOUNT_POINT";
                    case IO_REPARSE_TAG_SYMLINK:            return L"SYMLINK";
                    case kernel::IO_REPARSE_TAG_LX_SYMLINK: return L"LX_SYMLINK";
                    case IO_REPARSE_TAG_HSM:                return L"HSM";
                }

                return std::format(L"{:08X}", tag);
            }


            std::wstring RenderTransactionNotify(uint32_t notification) {

                switch (notification) {
                    case TRANSACTION_NOTIFY_PREPREPARE:          return L"PREPREPARE";
                    case TRANSACTION_NOTIFY_PREPARE:             return L"PREPARE";
                    case TRANSACTION_NOTIFY_COMMIT:              return L"COMMIT";
                    case TRANSACTION_NOTIFY_ROLLBACK:            return L"ROLLBACK";
                    case TRANSACTION_NOTIFY_PREPREPARE_COMPLETE: return L"PREPREPARE_COMPLETE";
                    case TRANSACTION_NOTIFY_PREPARE_COMPLETE:    return L"PREPARE_COMPLETE";
                    case TRANSACTION_NOTIFY_COMMIT_COMPLETE:     return L"COMMIT_COMPLETE";
                    case TRANSACTION_NOTIFY_ROLLBACK_COMPLETE:   return L"ROLLBACK_COMPLETE";
                    case TRANSACTION_NOTIFY_RECOVER:             return L"RECOVER";
                    case TRANSACTION_NOTIFY_SINGLE_PHASE_COMMIT: return L"SINGLE_PHASE_COMMIT";
                    case TRANSACTION_NOTIFY_DELEGATE_COMMIT:     return L"DELEGATE_COMMIT";
                    case TRANSACTION_NOTIFY_RECOVER_QUERY:       return L"RECOVER_QUERY";
                    case TRANSACTION_NOTIFY_ENLIST_PREPREPARE:   return L"ENLIST_PREPREPARE";
                    case TRANSACTION_NOTIFY_LAST_RECOVER:        return L"LAST_RECOVER";
                    case TRANSACTION_NOTIFY_INDOUBT:             return L"INDOUBT";
                    case TRANSACTION_NOTIFY_PROPAGATE_PULL:      return L"PROPAGATE_PULL";
                    case TRANSACTION_NOTIFY_PROPAGATE_PUSH:      return L"PROPAGATE_PUSH";
                    case TRANSACTION_NOTIFY_MARSHAL:             return L"MARSHAL";
                    case TRANSACTION_NOTIFY_RM_DISCONNECTED:     return L"RM_DISCONNECTED";
                    case TRANSACTION_NOTIFY_TM_ONLINE:           return L"TM_ONLINE";
                    case TRANSACTION_NOTIFY_COMMIT_REQUEST:      return L"COMMIT_REQUEST";
                    case TRANSACTION_NOTIFY_PROMOTE:             return L"PROMOTE";
                    case TRANSACTION_NOTIFY_PROMOTE_NEW:         return L"PROMOTE_NEW";
                    case TRANSACTION_NOTIFY_REQUEST_OUTCOME:     return L"REQUEST_OUTCOME";
                    case TRANSACTION_NOTIFY_COMMIT_FINALIZE:     return L"COMMIT_FINALIZE";
                }

                return std::format(L"{:08X}", notification);
            }


            std::wstring RenderCreateDisposition(uint32_t disposition) {

                switch (disposition) {
                    case kernel::FILE_SUPERSEDE:    return L"Supersede";
                    case kernel::FILE_OPEN:         return L"Open";
                    case kernel::FILE_CREATE:       return L"Create";
                    case kernel::FILE_OPEN_IF:      return L"OpenIf";
                    case kernel::FILE_OVERWRITE:    return L"Overwrite";
                    case kernel::FILE_OVERWRITE_IF: return L"OverwriteIf";
                }

                return std::to_wstring(disposition);
            }


            std::wstring RenderOpenResult(uint64_t information) {

                switch (information) {
                    case kernel::FILE_SUPERSEDED:     return L"Superseded";
                    case kernel::FILE_OPENED:         return L"Opened";
                    case kernel::FILE_CREATED:        return L"Created";
                    case kernel::FILE_OVERWRITTEN:    return L"Overwritten";
                    case kernel::FILE_EXISTS:         return L"Exists";
                    case kernel::FILE_DOES_NOT_EXIST: return L"Does Not Exist";
                }

                return L"";
            }


            std::wstring RenderDesiredAccess(uint32_t desiredAccess) {

                return RenderFlags(desiredAccess, DESIRED_ACCESS_NAMES, L"|");
            }


            std::wstring RenderCreateOptions(uint32_t createOptions) {

                return RenderFlags(createOptions, CREATE_OPTION_NAMES, L"|");
            }


            std::wstring RenderShareAccess(uint32_t shareAccess) {

                if (!shareAccess) return L"None";

                return RenderFlags(shareAccess, SHARE_ACCESS_NAMES, L"|");
            }


            std::wstring RenderFileAttributes(uint32_t fileAttributes) {

                if (!fileAttributes) return L"n/a";

                return RenderFlags(fileAttributes, FILE_ATTRIBUTE_LETTERS, L"");
            }


            std::wstring RenderFileInformationClass(uint32_t fileInformationClass) {

                switch (fileInformationClass) {
                    case kernel::FileDirectoryInformation:                     return L"FileDirectoryInformation";
                    case kernel::FileFullDirectoryInformation:                 return L"FileFullDirectoryInformation";
                    case kernel::FileBothDirectoryInformation:                 return L"FileBothDirectoryInformation";
                    case kernel::FileBasicInformation:                         return L"FileBasicInformation";
                    case kernel::FileStandardInformation:                      return L"FileStandardInformation";
                    case kernel::FileInternalInformation:                      return L"FileInternalInformation";
                    case kernel::FileEaInformation:                            return L"FileEaInformation";
                    case kernel::FileAccessInformation:                        return L"FileAccessInformation";
                    case kernel::FileNameInformation:                          return L"FileNameInformation";
                    case kernel::FileRenameInformation:                        return L"FileRenameInformation";
                    case kernel::FileLinkInformation:                          return L"FileLinkInformation";
                    case kernel::FileNamesInformation:                         return L"FileNamesInformation";
                    case kernel::FileDispositionInformation:                   return L"FileDispositionInformation";
                    case kernel::FilePositionInformation:                      return L"FilePositionInformation";
                    case kernel::FileFullEaInformation:                        return L"FileFullEaInformation";
                    case kernel::FileModeInformation:                          return L"FileModeInformation";
                    case kernel::FileAlignmentInformation:                     return L"FileAlignmentInformation";
                    case kernel::FileAllInformation:                           return L"FileAllInformation";
                    case kernel::FileAllocationInformation:                    return L"FileAllocationInformation";
                    case kernel::FileEndOfFileInformation:                     return L"FileEndOfFileInformation";
                    case kernel::FileAlternateNameInformation:                 return L"FileAlternateNameInformation";
                    case kernel::FileStreamInformation:                        return L"FileStreamInformation";
                    case kernel::FilePipeInformation:                          return L"FilePipeInformation";
                    case kernel::FilePipeLocalInformation:                     return L"FilePipeLocalInformation";
                    case kernel::FilePipeRemoteInformation:                    return L"FilePipeRemoteInformation";
                    case kernel::FileMailslotQueryInformation:                 return L"FileMailslotQueryInformation";
                    case kernel::FileMailslotSetInformation:                   return L"FileMailslotSetInformation";
                    case kernel::FileCompressionInformation:                   return L"FileCompressionInformation";
                    case kernel::FileObjectIdInformation:                      return L"FileObjectIdInformation";
                    case kernel::FileCompletionInformation:                    return L"FileCompletionInformation";
                    case kernel::FileMoveClusterInformation:                   return L"FileMoveClusterInformation";
                    case kernel::FileQuotaInformation:                         return L"FileQuotaInformation";
                    case kernel::FileReparsePointInformation:                  return L"FileReparsePointInformation";
                    case kernel::FileNetworkOpenInformation:                   return L"FileNetworkOpenInformation";
                    case kernel::FileAttributeTagInformation:                  return L"FileAttributeTagInformation";
                    case kernel::FileTrackingInformation:                      return L"FileTrackingInformation";
                    case kernel::FileIdBothDirectoryInformation:               return L"FileIdBothDirectoryInformation";
                    case kernel::FileIdFullDirectoryInformation:               return L"FileIdFullDirectoryInformation";
                    case kernel::FileValidDataLengthInformation:               return L"FileValidDataLengthInformation";
                    case kernel::FileShortNameInformation:                     return L"FileShortNameInformation";
                    case kernel::FileIoCompletionNotificationInformation:      return L"FileIoCompletionNotificationInformation";
                    case kernel::FileIoStatusBlockRangeInformation:            return L"FileIoStatusBlockRangeInformation";
                    case kernel::FileIoPriorityHintInformation:                return L"FileIoPriorityHintInformation";
                    case kernel::FileSfioReserveInformation:                   return L"FileSfioReserveInformation";
                    case kernel::FileSfioVolumeInformation:                    return L"FileSfioVolumeInformation";
                    case kernel::FileHardLinkInformation:                      return L"FileHardLinkInformation";
                    case kernel::FileProcessIdsUsingFileInformation:           return L"FileProcessIdsUsingFileInformation";
                    case kernel::FileNormalizedNameInformation:                return L"FileNormalizedNameInformation";
                    case kernel::FileNetworkPhysicalNameInformation:           return L"FileNetworkPhysicalNameInformation";
                    case kernel::FileIdGlobalTxDirectoryInformation:           return L"FileIdGlobalTxDirectoryInformation";
                    case kernel::FileIsRemoteDeviceInformation:                return L"FileIsRemoteDeviceInformation";
                    case kernel::FileUnusedInformation:                        return L"FileUnusedInformation";
                    case kernel::FileNumaNodeInformation:                      return L"FileNumaNodeInformation";
                    case kernel::FileStandardLinkInformation:                  return L"FileStandardLinkInformation";
                    case kernel::FileRemoteProtocolInformation:                return L"FileRemoteProtocolInformation";
                    case kernel::FileRenameInformationBypassAccessCheck:       return L"FileRenameInformationBypassAccessCheck";
                    case kernel::FileLinkInformationBypassAccessCheck:         return L"FileLinkInformationBypassAccessCheck";
                    case kernel::FileVolumeNameInformation:                    return L"FileVolumeNameInformation";
                    case kernel::FileIdInformation:                            return L"FileIdInformation";
                    case kernel::FileIdExtdDirectoryInformation:               return L"FileIdExtdDirectoryInformation";
                    case kernel::FileReplaceCompletionInformation:             return L"FileReplaceCompletionInformation";
                    case kernel::FileHardLinkFullIdInformation:                return L"FileHardLinkFullIdInformation";
                    case kernel::FileIdExtdBothDirectoryInformation:           return L"FileIdExtdBothDirectoryInformation";
                    case kernel::FileDispositionInformationEx:                 return L"FileDispositionInformationEx";
                    case kernel::FileRenameInformationEx:                      return L"FileRenameInformationEx";
                    case kernel::FileRenameInformationExBypassAccessCheck:     return L"FileRenameInformationExBypassAccessCheck";
                    case kernel::FileDesiredStorageClassInformation:           return L"FileDesiredStorageClassInformation";
                    case kernel::FileStatInformation:                          return L"FileStatInformation";
                    case kernel::FileMemoryPartitionInformation:               return L"FileMemoryPartitionInformation";
                    case kernel::FileStatLxInformation:                        return L"FileStatLxInformation";
                    case kernel::FileCaseSensitiveInformation:                 return L"FileCaseSensitiveInformation";
                    case kernel::FileLinkInformationEx:                        return L"FileLinkInformationEx";
                    case kernel::FileLinkInformationExBypassAccessCheck:       return L"FileLinkInformationExBypassAccessCheck";
                    case kernel::FileStorageReserveIdInformation:              return L"FileStorageReserveIdInformation";
                    case kernel::FileCaseSensitiveInformationForceAccessCheck: return L"FileCaseSensitiveInformationForceAccessCheck";
                    case kernel::FileKnownFolderInformation:                   return L"FileKnownFolderInformation";
                    case kernel::FileStatBasicInformation:                     return L"FileStatBasicInformation";
                    case kernel::FileId64ExtdDirectoryInformation:             return L"FileId64ExtdDirectoryInformation";
                    case kernel::FileId64ExtdBothDirectoryInformation:         return L"FileId64ExtdBothDirectoryInformation";
                    case kernel::FileIdAllExtdDirectoryInformation:            return L"FileIdAllExtdDirectoryInformation";
                    case kernel::FileIdAllExtdBothDirectoryInformation:        return L"FileIdAllExtdBothDirectoryInformation";
                    case kernel::FileStreamReservationInformation:             return L"FileStreamReservationInformation";
                    case kernel::FileMupProviderInfo:                          return L"FileMupProviderInfo";
                }

                return std::to_wstring(fileInformationClass);
            }


            std::wstring RenderCompressionFormat(uint16_t format) {

                switch (format) {
                    case COMPRESSION_FORMAT_NONE:        return L"None";
                    case COMPRESSION_FORMAT_DEFAULT:     return L"Default";
                    case COMPRESSION_FORMAT_LZNT1:       return L"LZNT1";
                    case COMPRESSION_FORMAT_XPRESS:      return L"XPRESS";
                    case COMPRESSION_FORMAT_XPRESS_HUFF: return L"XPRESS_HUFF";
                }

                return std::to_wstring(format);
            }


            std::wstring RenderRemoteProtocol(uint32_t protocol) {

                switch (protocol) {
                    case WNNC_NET_SMB:    return L"SMB";
                    case WNNC_NET_DAV:    return L"DAV";
                    case WNNC_NET_MS_NFS: return L"NFS";
                }

                return std::format(L"0x{:X}", protocol);
            }


            std::wstring RenderRemoteProtocolFlags(uint32_t flags) {

                return RenderFlags(flags, REMOTE_PROTOCOL_FLAG_NAMES, L"|");
            }


            std::wstring RenderDispositionFlags(uint32_t flags) {

                return RenderFlags(flags, DISPOSITION_FLAG_NAMES, L"|");
            }


            std::wstring RenderRenameFlags(uint32_t flags) {

                return RenderFlags(flags, RENAME_FLAG_NAMES, L"|");
            }


            std::wstring RenderFsInformationClass(uint32_t fsInformationClass) {

                switch (fsInformationClass) {
                    case kernel::FileFsVolumeInformation:       return L"FileFsVolumeInformation";
                    case kernel::FileFsLabelInformation:        return L"FileFsLabelInformation";
                    case kernel::FileFsSizeInformation:         return L"FileFsSizeInformation";
                    case kernel::FileFsDeviceInformation:       return L"FileFsDeviceInformation";
                    case kernel::FileFsAttributeInformation:    return L"FileFsAttributeInformation";
                    case kernel::FileFsControlInformation:      return L"FileFsControlInformation";
                    case kernel::FileFsFullSizeInformation:     return L"FileFsFullSizeInformation";
                    case kernel::FileFsObjectIdInformation:     return L"FileFsObjectIdInformation";
                    case kernel::FileFsDriverPathInformation:   return L"FileFsDriverPathInformation";
                    case kernel::FileFsVolumeFlagsInformation:  return L"FileFsVolumeFlagsInformation";
                    case kernel::FileFsSectorSizeInformation:   return L"FileFsSectorSizeInformation";
                    case kernel::FileFsDataCopyInformation:     return L"FileFsDataCopyInformation";
                    case kernel::FileFsMetadataSizeInformation: return L"FileFsMetadataSizeInformation";
                    case kernel::FileFsFullSizeInformationEx:   return L"FileFsFullSizeInformationEx";
                    case kernel::FileFsGuidInformation:         return L"FileFsGuidInformation";
                }

                return std::to_wstring(fsInformationClass);
            }


            std::wstring RenderFileSystemAttributes(uint32_t fileSystemAttributes) {

                if (!fileSystemAttributes) return L"None";

                return RenderFlags(fileSystemAttributes, FILE_SYSTEM_ATTRIBUTE_NAMES, L"|");
            }


            std::wstring RenderQueryDirectoryFlags(uint8_t operationFlags) {

                return RenderFlags(operationFlags, QUERY_DIRECTORY_FLAG_NAMES, L"|");
            }


            std::wstring RenderCompletionFilter(uint32_t completionFilter) {

                return RenderFlags(completionFilter, COMPLETION_FILTER_NAMES, L"|");
            }


            std::wstring RenderDirectoryNotifyInformationClass(uint32_t directoryNotifyInformationClass) {

                switch (directoryNotifyInformationClass) {
                    case kernel::DirectoryNotifyInformation:         return L"DirectoryNotifyInformation";
                    case kernel::DirectoryNotifyExtendedInformation: return L"DirectoryNotifyExtendedInformation";
                    case kernel::DirectoryNotifyFullInformation:     return L"DirectoryNotifyFullInformation";
                }

                return std::to_wstring(directoryNotifyInformationClass);
            }


            std::wstring RenderFsControlCode(uint32_t fsControlCode) {

                switch (fsControlCode) {
                    case FSCTL_REQUEST_OPLOCK_LEVEL_1:                           return L"FSCTL_REQUEST_OPLOCK_LEVEL_1";
                    case FSCTL_REQUEST_OPLOCK_LEVEL_2:                           return L"FSCTL_REQUEST_OPLOCK_LEVEL_2";
                    case FSCTL_REQUEST_BATCH_OPLOCK:                             return L"FSCTL_REQUEST_BATCH_OPLOCK";
                    case FSCTL_OPLOCK_BREAK_ACKNOWLEDGE:                         return L"FSCTL_OPLOCK_BREAK_ACKNOWLEDGE";
                    case FSCTL_OPBATCH_ACK_CLOSE_PENDING:                        return L"FSCTL_OPBATCH_ACK_CLOSE_PENDING";
                    case FSCTL_OPLOCK_BREAK_NOTIFY:                              return L"FSCTL_OPLOCK_BREAK_NOTIFY";
                    case FSCTL_LOCK_VOLUME:                                      return L"FSCTL_LOCK_VOLUME";
                    case FSCTL_UNLOCK_VOLUME:                                    return L"FSCTL_UNLOCK_VOLUME";
                    case FSCTL_DISMOUNT_VOLUME:                                  return L"FSCTL_DISMOUNT_VOLUME";
                    case FSCTL_IS_VOLUME_MOUNTED:                                return L"FSCTL_IS_VOLUME_MOUNTED";
                    case FSCTL_IS_PATHNAME_VALID:                                return L"FSCTL_IS_PATHNAME_VALID";
                    case FSCTL_MARK_VOLUME_DIRTY:                                return L"FSCTL_MARK_VOLUME_DIRTY";
                    case FSCTL_QUERY_RETRIEVAL_POINTERS:                         return L"FSCTL_QUERY_RETRIEVAL_POINTERS";
                    case FSCTL_GET_COMPRESSION:                                  return L"FSCTL_GET_COMPRESSION";
                    case FSCTL_SET_COMPRESSION:                                  return L"FSCTL_SET_COMPRESSION";
                    case FSCTL_SET_BOOTLOADER_ACCESSED:                          return L"FSCTL_SET_BOOTLOADER_ACCESSED";
                    case FSCTL_OPLOCK_BREAK_ACK_NO_2:                            return L"FSCTL_OPLOCK_BREAK_ACK_NO_2";
                    case FSCTL_INVALIDATE_VOLUMES:                               return L"FSCTL_INVALIDATE_VOLUMES";
                    case FSCTL_QUERY_FAT_BPB:                                    return L"FSCTL_QUERY_FAT_BPB";
                    case FSCTL_REQUEST_FILTER_OPLOCK:                            return L"FSCTL_REQUEST_FILTER_OPLOCK";
                    case FSCTL_FILESYSTEM_GET_STATISTICS:                        return L"FSCTL_FILESYSTEM_GET_STATISTICS";
                    case FSCTL_GET_NTFS_VOLUME_DATA:                             return L"FSCTL_GET_NTFS_VOLUME_DATA";
                    case FSCTL_GET_NTFS_FILE_RECORD:                             return L"FSCTL_GET_NTFS_FILE_RECORD";
                    case FSCTL_GET_VOLUME_BITMAP:                                return L"FSCTL_GET_VOLUME_BITMAP";
                    case FSCTL_GET_RETRIEVAL_POINTERS:                           return L"FSCTL_GET_RETRIEVAL_POINTERS";
                    case FSCTL_MOVE_FILE:                                        return L"FSCTL_MOVE_FILE";
                    case FSCTL_IS_VOLUME_DIRTY:                                  return L"FSCTL_IS_VOLUME_DIRTY";
                    case FSCTL_ALLOW_EXTENDED_DASD_IO:                           return L"FSCTL_ALLOW_EXTENDED_DASD_IO";
                    case FSCTL_FIND_FILES_BY_SID:                                return L"FSCTL_FIND_FILES_BY_SID";
                    case FSCTL_SET_OBJECT_ID:                                    return L"FSCTL_SET_OBJECT_ID";
                    case FSCTL_GET_OBJECT_ID:                                    return L"FSCTL_GET_OBJECT_ID";
                    case FSCTL_DELETE_OBJECT_ID:                                 return L"FSCTL_DELETE_OBJECT_ID";
                    case FSCTL_SET_REPARSE_POINT:                                return L"FSCTL_SET_REPARSE_POINT";
                    case FSCTL_GET_REPARSE_POINT:                                return L"FSCTL_GET_REPARSE_POINT";
                    case FSCTL_DELETE_REPARSE_POINT:                             return L"FSCTL_DELETE_REPARSE_POINT";
                    case FSCTL_ENUM_USN_DATA:                                    return L"FSCTL_ENUM_USN_DATA";
                    case FSCTL_SECURITY_ID_CHECK:                                return L"FSCTL_SECURITY_ID_CHECK";
                    case FSCTL_READ_USN_JOURNAL:                                 return L"FSCTL_READ_USN_JOURNAL";
                    case FSCTL_SET_OBJECT_ID_EXTENDED:                           return L"FSCTL_SET_OBJECT_ID_EXTENDED";
                    case FSCTL_CREATE_OR_GET_OBJECT_ID:                          return L"FSCTL_CREATE_OR_GET_OBJECT_ID";
                    case FSCTL_SET_SPARSE:                                       return L"FSCTL_SET_SPARSE";
                    case FSCTL_SET_ZERO_DATA:                                    return L"FSCTL_SET_ZERO_DATA";
                    case FSCTL_QUERY_ALLOCATED_RANGES:                           return L"FSCTL_QUERY_ALLOCATED_RANGES";
                    case FSCTL_ENABLE_UPGRADE:                                   return L"FSCTL_ENABLE_UPGRADE";
                    case FSCTL_SET_ENCRYPTION:                                   return L"FSCTL_SET_ENCRYPTION";
                    case FSCTL_ENCRYPTION_FSCTL_IO:                              return L"FSCTL_ENCRYPTION_FSCTL_IO";
                    case FSCTL_WRITE_RAW_ENCRYPTED:                              return L"FSCTL_WRITE_RAW_ENCRYPTED";
                    case FSCTL_READ_RAW_ENCRYPTED:                               return L"FSCTL_READ_RAW_ENCRYPTED";
                    case FSCTL_CREATE_USN_JOURNAL:                               return L"FSCTL_CREATE_USN_JOURNAL";
                    case FSCTL_READ_FILE_USN_DATA:                               return L"FSCTL_READ_FILE_USN_DATA";
                    case FSCTL_WRITE_USN_CLOSE_RECORD:                           return L"FSCTL_WRITE_USN_CLOSE_RECORD";
                    case FSCTL_EXTEND_VOLUME:                                    return L"FSCTL_EXTEND_VOLUME";
                    case FSCTL_QUERY_USN_JOURNAL:                                return L"FSCTL_QUERY_USN_JOURNAL";
                    case FSCTL_DELETE_USN_JOURNAL:                               return L"FSCTL_DELETE_USN_JOURNAL";
                    case FSCTL_MARK_HANDLE:                                      return L"FSCTL_MARK_HANDLE";
                    case FSCTL_SIS_COPYFILE:                                     return L"FSCTL_SIS_COPYFILE";
                    case FSCTL_SIS_LINK_FILES:                                   return L"FSCTL_SIS_LINK_FILES";
                    case FSCTL_RECALL_FILE:                                      return L"FSCTL_RECALL_FILE";
                    case FSCTL_READ_FROM_PLEX:                                   return L"FSCTL_READ_FROM_PLEX";
                    case FSCTL_FILE_PREFETCH:                                    return L"FSCTL_FILE_PREFETCH";
                    case FSCTL_MAKE_MEDIA_COMPATIBLE:                            return L"FSCTL_MAKE_MEDIA_COMPATIBLE";
                    case FSCTL_SET_DEFECT_MANAGEMENT:                            return L"FSCTL_SET_DEFECT_MANAGEMENT";
                    case FSCTL_QUERY_SPARING_INFO:                               return L"FSCTL_QUERY_SPARING_INFO";
                    case FSCTL_QUERY_ON_DISK_VOLUME_INFO:                        return L"FSCTL_QUERY_ON_DISK_VOLUME_INFO";
                    case FSCTL_SET_VOLUME_COMPRESSION_STATE:                     return L"FSCTL_SET_VOLUME_COMPRESSION_STATE";
                    case FSCTL_TXFS_MODIFY_RM:                                   return L"FSCTL_TXFS_MODIFY_RM";
                    case FSCTL_TXFS_QUERY_RM_INFORMATION:                        return L"FSCTL_TXFS_QUERY_RM_INFORMATION";
                    case FSCTL_TXFS_ROLLFORWARD_REDO:                            return L"FSCTL_TXFS_ROLLFORWARD_REDO";
                    case FSCTL_TXFS_ROLLFORWARD_UNDO:                            return L"FSCTL_TXFS_ROLLFORWARD_UNDO";
                    case FSCTL_TXFS_START_RM:                                    return L"FSCTL_TXFS_START_RM";
                    case FSCTL_TXFS_SHUTDOWN_RM:                                 return L"FSCTL_TXFS_SHUTDOWN_RM";
                    case FSCTL_TXFS_READ_BACKUP_INFORMATION:                     return L"FSCTL_TXFS_READ_BACKUP_INFORMATION";
                    case FSCTL_TXFS_WRITE_BACKUP_INFORMATION:                    return L"FSCTL_TXFS_WRITE_BACKUP_INFORMATION";
                    case FSCTL_TXFS_CREATE_SECONDARY_RM:                         return L"FSCTL_TXFS_CREATE_SECONDARY_RM";
                    case FSCTL_TXFS_GET_METADATA_INFO:                           return L"FSCTL_TXFS_GET_METADATA_INFO";
                    case FSCTL_TXFS_GET_TRANSACTED_VERSION:                      return L"FSCTL_TXFS_GET_TRANSACTED_VERSION";
                    case FSCTL_TXFS_SAVEPOINT_INFORMATION:                       return L"FSCTL_TXFS_SAVEPOINT_INFORMATION";
                    case FSCTL_TXFS_CREATE_MINIVERSION:                          return L"FSCTL_TXFS_CREATE_MINIVERSION";
                    case FSCTL_TXFS_TRANSACTION_ACTIVE:                          return L"FSCTL_TXFS_TRANSACTION_ACTIVE";
                    case FSCTL_SET_ZERO_ON_DEALLOCATION:                         return L"FSCTL_SET_ZERO_ON_DEALLOCATION";
                    case FSCTL_SET_REPAIR:                                       return L"FSCTL_SET_REPAIR";
                    case FSCTL_GET_REPAIR:                                       return L"FSCTL_GET_REPAIR";
                    case FSCTL_WAIT_FOR_REPAIR:                                  return L"FSCTL_WAIT_FOR_REPAIR";
                    case FSCTL_INITIATE_REPAIR:                                  return L"FSCTL_INITIATE_REPAIR";
                    case FSCTL_CSC_INTERNAL:                                     return L"FSCTL_CSC_INTERNAL";
                    case FSCTL_SHRINK_VOLUME:                                    return L"FSCTL_SHRINK_VOLUME";
                    case FSCTL_SET_SHORT_NAME_BEHAVIOR:                          return L"FSCTL_SET_SHORT_NAME_BEHAVIOR";
                    case FSCTL_DFSR_SET_GHOST_HANDLE_STATE:                      return L"FSCTL_DFSR_SET_GHOST_HANDLE_STATE";
                    case FSCTL_TXFS_LIST_TRANSACTIONS:                           return L"FSCTL_TXFS_LIST_TRANSACTIONS";
                    case FSCTL_QUERY_PAGEFILE_ENCRYPTION:                        return L"FSCTL_QUERY_PAGEFILE_ENCRYPTION";
                    case FSCTL_RESET_VOLUME_ALLOCATION_HINTS:                    return L"FSCTL_RESET_VOLUME_ALLOCATION_HINTS";
                    case FSCTL_QUERY_DEPENDENT_VOLUME:                           return L"FSCTL_QUERY_DEPENDENT_VOLUME";
                    case FSCTL_SD_GLOBAL_CHANGE:                                 return L"FSCTL_SD_GLOBAL_CHANGE";
                    case FSCTL_TXFS_READ_BACKUP_INFORMATION2:                    return L"FSCTL_TXFS_READ_BACKUP_INFORMATION2";
                    case FSCTL_LOOKUP_STREAM_FROM_CLUSTER:                       return L"FSCTL_LOOKUP_STREAM_FROM_CLUSTER";
                    case FSCTL_TXFS_WRITE_BACKUP_INFORMATION2:                   return L"FSCTL_TXFS_WRITE_BACKUP_INFORMATION2";
                    case FSCTL_FILE_TYPE_NOTIFICATION:                           return L"FSCTL_FILE_TYPE_NOTIFICATION";
                    case FSCTL_FILE_LEVEL_TRIM:                                  return L"FSCTL_FILE_LEVEL_TRIM";
                    case FSCTL_GET_BOOT_AREA_INFO:                               return L"FSCTL_GET_BOOT_AREA_INFO";
                    case FSCTL_GET_RETRIEVAL_POINTER_BASE:                       return L"FSCTL_GET_RETRIEVAL_POINTER_BASE";
                    case FSCTL_SET_PERSISTENT_VOLUME_STATE:                      return L"FSCTL_SET_PERSISTENT_VOLUME_STATE";
                    case FSCTL_QUERY_PERSISTENT_VOLUME_STATE:                    return L"FSCTL_QUERY_PERSISTENT_VOLUME_STATE";
                    case FSCTL_REQUEST_OPLOCK:                                   return L"FSCTL_REQUEST_OPLOCK";
                    case FSCTL_CSV_TUNNEL_REQUEST:                               return L"FSCTL_CSV_TUNNEL_REQUEST";
                    case FSCTL_IS_CSV_FILE:                                      return L"FSCTL_IS_CSV_FILE";
                    case FSCTL_QUERY_FILE_SYSTEM_RECOGNITION:                    return L"FSCTL_QUERY_FILE_SYSTEM_RECOGNITION";
                    case FSCTL_CSV_GET_VOLUME_PATH_NAME:                         return L"FSCTL_CSV_GET_VOLUME_PATH_NAME";
                    case FSCTL_CSV_GET_VOLUME_NAME_FOR_VOLUME_MOUNT_POINT:       return L"FSCTL_CSV_GET_VOLUME_NAME_FOR_VOLUME_MOUNT_POINT";
                    case FSCTL_CSV_GET_VOLUME_PATH_NAMES_FOR_VOLUME_NAME:        return L"FSCTL_CSV_GET_VOLUME_PATH_NAMES_FOR_VOLUME_NAME";
                    case FSCTL_IS_FILE_ON_CSV_VOLUME:                            return L"FSCTL_IS_FILE_ON_CSV_VOLUME";
                    case FSCTL_CORRUPTION_HANDLING:                              return L"FSCTL_CORRUPTION_HANDLING";
                    case FSCTL_OFFLOAD_READ:                                     return L"FSCTL_OFFLOAD_READ";
                    case FSCTL_OFFLOAD_WRITE:                                    return L"FSCTL_OFFLOAD_WRITE";
                    case FSCTL_CSV_INTERNAL:                                     return L"FSCTL_CSV_INTERNAL";
                    case FSCTL_SET_PURGE_FAILURE_MODE:                           return L"FSCTL_SET_PURGE_FAILURE_MODE";
                    case FSCTL_QUERY_FILE_LAYOUT:                                return L"FSCTL_QUERY_FILE_LAYOUT";
                    case FSCTL_IS_VOLUME_OWNED_BYCSVFS:                          return L"FSCTL_IS_VOLUME_OWNED_BYCSVFS";
                    case FSCTL_GET_INTEGRITY_INFORMATION:                        return L"FSCTL_GET_INTEGRITY_INFORMATION";
                    case FSCTL_SET_INTEGRITY_INFORMATION:                        return L"FSCTL_SET_INTEGRITY_INFORMATION";
                    case FSCTL_QUERY_FILE_REGIONS:                               return L"FSCTL_QUERY_FILE_REGIONS";
                    case FSCTL_RKF_INTERNAL:                                     return L"FSCTL_RKF_INTERNAL";
                    case FSCTL_SCRUB_DATA:                                       return L"FSCTL_SCRUB_DATA";
                    case FSCTL_REPAIR_COPIES:                                    return L"FSCTL_REPAIR_COPIES";
                    case FSCTL_DISABLE_LOCAL_BUFFERING:                          return L"FSCTL_DISABLE_LOCAL_BUFFERING";
                    case FSCTL_CSV_MGMT_LOCK:                                    return L"FSCTL_CSV_MGMT_LOCK";
                    case FSCTL_CSV_QUERY_DOWN_LEVEL_FILE_SYSTEM_CHARACTERISTICS: return L"FSCTL_CSV_QUERY_DOWN_LEVEL_FILE_SYSTEM_CHARACTERISTICS";
                    case FSCTL_ADVANCE_FILE_ID:                                  return L"FSCTL_ADVANCE_FILE_ID";
                    case FSCTL_CSV_SYNC_TUNNEL_REQUEST:                          return L"FSCTL_CSV_SYNC_TUNNEL_REQUEST";
                    case FSCTL_CSV_QUERY_VETO_FILE_DIRECT_IO:                    return L"FSCTL_CSV_QUERY_VETO_FILE_DIRECT_IO";
                    case FSCTL_WRITE_USN_REASON:                                 return L"FSCTL_WRITE_USN_REASON";
                    case FSCTL_CSV_CONTROL:                                      return L"FSCTL_CSV_CONTROL";
                    case FSCTL_GET_REFS_VOLUME_DATA:                             return L"FSCTL_GET_REFS_VOLUME_DATA";
                    case FSCTL_CSV_H_BREAKING_SYNC_TUNNEL_REQUEST:               return L"FSCTL_CSV_H_BREAKING_SYNC_TUNNEL_REQUEST";
                    case FSCTL_QUERY_STORAGE_CLASSES:                            return L"FSCTL_QUERY_STORAGE_CLASSES";
                    case FSCTL_QUERY_REGION_INFO:                                return L"FSCTL_QUERY_REGION_INFO";
                    case FSCTL_USN_TRACK_MODIFIED_RANGES:                        return L"FSCTL_USN_TRACK_MODIFIED_RANGES";
                    case FSCTL_QUERY_SHARED_VIRTUAL_DISK_SUPPORT:                return L"FSCTL_QUERY_SHARED_VIRTUAL_DISK_SUPPORT";
                    case FSCTL_SVHDX_SYNC_TUNNEL_REQUEST:                        return L"FSCTL_SVHDX_SYNC_TUNNEL_REQUEST";
                    case FSCTL_SVHDX_SET_INITIATOR_INFORMATION:                  return L"FSCTL_SVHDX_SET_INITIATOR_INFORMATION";
                    case FSCTL_SET_EXTERNAL_BACKING:                             return L"FSCTL_SET_EXTERNAL_BACKING";
                    case FSCTL_GET_EXTERNAL_BACKING:                             return L"FSCTL_GET_EXTERNAL_BACKING";
                    case FSCTL_DELETE_EXTERNAL_BACKING:                          return L"FSCTL_DELETE_EXTERNAL_BACKING";
                    case FSCTL_ENUM_EXTERNAL_BACKING:                            return L"FSCTL_ENUM_EXTERNAL_BACKING";
                    case FSCTL_ENUM_OVERLAY:                                     return L"FSCTL_ENUM_OVERLAY";
                    case FSCTL_ADD_OVERLAY:                                      return L"FSCTL_ADD_OVERLAY";
                    case FSCTL_REMOVE_OVERLAY:                                   return L"FSCTL_REMOVE_OVERLAY";
                    case FSCTL_UPDATE_OVERLAY:                                   return L"FSCTL_UPDATE_OVERLAY";
                    case FSCTL_SHUFFLE_FILE:                                     return L"FSCTL_SHUFFLE_FILE";
                    case FSCTL_DUPLICATE_EXTENTS_TO_FILE:                        return L"FSCTL_DUPLICATE_EXTENTS_TO_FILE";
                    case FSCTL_SPARSE_OVERALLOCATE:                              return L"FSCTL_SPARSE_OVERALLOCATE";
                    case FSCTL_STORAGE_QOS_CONTROL:                              return L"FSCTL_STORAGE_QOS_CONTROL";
                    case FSCTL_INITIATE_FILE_METADATA_OPTIMIZATION:              return L"FSCTL_INITIATE_FILE_METADATA_OPTIMIZATION";
                    case FSCTL_QUERY_FILE_METADATA_OPTIMIZATION:                 return L"FSCTL_QUERY_FILE_METADATA_OPTIMIZATION";
                    case FSCTL_SVHDX_ASYNC_TUNNEL_REQUEST:                       return L"FSCTL_SVHDX_ASYNC_TUNNEL_REQUEST";
                    case FSCTL_GET_WOF_VERSION:                                  return L"FSCTL_GET_WOF_VERSION";
                    case FSCTL_HCS_SYNC_TUNNEL_REQUEST:                          return L"FSCTL_HCS_SYNC_TUNNEL_REQUEST";
                    case FSCTL_HCS_ASYNC_TUNNEL_REQUEST:                         return L"FSCTL_HCS_ASYNC_TUNNEL_REQUEST";
                    case FSCTL_QUERY_EXTENT_READ_CACHE_INFO:                     return L"FSCTL_QUERY_EXTENT_READ_CACHE_INFO";
                    case FSCTL_QUERY_REFS_VOLUME_COUNTER_INFO:                   return L"FSCTL_QUERY_REFS_VOLUME_COUNTER_INFO";
                    case FSCTL_CLEAN_VOLUME_METADATA:                            return L"FSCTL_CLEAN_VOLUME_METADATA";
                    case FSCTL_SET_INTEGRITY_INFORMATION_EX:                     return L"FSCTL_SET_INTEGRITY_INFORMATION_EX";
                    case FSCTL_SUSPEND_OVERLAY:                                  return L"FSCTL_SUSPEND_OVERLAY";
                    case FSCTL_VIRTUAL_STORAGE_QUERY_PROPERTY:                   return L"FSCTL_VIRTUAL_STORAGE_QUERY_PROPERTY";
                    case FSCTL_FILESYSTEM_GET_STATISTICS_EX:                     return L"FSCTL_FILESYSTEM_GET_STATISTICS_EX";
                    case FSCTL_QUERY_VOLUME_CONTAINER_STATE:                     return L"FSCTL_QUERY_VOLUME_CONTAINER_STATE";
                    case FSCTL_SET_LAYER_ROOT:                                   return L"FSCTL_SET_LAYER_ROOT";
                    case FSCTL_QUERY_DIRECT_ACCESS_EXTENTS:                      return L"FSCTL_QUERY_DIRECT_ACCESS_EXTENTS";
                    case FSCTL_NOTIFY_STORAGE_SPACE_ALLOCATION:                  return L"FSCTL_NOTIFY_STORAGE_SPACE_ALLOCATION";
                    case FSCTL_SSDI_STORAGE_REQUEST:                             return L"FSCTL_SSDI_STORAGE_REQUEST";
                    case FSCTL_QUERY_DIRECT_IMAGE_ORIGINAL_BASE:                 return L"FSCTL_QUERY_DIRECT_IMAGE_ORIGINAL_BASE";
                    case FSCTL_READ_UNPRIVILEGED_USN_JOURNAL:                    return L"FSCTL_READ_UNPRIVILEGED_USN_JOURNAL";
                    case FSCTL_GHOST_FILE_EXTENTS:                               return L"FSCTL_GHOST_FILE_EXTENTS";
                    case FSCTL_QUERY_GHOSTED_FILE_EXTENTS:                       return L"FSCTL_QUERY_GHOSTED_FILE_EXTENTS";
                    case FSCTL_UNMAP_SPACE:                                      return L"FSCTL_UNMAP_SPACE";
                    case FSCTL_HCS_SYNC_NO_WRITE_TUNNEL_REQUEST:                 return L"FSCTL_HCS_SYNC_NO_WRITE_TUNNEL_REQUEST";
                    case FSCTL_START_VIRTUALIZATION_INSTANCE:                    return L"FSCTL_START_VIRTUALIZATION_INSTANCE";
                    case FSCTL_STREAMS_QUERY_PARAMETERS:                         return L"FSCTL_STREAMS_QUERY_PARAMETERS";
                    case FSCTL_STREAMS_ASSOCIATE_ID:                             return L"FSCTL_STREAMS_ASSOCIATE_ID";
                    case FSCTL_STREAMS_QUERY_ID:                                 return L"FSCTL_STREAMS_QUERY_ID";
                    case FSCTL_GET_RETRIEVAL_POINTERS_AND_REFCOUNT:              return L"FSCTL_GET_RETRIEVAL_POINTERS_AND_REFCOUNT";
                    case FSCTL_QUERY_VOLUME_NUMA_INFO:                           return L"FSCTL_QUERY_VOLUME_NUMA_INFO";
                    case FSCTL_REFS_DEALLOCATE_RANGES:                           return L"FSCTL_REFS_DEALLOCATE_RANGES";
                    case FSCTL_QUERY_REFS_SMR_VOLUME_INFO:                       return L"FSCTL_QUERY_REFS_SMR_VOLUME_INFO";
                    case FSCTL_SET_REFS_SMR_VOLUME_GC_PARAMETERS:                return L"FSCTL_SET_REFS_SMR_VOLUME_GC_PARAMETERS";
                    case FSCTL_SET_REFS_FILE_STRICTLY_SEQUENTIAL:                return L"FSCTL_SET_REFS_FILE_STRICTLY_SEQUENTIAL";
                    case FSCTL_DUPLICATE_EXTENTS_TO_FILE_EX:                     return L"FSCTL_DUPLICATE_EXTENTS_TO_FILE_EX";
                    case FSCTL_QUERY_BAD_RANGES:                                 return L"FSCTL_QUERY_BAD_RANGES";
                    case FSCTL_SET_DAX_ALLOC_ALIGNMENT_HINT:                     return L"FSCTL_SET_DAX_ALLOC_ALIGNMENT_HINT";
                    case FSCTL_DELETE_CORRUPTED_REFS_CONTAINER:                  return L"FSCTL_DELETE_CORRUPTED_REFS_CONTAINER";
                    case FSCTL_SCRUB_UNDISCOVERABLE_ID:                          return L"FSCTL_SCRUB_UNDISCOVERABLE_ID";
                    case FSCTL_NOTIFY_DATA_CHANGE:                               return L"FSCTL_NOTIFY_DATA_CHANGE";
                    case FSCTL_START_VIRTUALIZATION_INSTANCE_EX:                 return L"FSCTL_START_VIRTUALIZATION_INSTANCE_EX";
                    case FSCTL_ENCRYPTION_KEY_CONTROL:                           return L"FSCTL_ENCRYPTION_KEY_CONTROL";
                    case FSCTL_VIRTUAL_STORAGE_SET_BEHAVIOR:                     return L"FSCTL_VIRTUAL_STORAGE_SET_BEHAVIOR";
                    case FSCTL_SET_REPARSE_POINT_EX:                             return L"FSCTL_SET_REPARSE_POINT_EX";
                    case FSCTL_REARRANGE_FILE:                                   return L"FSCTL_REARRANGE_FILE";
                    case FSCTL_VIRTUAL_STORAGE_PASSTHROUGH:                      return L"FSCTL_VIRTUAL_STORAGE_PASSTHROUGH";
                    case FSCTL_GET_RETRIEVAL_POINTER_COUNT:                      return L"FSCTL_GET_RETRIEVAL_POINTER_COUNT";
                    case FSCTL_QUERY_ASYNC_DUPLICATE_EXTENTS_STATUS:             return L"FSCTL_QUERY_ASYNC_DUPLICATE_EXTENTS_STATUS";
                    case FSCTL_SMB_SHARE_FLUSH_AND_PURGE:                        return L"FSCTL_SMB_SHARE_FLUSH_AND_PURGE";
                    case FSCTL_REFS_STREAM_SNAPSHOT_MANAGEMENT:                  return L"FSCTL_REFS_STREAM_SNAPSHOT_MANAGEMENT";
                    case FSCTL_MANAGE_BYPASS_IO:                                 return L"FSCTL_MANAGE_BYPASS_IO";
                    case FSCTL_REFS_DEALLOCATE_RANGES_EX:                        return L"FSCTL_REFS_DEALLOCATE_RANGES_EX";
                    case FSCTL_SET_CACHED_RUNS_STATE:                            return L"FSCTL_SET_CACHED_RUNS_STATE";
                    case FSCTL_REFS_SET_VOLUME_COMPRESSION_INFO:                 return L"FSCTL_REFS_SET_VOLUME_COMPRESSION_INFO";
                    case FSCTL_REFS_QUERY_VOLUME_COMPRESSION_INFO:               return L"FSCTL_REFS_QUERY_VOLUME_COMPRESSION_INFO";
                    case FSCTL_DUPLICATE_CLUSTER:                                return L"FSCTL_DUPLICATE_CLUSTER";
                    case FSCTL_CREATE_LCN_WEAK_REFERENCE:                        return L"FSCTL_CREATE_LCN_WEAK_REFERENCE";
                    case FSCTL_CLEAR_LCN_WEAK_REFERENCE:                         return L"FSCTL_CLEAR_LCN_WEAK_REFERENCE";
                    case FSCTL_QUERY_LCN_WEAK_REFERENCE:                         return L"FSCTL_QUERY_LCN_WEAK_REFERENCE";
                    case FSCTL_CLEAR_ALL_LCN_WEAK_REFERENCES:                    return L"FSCTL_CLEAR_ALL_LCN_WEAK_REFERENCES";
                    case FSCTL_REFS_SET_VOLUME_DEDUP_INFO:                       return L"FSCTL_REFS_SET_VOLUME_DEDUP_INFO";
                    case FSCTL_REFS_QUERY_VOLUME_DEDUP_INFO:                     return L"FSCTL_REFS_QUERY_VOLUME_DEDUP_INFO";
                    case FSCTL_LMR_QUERY_INFO:                                   return L"FSCTL_LMR_QUERY_INFO";
                    case FSCTL_REFS_CHECKPOINT_VOLUME:                           return L"FSCTL_REFS_CHECKPOINT_VOLUME";
                    case FSCTL_REFS_QUERY_VOLUME_TOTAL_SHARED_LCNS:              return L"FSCTL_REFS_QUERY_VOLUME_TOTAL_SHARED_LCNS";
                    case FSCTL_UPGRADE_VOLUME:                                   return L"FSCTL_UPGRADE_VOLUME";
                    case FSCTL_REFS_SET_VOLUME_IO_METRICS_INFO:                  return L"FSCTL_REFS_SET_VOLUME_IO_METRICS_INFO";
                    case FSCTL_REFS_QUERY_VOLUME_IO_METRICS_INFO:                return L"FSCTL_REFS_QUERY_VOLUME_IO_METRICS_INFO";
                    case FSCTL_REFS_SET_ROLLBACK_PROTECTION_INFO:                return L"FSCTL_REFS_SET_ROLLBACK_PROTECTION_INFO";
                    case FSCTL_REFS_QUERY_ROLLBACK_PROTECTION_INFO:              return L"FSCTL_REFS_QUERY_ROLLBACK_PROTECTION_INFO";
                    case FSCTL_FILE_SOV_CHECK_RANGE:                             return L"FSCTL_FILE_SOV_CHECK_RANGE";
                    case FSCTL_CASCADES_REFS_SET_FILE_REMOTE:                    return L"FSCTL_CASCADES_REFS_SET_FILE_REMOTE";
                    case FSCTL_CIMFS_QUERY_BACKING_REGION_NAMES:                 return L"FSCTL_CIMFS_QUERY_BACKING_REGION_NAMES";
                    case FSCTL_REFS_VOLUME_ATTESTATION_PREPARE_TO_SIGN:          return L"FSCTL_REFS_VOLUME_ATTESTATION_PREPARE_TO_SIGN";
                    case FSCTL_REFS_VOLUME_ATTESTATION_INJECT_CERTIFICATE:       return L"FSCTL_REFS_VOLUME_ATTESTATION_INJECT_CERTIFICATE";
                    case FSCTL_REFS_VOLUME_ATTESTATION_QUERY_STATUS:             return L"FSCTL_REFS_VOLUME_ATTESTATION_QUERY_STATUS";
                }

                return RenderControlCode(fsControlCode);
            }


            std::wstring RenderIoControlCode(uint32_t ioControlCode) {

                switch (ioControlCode) {
                    case IOCTL_STORAGE_CHECK_VERIFY:                      return L"IOCTL_STORAGE_CHECK_VERIFY";
                    case IOCTL_STORAGE_CHECK_VERIFY2:                     return L"IOCTL_STORAGE_CHECK_VERIFY2";
                    case IOCTL_STORAGE_MEDIA_REMOVAL:                     return L"IOCTL_STORAGE_MEDIA_REMOVAL";
                    case IOCTL_STORAGE_EJECT_MEDIA:                       return L"IOCTL_STORAGE_EJECT_MEDIA";
                    case IOCTL_STORAGE_LOAD_MEDIA:                        return L"IOCTL_STORAGE_LOAD_MEDIA";
                    case IOCTL_STORAGE_LOAD_MEDIA2:                       return L"IOCTL_STORAGE_LOAD_MEDIA2";
                    case IOCTL_STORAGE_RESERVE:                           return L"IOCTL_STORAGE_RESERVE";
                    case IOCTL_STORAGE_RELEASE:                           return L"IOCTL_STORAGE_RELEASE";
                    case IOCTL_STORAGE_FIND_NEW_DEVICES:                  return L"IOCTL_STORAGE_FIND_NEW_DEVICES";
                    case IOCTL_STORAGE_EJECTION_CONTROL:                  return L"IOCTL_STORAGE_EJECTION_CONTROL";
                    case IOCTL_STORAGE_MCN_CONTROL:                       return L"IOCTL_STORAGE_MCN_CONTROL";
                    case IOCTL_STORAGE_GET_MEDIA_TYPES:                   return L"IOCTL_STORAGE_GET_MEDIA_TYPES";
                    case IOCTL_STORAGE_GET_MEDIA_TYPES_EX:                return L"IOCTL_STORAGE_GET_MEDIA_TYPES_EX";
                    case IOCTL_STORAGE_GET_MEDIA_SERIAL_NUMBER:           return L"IOCTL_STORAGE_GET_MEDIA_SERIAL_NUMBER";
                    case IOCTL_STORAGE_GET_HOTPLUG_INFO:                  return L"IOCTL_STORAGE_GET_HOTPLUG_INFO";
                    case IOCTL_STORAGE_SET_HOTPLUG_INFO:                  return L"IOCTL_STORAGE_SET_HOTPLUG_INFO";
                    case IOCTL_STORAGE_GET_SYSTEM_FEATURE_SUPPORT:        return L"IOCTL_STORAGE_GET_SYSTEM_FEATURE_SUPPORT";
                    case IOCTL_STORAGE_RESET_BUS:                         return L"IOCTL_STORAGE_RESET_BUS";
                    case IOCTL_STORAGE_RESET_DEVICE:                      return L"IOCTL_STORAGE_RESET_DEVICE";
                    case IOCTL_STORAGE_BREAK_RESERVATION:                 return L"IOCTL_STORAGE_BREAK_RESERVATION";
                    case IOCTL_STORAGE_PERSISTENT_RESERVE_IN:             return L"IOCTL_STORAGE_PERSISTENT_RESERVE_IN";
                    case IOCTL_STORAGE_PERSISTENT_RESERVE_OUT:            return L"IOCTL_STORAGE_PERSISTENT_RESERVE_OUT";
                    case IOCTL_STORAGE_MINIPORT_PASSTHROUGH_REQUEST:      return L"IOCTL_STORAGE_MINIPORT_PASSTHROUGH_REQUEST";
                    case IOCTL_STORAGE_GET_DEVICE_NUMBER:                 return L"IOCTL_STORAGE_GET_DEVICE_NUMBER";
                    case IOCTL_STORAGE_GET_DEVICE_NUMBER_EX:              return L"IOCTL_STORAGE_GET_DEVICE_NUMBER_EX";
                    case IOCTL_STORAGE_PREDICT_FAILURE:                   return L"IOCTL_STORAGE_PREDICT_FAILURE";
                    case IOCTL_STORAGE_FAILURE_PREDICTION_CONFIG:         return L"IOCTL_STORAGE_FAILURE_PREDICTION_CONFIG";
                    case IOCTL_STORAGE_GET_COUNTERS:                      return L"IOCTL_STORAGE_GET_COUNTERS";
                    case IOCTL_STORAGE_READ_CAPACITY:                     return L"IOCTL_STORAGE_READ_CAPACITY";
                    case IOCTL_STORAGE_GET_DEVICE_TELEMETRY:              return L"IOCTL_STORAGE_GET_DEVICE_TELEMETRY";
                    case IOCTL_STORAGE_DEVICE_TELEMETRY_NOTIFY:           return L"IOCTL_STORAGE_DEVICE_TELEMETRY_NOTIFY";
                    case IOCTL_STORAGE_DEVICE_TELEMETRY_QUERY_CAPS:       return L"IOCTL_STORAGE_DEVICE_TELEMETRY_QUERY_CAPS";
                    case IOCTL_STORAGE_GET_DEVICE_TELEMETRY_RAW:          return L"IOCTL_STORAGE_GET_DEVICE_TELEMETRY_RAW";
                    case IOCTL_STORAGE_SET_TEMPERATURE_THRESHOLD:         return L"IOCTL_STORAGE_SET_TEMPERATURE_THRESHOLD";
                    case IOCTL_STORAGE_PROTOCOL_COMMAND:                  return L"IOCTL_STORAGE_PROTOCOL_COMMAND";
                    case IOCTL_STORAGE_SET_PROPERTY:                      return L"IOCTL_STORAGE_SET_PROPERTY";
                    case IOCTL_STORAGE_QUERY_PROPERTY:                    return L"IOCTL_STORAGE_QUERY_PROPERTY";
                    case IOCTL_STORAGE_MANAGE_DATA_SET_ATTRIBUTES:        return L"IOCTL_STORAGE_MANAGE_DATA_SET_ATTRIBUTES";
                    case IOCTL_STORAGE_GET_LB_PROVISIONING_MAP_RESOURCES: return L"IOCTL_STORAGE_GET_LB_PROVISIONING_MAP_RESOURCES";
                    case IOCTL_STORAGE_REINITIALIZE_MEDIA:                return L"IOCTL_STORAGE_REINITIALIZE_MEDIA";
                    case IOCTL_STORAGE_GET_BC_PROPERTIES:                 return L"IOCTL_STORAGE_GET_BC_PROPERTIES";
                    case IOCTL_STORAGE_ALLOCATE_BC_STREAM:                return L"IOCTL_STORAGE_ALLOCATE_BC_STREAM";
                    case IOCTL_STORAGE_FREE_BC_STREAM:                    return L"IOCTL_STORAGE_FREE_BC_STREAM";
                    case IOCTL_STORAGE_CHECK_PRIORITY_HINT_SUPPORT:       return L"IOCTL_STORAGE_CHECK_PRIORITY_HINT_SUPPORT";
                    case IOCTL_STORAGE_START_DATA_INTEGRITY_CHECK:        return L"IOCTL_STORAGE_START_DATA_INTEGRITY_CHECK";
                    case IOCTL_STORAGE_STOP_DATA_INTEGRITY_CHECK:         return L"IOCTL_STORAGE_STOP_DATA_INTEGRITY_CHECK";
                    case IOCTL_STORAGE_FIRMWARE_GET_INFO:                 return L"IOCTL_STORAGE_FIRMWARE_GET_INFO";
                    case IOCTL_STORAGE_FIRMWARE_DOWNLOAD:                 return L"IOCTL_STORAGE_FIRMWARE_DOWNLOAD";
                    case IOCTL_STORAGE_FIRMWARE_ACTIVATE:                 return L"IOCTL_STORAGE_FIRMWARE_ACTIVATE";
                    case IOCTL_STORAGE_BOOT_PARTITION_DOWNLOAD:           return L"IOCTL_STORAGE_BOOT_PARTITION_DOWNLOAD";
                    case IOCTL_STORAGE_BOOT_PARTITION_ACTIVATE:           return L"IOCTL_STORAGE_BOOT_PARTITION_ACTIVATE";
                    case IOCTL_STORAGE_BOOT_PARTITION_GET_INFO:           return L"IOCTL_STORAGE_BOOT_PARTITION_GET_INFO";
                    case IOCTL_STORAGE_ENABLE_IDLE_POWER:                 return L"IOCTL_STORAGE_ENABLE_IDLE_POWER";
                    case IOCTL_STORAGE_GET_IDLE_POWERUP_REASON:           return L"IOCTL_STORAGE_GET_IDLE_POWERUP_REASON";
                    case IOCTL_STORAGE_POWER_ACTIVE:                      return L"IOCTL_STORAGE_POWER_ACTIVE";
                    case IOCTL_STORAGE_POWER_IDLE:                        return L"IOCTL_STORAGE_POWER_IDLE";
                    case IOCTL_STORAGE_EVENT_NOTIFICATION:                return L"IOCTL_STORAGE_EVENT_NOTIFICATION";
                    case IOCTL_STORAGE_DEVICE_POWER_CAP:                  return L"IOCTL_STORAGE_DEVICE_POWER_CAP";
                    case IOCTL_STORAGE_RPMB_COMMAND:                      return L"IOCTL_STORAGE_RPMB_COMMAND";
                    case IOCTL_STORAGE_ATTRIBUTE_MANAGEMENT:              return L"IOCTL_STORAGE_ATTRIBUTE_MANAGEMENT";
                    case IOCTL_STORAGE_DIAGNOSTIC:                        return L"IOCTL_STORAGE_DIAGNOSTIC";
                    case IOCTL_STORAGE_GET_PHYSICAL_ELEMENT_STATUS:       return L"IOCTL_STORAGE_GET_PHYSICAL_ELEMENT_STATUS";
                    case IOCTL_STORAGE_REMOVE_ELEMENT_AND_TRUNCATE:       return L"IOCTL_STORAGE_REMOVE_ELEMENT_AND_TRUNCATE";
                    case IOCTL_STORAGE_GET_DEVICE_INTERNAL_LOG:           return L"IOCTL_STORAGE_GET_DEVICE_INTERNAL_LOG";
                    case IOCTL_SCM_BUS_GET_LOGICAL_DEVICES:               return L"IOCTL_SCM_BUS_GET_LOGICAL_DEVICES";
                    case IOCTL_SCM_BUS_GET_PHYSICAL_DEVICES:              return L"IOCTL_SCM_BUS_GET_PHYSICAL_DEVICES";
                    case IOCTL_SCM_BUS_GET_REGIONS:                       return L"IOCTL_SCM_BUS_GET_REGIONS";
                    case IOCTL_SCM_BUS_QUERY_PROPERTY:                    return L"IOCTL_SCM_BUS_QUERY_PROPERTY";
                    case IOCTL_SCM_BUS_SET_PROPERTY:                      return L"IOCTL_SCM_BUS_SET_PROPERTY";
                    case IOCTL_SCM_BUS_RUNTIME_FW_ACTIVATE:               return L"IOCTL_SCM_BUS_RUNTIME_FW_ACTIVATE";
                    case IOCTL_SCM_BUS_REFRESH_NAMESPACE:                 return L"IOCTL_SCM_BUS_REFRESH_NAMESPACE";
                    case IOCTL_SCM_LD_GET_INTERLEAVE_SET:                 return L"IOCTL_SCM_LD_GET_INTERLEAVE_SET";
                    case IOCTL_SCM_PD_QUERY_PROPERTY:                     return L"IOCTL_SCM_PD_QUERY_PROPERTY";
                    case IOCTL_SCM_PD_FIRMWARE_DOWNLOAD:                  return L"IOCTL_SCM_PD_FIRMWARE_DOWNLOAD";
                    case IOCTL_SCM_PD_FIRMWARE_ACTIVATE:                  return L"IOCTL_SCM_PD_FIRMWARE_ACTIVATE";
                    case IOCTL_SCM_PD_PASSTHROUGH:                        return L"IOCTL_SCM_PD_PASSTHROUGH";
                    case IOCTL_SCM_PD_UPDATE_MANAGEMENT_STATUS:           return L"IOCTL_SCM_PD_UPDATE_MANAGEMENT_STATUS";
                    case IOCTL_SCM_PD_REINITIALIZE_MEDIA:                 return L"IOCTL_SCM_PD_REINITIALIZE_MEDIA";
                    case IOCTL_SCM_PD_SET_PROPERTY:                       return L"IOCTL_SCM_PD_SET_PROPERTY";
                    case IOCTL_DISK_GET_DRIVE_GEOMETRY:                   return L"IOCTL_DISK_GET_DRIVE_GEOMETRY";
                    case IOCTL_DISK_GET_PARTITION_INFO:                   return L"IOCTL_DISK_GET_PARTITION_INFO";
                    case IOCTL_DISK_SET_PARTITION_INFO:                   return L"IOCTL_DISK_SET_PARTITION_INFO";
                    case IOCTL_DISK_GET_DRIVE_LAYOUT:                     return L"IOCTL_DISK_GET_DRIVE_LAYOUT";
                    case IOCTL_DISK_SET_DRIVE_LAYOUT:                     return L"IOCTL_DISK_SET_DRIVE_LAYOUT";
                    case IOCTL_DISK_VERIFY:                               return L"IOCTL_DISK_VERIFY";
                    case IOCTL_DISK_FORMAT_TRACKS:                        return L"IOCTL_DISK_FORMAT_TRACKS";
                    case IOCTL_DISK_REASSIGN_BLOCKS:                      return L"IOCTL_DISK_REASSIGN_BLOCKS";
                    case IOCTL_DISK_PERFORMANCE:                          return L"IOCTL_DISK_PERFORMANCE";
                    case IOCTL_DISK_IS_WRITABLE:                          return L"IOCTL_DISK_IS_WRITABLE";
                    case IOCTL_DISK_LOGGING:                              return L"IOCTL_DISK_LOGGING";
                    case IOCTL_DISK_FORMAT_TRACKS_EX:                     return L"IOCTL_DISK_FORMAT_TRACKS_EX";
                    case IOCTL_DISK_HISTOGRAM_STRUCTURE:                  return L"IOCTL_DISK_HISTOGRAM_STRUCTURE";
                    case IOCTL_DISK_HISTOGRAM_DATA:                       return L"IOCTL_DISK_HISTOGRAM_DATA";
                    case IOCTL_DISK_HISTOGRAM_RESET:                      return L"IOCTL_DISK_HISTOGRAM_RESET";
                    case IOCTL_DISK_REQUEST_STRUCTURE:                    return L"IOCTL_DISK_REQUEST_STRUCTURE";
                    case IOCTL_DISK_REQUEST_DATA:                         return L"IOCTL_DISK_REQUEST_DATA";
                    case IOCTL_DISK_PERFORMANCE_OFF:                      return L"IOCTL_DISK_PERFORMANCE_OFF";
                    case IOCTL_DISK_CONTROLLER_NUMBER:                    return L"IOCTL_DISK_CONTROLLER_NUMBER";
                    case IOCTL_DISK_GET_PARTITION_INFO_EX:                return L"IOCTL_DISK_GET_PARTITION_INFO_EX";
                    case IOCTL_DISK_SET_PARTITION_INFO_EX:                return L"IOCTL_DISK_SET_PARTITION_INFO_EX";
                    case IOCTL_DISK_GET_DRIVE_LAYOUT_EX:                  return L"IOCTL_DISK_GET_DRIVE_LAYOUT_EX";
                    case IOCTL_DISK_SET_DRIVE_LAYOUT_EX:                  return L"IOCTL_DISK_SET_DRIVE_LAYOUT_EX";
                    case IOCTL_DISK_CREATE_DISK:                          return L"IOCTL_DISK_CREATE_DISK";
                    case IOCTL_DISK_GET_LENGTH_INFO:                      return L"IOCTL_DISK_GET_LENGTH_INFO";
                    case IOCTL_DISK_GET_DRIVE_GEOMETRY_EX:                return L"IOCTL_DISK_GET_DRIVE_GEOMETRY_EX";
                    case IOCTL_DISK_REASSIGN_BLOCKS_EX:                   return L"IOCTL_DISK_REASSIGN_BLOCKS_EX";
                    case IOCTL_DISK_UPDATE_DRIVE_SIZE:                    return L"IOCTL_DISK_UPDATE_DRIVE_SIZE";
                    case IOCTL_DISK_GROW_PARTITION:                       return L"IOCTL_DISK_GROW_PARTITION";
                    case IOCTL_DISK_GET_CACHE_INFORMATION:                return L"IOCTL_DISK_GET_CACHE_INFORMATION";
                    case IOCTL_DISK_SET_CACHE_INFORMATION:                return L"IOCTL_DISK_SET_CACHE_INFORMATION";
                    case IOCTL_DISK_DELETE_DRIVE_LAYOUT:                  return L"IOCTL_DISK_DELETE_DRIVE_LAYOUT";
                    case IOCTL_DISK_UPDATE_PROPERTIES:                    return L"IOCTL_DISK_UPDATE_PROPERTIES";
                    case IOCTL_DISK_FORMAT_DRIVE:                         return L"IOCTL_DISK_FORMAT_DRIVE";
                    case IOCTL_DISK_SENSE_DEVICE:                         return L"IOCTL_DISK_SENSE_DEVICE";
                    case IOCTL_DISK_CHECK_VERIFY:                         return L"IOCTL_DISK_CHECK_VERIFY";
                    case IOCTL_DISK_MEDIA_REMOVAL:                        return L"IOCTL_DISK_MEDIA_REMOVAL";
                    case IOCTL_DISK_EJECT_MEDIA:                          return L"IOCTL_DISK_EJECT_MEDIA";
                    case IOCTL_DISK_LOAD_MEDIA:                           return L"IOCTL_DISK_LOAD_MEDIA";
                    case IOCTL_DISK_RESERVE:                              return L"IOCTL_DISK_RESERVE";
                    case IOCTL_DISK_RELEASE:                              return L"IOCTL_DISK_RELEASE";
                    case IOCTL_DISK_FIND_NEW_DEVICES:                     return L"IOCTL_DISK_FIND_NEW_DEVICES";
                    case IOCTL_DISK_GET_MEDIA_TYPES:                      return L"IOCTL_DISK_GET_MEDIA_TYPES";
                    case IOCTL_DISK_GET_DISK_ATTRIBUTES:                  return L"IOCTL_DISK_GET_DISK_ATTRIBUTES";
                    case IOCTL_DISK_SET_DISK_ATTRIBUTES:                  return L"IOCTL_DISK_SET_DISK_ATTRIBUTES";
                    case IOCTL_DISK_RESET_SNAPSHOT_INFO:                  return L"IOCTL_DISK_RESET_SNAPSHOT_INFO";
                    case IOCTL_CHANGER_GET_PARAMETERS:                    return L"IOCTL_CHANGER_GET_PARAMETERS";
                    case IOCTL_CHANGER_GET_STATUS:                        return L"IOCTL_CHANGER_GET_STATUS";
                    case IOCTL_CHANGER_GET_PRODUCT_DATA:                  return L"IOCTL_CHANGER_GET_PRODUCT_DATA";
                    case IOCTL_CHANGER_SET_ACCESS:                        return L"IOCTL_CHANGER_SET_ACCESS";
                    case IOCTL_CHANGER_GET_ELEMENT_STATUS:                return L"IOCTL_CHANGER_GET_ELEMENT_STATUS";
                    case IOCTL_CHANGER_INITIALIZE_ELEMENT_STATUS:         return L"IOCTL_CHANGER_INITIALIZE_ELEMENT_STATUS";
                    case IOCTL_CHANGER_SET_POSITION:                      return L"IOCTL_CHANGER_SET_POSITION";
                    case IOCTL_CHANGER_EXCHANGE_MEDIUM:                   return L"IOCTL_CHANGER_EXCHANGE_MEDIUM";
                    case IOCTL_CHANGER_MOVE_MEDIUM:                       return L"IOCTL_CHANGER_MOVE_MEDIUM";
                    case IOCTL_CHANGER_REINITIALIZE_TRANSPORT:            return L"IOCTL_CHANGER_REINITIALIZE_TRANSPORT";
                    case IOCTL_CHANGER_QUERY_VOLUME_TAGS:                 return L"IOCTL_CHANGER_QUERY_VOLUME_TAGS";
                    case IOCTL_SERIAL_LSRMST_INSERT:                      return L"IOCTL_SERIAL_LSRMST_INSERT";
                    case IOCTL_SERENUM_EXPOSE_HARDWARE:                   return L"IOCTL_SERENUM_EXPOSE_HARDWARE";
                    case IOCTL_SERENUM_REMOVE_HARDWARE:                   return L"IOCTL_SERENUM_REMOVE_HARDWARE";
                    case IOCTL_SERENUM_PORT_DESC:                         return L"IOCTL_SERENUM_PORT_DESC";
                    case IOCTL_SERENUM_GET_PORT_NAME:                     return L"IOCTL_SERENUM_GET_PORT_NAME";
                    case IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS:            return L"IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS";
                    case IOCTL_VOLUME_ONLINE:                             return L"IOCTL_VOLUME_ONLINE";
                    case IOCTL_VOLUME_OFFLINE:                            return L"IOCTL_VOLUME_OFFLINE";
                    case IOCTL_VOLUME_IS_CLUSTERED:                       return L"IOCTL_VOLUME_IS_CLUSTERED";
                    case IOCTL_VOLUME_GET_GPT_ATTRIBUTES:                 return L"IOCTL_VOLUME_GET_GPT_ATTRIBUTES";
                }

                return RenderControlCode(ioControlCode);
            }


            std::wstring RenderDeviceType(uint32_t deviceType) {

                switch (deviceType) {
                    case FILE_DEVICE_BEEP:                 return L"FILE_DEVICE_BEEP";
                    case FILE_DEVICE_CD_ROM:               return L"FILE_DEVICE_CD_ROM";
                    case FILE_DEVICE_CD_ROM_FILE_SYSTEM:   return L"FILE_DEVICE_CD_ROM_FILE_SYSTEM";
                    case FILE_DEVICE_CONTROLLER:           return L"FILE_DEVICE_CONTROLLER";
                    case FILE_DEVICE_DATALINK:             return L"FILE_DEVICE_DATALINK";
                    case FILE_DEVICE_DFS:                  return L"FILE_DEVICE_DFS";
                    case FILE_DEVICE_DISK:                 return L"FILE_DEVICE_DISK";
                    case FILE_DEVICE_DISK_FILE_SYSTEM:     return L"FILE_DEVICE_DISK_FILE_SYSTEM";
                    case FILE_DEVICE_FILE_SYSTEM:          return L"FILE_DEVICE_FILE_SYSTEM";
                    case FILE_DEVICE_INPORT_PORT:          return L"FILE_DEVICE_INPORT_PORT";
                    case FILE_DEVICE_KEYBOARD:             return L"FILE_DEVICE_KEYBOARD";
                    case FILE_DEVICE_MAILSLOT:             return L"FILE_DEVICE_MAILSLOT";
                    case FILE_DEVICE_MIDI_IN:              return L"FILE_DEVICE_MIDI_IN";
                    case FILE_DEVICE_MIDI_OUT:             return L"FILE_DEVICE_MIDI_OUT";
                    case FILE_DEVICE_MOUSE:                return L"FILE_DEVICE_MOUSE";
                    case FILE_DEVICE_MULTI_UNC_PROVIDER:   return L"FILE_DEVICE_MULTI_UNC_PROVIDER";
                    case FILE_DEVICE_NAMED_PIPE:           return L"FILE_DEVICE_NAMED_PIPE";
                    case FILE_DEVICE_NETWORK:              return L"FILE_DEVICE_NETWORK";
                    case FILE_DEVICE_NETWORK_BROWSER:      return L"FILE_DEVICE_NETWORK_BROWSER";
                    case FILE_DEVICE_NETWORK_FILE_SYSTEM:  return L"FILE_DEVICE_NETWORK_FILE_SYSTEM";
                    case FILE_DEVICE_NULL:                 return L"FILE_DEVICE_NULL";
                    case FILE_DEVICE_PARALLEL_PORT:        return L"FILE_DEVICE_PARALLEL_PORT";
                    case FILE_DEVICE_PHYSICAL_NETCARD:     return L"FILE_DEVICE_PHYSICAL_NETCARD";
                    case FILE_DEVICE_PRINTER:              return L"FILE_DEVICE_PRINTER";
                    case FILE_DEVICE_SCANNER:              return L"FILE_DEVICE_SCANNER";
                    case FILE_DEVICE_SERIAL_MOUSE_PORT:    return L"FILE_DEVICE_SERIAL_MOUSE_PORT";
                    case FILE_DEVICE_SERIAL_PORT:          return L"FILE_DEVICE_SERIAL_PORT";
                    case FILE_DEVICE_SCREEN:               return L"FILE_DEVICE_SCREEN";
                    case FILE_DEVICE_SOUND:                return L"FILE_DEVICE_SOUND";
                    case FILE_DEVICE_STREAMS:              return L"FILE_DEVICE_STREAMS";
                    case FILE_DEVICE_TAPE:                 return L"FILE_DEVICE_TAPE";
                    case FILE_DEVICE_TAPE_FILE_SYSTEM:     return L"FILE_DEVICE_TAPE_FILE_SYSTEM";
                    case FILE_DEVICE_TRANSPORT:            return L"FILE_DEVICE_TRANSPORT";
                    case FILE_DEVICE_UNKNOWN:              return L"FILE_DEVICE_UNKNOWN";
                    case FILE_DEVICE_VIDEO:                return L"FILE_DEVICE_VIDEO";
                    case FILE_DEVICE_VIRTUAL_DISK:         return L"FILE_DEVICE_VIRTUAL_DISK";
                    case FILE_DEVICE_WAVE_IN:              return L"FILE_DEVICE_WAVE_IN";
                    case FILE_DEVICE_WAVE_OUT:             return L"FILE_DEVICE_WAVE_OUT";
                    case FILE_DEVICE_8042_PORT:            return L"FILE_DEVICE_8042_PORT";
                    case FILE_DEVICE_NETWORK_REDIRECTOR:   return L"FILE_DEVICE_NETWORK_REDIRECTOR";
                    case FILE_DEVICE_BATTERY:              return L"FILE_DEVICE_BATTERY";
                    case FILE_DEVICE_BUS_EXTENDER:         return L"FILE_DEVICE_BUS_EXTENDER";
                    case FILE_DEVICE_MODEM:                return L"FILE_DEVICE_MODEM";
                    case FILE_DEVICE_VDM:                  return L"FILE_DEVICE_VDM";
                    case FILE_DEVICE_MASS_STORAGE:         return L"FILE_DEVICE_MASS_STORAGE";
                    case FILE_DEVICE_SMB:                  return L"FILE_DEVICE_SMB";
                    case FILE_DEVICE_KS:                   return L"FILE_DEVICE_KS";
                    case FILE_DEVICE_CHANGER:              return L"FILE_DEVICE_CHANGER";
                    case FILE_DEVICE_SMARTCARD:            return L"FILE_DEVICE_SMARTCARD";
                    case FILE_DEVICE_ACPI:                 return L"FILE_DEVICE_ACPI";
                    case FILE_DEVICE_DVD:                  return L"FILE_DEVICE_DVD";
                    case FILE_DEVICE_FULLSCREEN_VIDEO:     return L"FILE_DEVICE_FULLSCREEN_VIDEO";
                    case FILE_DEVICE_DFS_FILE_SYSTEM:      return L"FILE_DEVICE_DFS_FILE_SYSTEM";
                    case FILE_DEVICE_DFS_VOLUME:           return L"FILE_DEVICE_DFS_VOLUME";
                    case FILE_DEVICE_SERENUM:              return L"FILE_DEVICE_SERENUM";
                    case FILE_DEVICE_TERMSRV:              return L"FILE_DEVICE_TERMSRV";
                    case FILE_DEVICE_KSEC:                 return L"FILE_DEVICE_KSEC";
                    case FILE_DEVICE_FIPS:                 return L"FILE_DEVICE_FIPS";
                    case FILE_DEVICE_INFINIBAND:           return L"FILE_DEVICE_INFINIBAND";
                    case FILE_DEVICE_VMBUS:                return L"FILE_DEVICE_VMBUS";
                    case FILE_DEVICE_CRYPT_PROVIDER:       return L"FILE_DEVICE_CRYPT_PROVIDER";
                    case FILE_DEVICE_WPD:                  return L"FILE_DEVICE_WPD";
                    case FILE_DEVICE_BLUETOOTH:            return L"FILE_DEVICE_BLUETOOTH";
                    case FILE_DEVICE_MT_COMPOSITE:         return L"FILE_DEVICE_MT_COMPOSITE";
                    case FILE_DEVICE_MT_TRANSPORT:         return L"FILE_DEVICE_MT_TRANSPORT";
                    case FILE_DEVICE_BIOMETRIC:            return L"FILE_DEVICE_BIOMETRIC";
                    case FILE_DEVICE_PMI:                  return L"FILE_DEVICE_PMI";
                    case FILE_DEVICE_EHSTOR:               return L"FILE_DEVICE_EHSTOR";
                    case FILE_DEVICE_DEVAPI:               return L"FILE_DEVICE_DEVAPI";
                    case FILE_DEVICE_GPIO:                 return L"FILE_DEVICE_GPIO";
                    case FILE_DEVICE_USBEX:                return L"FILE_DEVICE_USBEX";
                    case FILE_DEVICE_CONSOLE:              return L"FILE_DEVICE_CONSOLE";
                    case FILE_DEVICE_NFP:                  return L"FILE_DEVICE_NFP";
                    case FILE_DEVICE_SYSENV:               return L"FILE_DEVICE_SYSENV";
                    case FILE_DEVICE_VIRTUAL_BLOCK:        return L"FILE_DEVICE_VIRTUAL_BLOCK";
                    case FILE_DEVICE_POINT_OF_SERVICE:     return L"FILE_DEVICE_POINT_OF_SERVICE";
                    case FILE_DEVICE_STORAGE_REPLICATION:  return L"FILE_DEVICE_STORAGE_REPLICATION";
                    case FILE_DEVICE_TRUST_ENV:            return L"FILE_DEVICE_TRUST_ENV";
                    case FILE_DEVICE_UCM:                  return L"FILE_DEVICE_UCM";
                    case FILE_DEVICE_UCMTCPCI:             return L"FILE_DEVICE_UCMTCPCI";
                    case FILE_DEVICE_PERSISTENT_MEMORY:    return L"FILE_DEVICE_PERSISTENT_MEMORY";
                    case FILE_DEVICE_NVDIMM:               return L"FILE_DEVICE_NVDIMM";
                    case FILE_DEVICE_HOLOGRAPHIC:          return L"FILE_DEVICE_HOLOGRAPHIC";
                    case FILE_DEVICE_SDFXHCI:              return L"FILE_DEVICE_SDFXHCI";
                    case FILE_DEVICE_UCMUCSI:              return L"FILE_DEVICE_UCMUCSI";
                    case FILE_DEVICE_PRM:                  return L"FILE_DEVICE_PRM";
                    case FILE_DEVICE_EVENT_COLLECTOR:      return L"FILE_DEVICE_EVENT_COLLECTOR";
                    case FILE_DEVICE_USB4:                 return L"FILE_DEVICE_USB4";
                    case FILE_DEVICE_SOUNDWIRE:            return L"FILE_DEVICE_SOUNDWIRE";
                    case FILE_DEVICE_FABRIC_NVME:          return L"FILE_DEVICE_FABRIC_NVME";
                    case FILE_DEVICE_SVM:                  return L"FILE_DEVICE_SVM";
                    case FILE_DEVICE_HARDWARE_ACCELERATOR: return L"FILE_DEVICE_HARDWARE_ACCELERATOR";
                    case FILE_DEVICE_I3C:                  return L"FILE_DEVICE_I3C";
                    case FILE_DEVICE_MULTITIER_MEMORY:     return L"FILE_DEVICE_MULTITIER_MEMORY";
                    case FILE_DEVICE_CXL_TYPE3:            return L"FILE_DEVICE_CXL_TYPE3";
                }

                return std::format(L"0x{:X}", deviceType);
            }


            std::wstring RenderSymlinkFlags(uint32_t flags) {

                return RenderFlags(flags, SYMLINK_FLAG_NAMES, L"|");
            }


            std::wstring RenderFileSystemStatisticsType(uint16_t type) {

                switch (type) {
                    case FILESYSTEM_STATISTICS_TYPE_NTFS:  return L"NTFS";
                    case FILESYSTEM_STATISTICS_TYPE_FAT:   return L"FAT";
                    case FILESYSTEM_STATISTICS_TYPE_EXFAT: return L"EXFAT";
                    case FILESYSTEM_STATISTICS_TYPE_REFS:  return L"REFS";
                }

                return std::to_wstring(type);
            }


            std::wstring RenderOplockLevel(uint32_t level) {

                if (!level) return L"None";

                return RenderFlags(level, OPLOCK_LEVEL_NAMES, L"|");
            }


            std::wstring RenderOplockInputFlags(uint32_t flags) {

                if (!flags) return L"None";

                return RenderFlags(flags, OPLOCK_INPUT_FLAG_NAMES, L"|");
            }


            std::wstring RenderOplockOutputFlags(uint32_t flags) {

                if (!flags) return L"None";

                return RenderFlags(flags, OPLOCK_OUTPUT_FLAG_NAMES, L"|");
            }


            std::wstring RenderFileRegionUsage(uint32_t usage) {

                if (!usage) return L"None";

                return RenderFlags(usage, FILE_REGION_USAGE_NAMES, L"|");
            }


            std::wstring RenderUsnReason(uint32_t reason) {

                return RenderFlags(reason, USN_REASON_NAMES, L"|");
            }


            std::wstring RenderStoragePropertyId(uint32_t propertyId) {

                switch (propertyId) {
                    case StorageDeviceProperty:                    return L"StorageDeviceProperty";
                    case StorageAdapterProperty:                   return L"StorageAdapterProperty";
                    case StorageDeviceIdProperty:                  return L"StorageDeviceIdProperty";
                    case StorageDeviceUniqueIdProperty:            return L"StorageDeviceUniqueIdProperty";
                    case StorageDeviceWriteCacheProperty:          return L"StorageDeviceWriteCacheProperty";
                    case StorageMiniportProperty:                  return L"StorageMiniportProperty";
                    case StorageAccessAlignmentProperty:           return L"StorageAccessAlignmentProperty";
                    case StorageDeviceSeekPenaltyProperty:         return L"StorageDeviceSeekPenaltyProperty";
                    case StorageDeviceTrimProperty:                return L"StorageDeviceTrimProperty";
                    case StorageDeviceWriteAggregationProperty:    return L"StorageDeviceWriteAggregationProperty";
                    case StorageDeviceDeviceTelemetryProperty:     return L"StorageDeviceDeviceTelemetryProperty";
                    case StorageDeviceLBProvisioningProperty:      return L"StorageDeviceLBProvisioningProperty";
                    case StorageDevicePowerProperty:               return L"StorageDevicePowerProperty";
                    case StorageDeviceCopyOffloadProperty:         return L"StorageDeviceCopyOffloadProperty";
                    case StorageDeviceResiliencyProperty:          return L"StorageDeviceResiliencyProperty";
                    case StorageDeviceMediumProductType:           return L"StorageDeviceMediumProductType";
                    case StorageAdapterRpmbProperty:               return L"StorageAdapterRpmbProperty";
                    case StorageAdapterCryptoProperty:             return L"StorageAdapterCryptoProperty";
                    case StorageDeviceIoCapabilityProperty:        return L"StorageDeviceIoCapabilityProperty";
                    case StorageAdapterProtocolSpecificProperty:   return L"StorageAdapterProtocolSpecificProperty";
                    case StorageDeviceProtocolSpecificProperty:    return L"StorageDeviceProtocolSpecificProperty";
                    case StorageAdapterTemperatureProperty:        return L"StorageAdapterTemperatureProperty";
                    case StorageDeviceTemperatureProperty:         return L"StorageDeviceTemperatureProperty";
                    case StorageAdapterPhysicalTopologyProperty:   return L"StorageAdapterPhysicalTopologyProperty";
                    case StorageDevicePhysicalTopologyProperty:    return L"StorageDevicePhysicalTopologyProperty";
                    case StorageDeviceAttributesProperty:          return L"StorageDeviceAttributesProperty";
                    case StorageDeviceManagementStatus:            return L"StorageDeviceManagementStatus";
                    case StorageAdapterSerialNumberProperty:       return L"StorageAdapterSerialNumberProperty";
                    case StorageDeviceLocationProperty:            return L"StorageDeviceLocationProperty";
                    case StorageDeviceNumaProperty:                return L"StorageDeviceNumaProperty";
                    case StorageDeviceZonedDeviceProperty:         return L"StorageDeviceZonedDeviceProperty";
                    case StorageDeviceUnsafeShutdownCount:         return L"StorageDeviceUnsafeShutdownCount";
                    case StorageDeviceEnduranceProperty:           return L"StorageDeviceEnduranceProperty";
                    case StorageDeviceLedStateProperty:            return L"StorageDeviceLedStateProperty";
                    case StorageDeviceSelfEncryptionProperty:      return L"StorageDeviceSelfEncryptionProperty";
                    case StorageFruIdProperty:                     return L"StorageFruIdProperty";
                    case StorageStackProperty:                     return L"StorageStackProperty";
                    case StorageAdapterProtocolSpecificPropertyEx: return L"StorageAdapterProtocolSpecificPropertyEx";
                    case StorageDeviceProtocolSpecificPropertyEx:  return L"StorageDeviceProtocolSpecificPropertyEx";
                    case StorageHwCryptoProperty:                  return L"StorageHwCryptoProperty";
                }

                return std::to_wstring(propertyId);
            }


            std::wstring RenderStorageQueryType(uint32_t queryType) {

                switch (queryType) {
                    case PropertyStandardQuery: return L"PropertyStandardQuery";
                    case PropertyExistsQuery:   return L"PropertyExistsQuery";
                    case PropertyMaskQuery:     return L"PropertyMaskQuery";
                }

                return std::to_wstring(queryType);
            }


            std::wstring RenderMediaType(uint32_t mediaType) {

                switch (mediaType) {
                    case Unknown:        return L"Unknown";
                    case F5_1Pt2_512:    return L"F5_1Pt2_512";
                    case F3_1Pt44_512:   return L"F3_1Pt44_512";
                    case F3_2Pt88_512:   return L"F3_2Pt88_512";
                    case F3_20Pt8_512:   return L"F3_20Pt8_512";
                    case F3_720_512:     return L"F3_720_512";
                    case F5_360_512:     return L"F5_360_512";
                    case F5_320_512:     return L"F5_320_512";
                    case F5_320_1024:    return L"F5_320_1024";
                    case F5_180_512:     return L"F5_180_512";
                    case F5_160_512:     return L"F5_160_512";
                    case RemovableMedia: return L"RemovableMedia";
                    case FixedMedia:     return L"FixedMedia";
                    case F3_120M_512:    return L"F3_120M_512";
                    case F3_640_512:     return L"F3_640_512";
                    case F5_640_512:     return L"F5_640_512";
                    case F5_720_512:     return L"F5_720_512";
                    case F3_1Pt2_512:    return L"F3_1Pt2_512";
                    case F3_1Pt23_1024:  return L"F3_1Pt23_1024";
                    case F5_1Pt23_1024:  return L"F5_1Pt23_1024";
                    case F3_128Mb_512:   return L"F3_128Mb_512";
                    case F3_230Mb_512:   return L"F3_230Mb_512";
                    case F8_256_128:     return L"F8_256_128";
                    case F3_200Mb_512:   return L"F3_200Mb_512";
                    case F3_240M_512:    return L"F3_240M_512";
                    case F3_32M_512:     return L"F3_32M_512";
                }

                return std::to_wstring(mediaType);
            }


            std::wstring RenderSecurityInformation(uint32_t securityInformation) {

                if (!securityInformation) return L"None";

                return RenderFlags(securityInformation, SECURITY_INFORMATION_NAMES, L"|");
            }

        }

    }

}
