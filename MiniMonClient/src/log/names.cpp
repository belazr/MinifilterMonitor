#include "names.h"

#include "..\kernel.h"

#include <Windows.h>
#include <wnnc.h>

#include <cstddef>
#include <format>
#include <span>
#include <string>
#include <string_view>

using namespace mimo;

namespace {

    struct FlagName {
        ULONG flag;
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
        ULONG flags,
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
        { kernel::FILE_DIRECTORY_FILE,            L"Directory" },
        { kernel::FILE_WRITE_THROUGH,             L"Write Through" },
        { kernel::FILE_SEQUENTIAL_ONLY,           L"Sequential Access" },
        { kernel::FILE_NO_INTERMEDIATE_BUFFERING, L"No Buffering" },
        { kernel::FILE_SYNCHRONOUS_IO_ALERT,      L"Synchronous IO Alert" },
        { kernel::FILE_SYNCHRONOUS_IO_NONALERT,   L"Synchronous IO Non-Alert" },
        { kernel::FILE_NON_DIRECTORY_FILE,        L"Non-Directory File" },
        { kernel::FILE_CREATE_TREE_CONNECTION,    L"Create Tree Connection" },
        { kernel::FILE_COMPLETE_IF_OPLOCKED,      L"Complete If Oplocked" },
        { kernel::FILE_NO_EA_KNOWLEDGE,           L"No EA Knowledge" },
        { kernel::FILE_OPEN_REMOTE_INSTANCE,      L"Open Remote Instance" },
        { kernel::FILE_RANDOM_ACCESS,             L"Random Access" },
        { kernel::FILE_DELETE_ON_CLOSE,           L"Delete On Close" },
        { kernel::FILE_OPEN_BY_FILE_ID,           L"Open By ID" },
        { kernel::FILE_OPEN_FOR_BACKUP_INTENT,    L"Open For Backup" },
        { kernel::FILE_NO_COMPRESSION,            L"No Compression" },
        { kernel::FILE_OPEN_REQUIRING_OPLOCK,     L"Open Requiring Oplock" },
        { kernel::FILE_DISALLOW_EXCLUSIVE,        L"Disallow Exclusive" },
        { kernel::FILE_SESSION_AWARE,             L"Session Aware" },
        { kernel::FILE_RESERVE_OPFILTER,          L"Reserve OpFilter" },
        { kernel::FILE_OPEN_REPARSE_POINT,        L"Open Reparse Point" },
        { kernel::FILE_OPEN_NO_RECALL,            L"Open No Recall" },
        { kernel::FILE_OPEN_FOR_FREE_SPACE_QUERY, L"Open For Free Space Query" },
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
        { kernel::REMOTE_PROTOCOL_FLAG_LOOPBACK,          L"Loopback" },
        { kernel::REMOTE_PROTOCOL_FLAG_OFFLINE,           L"Offline" },
        { kernel::REMOTE_PROTOCOL_FLAG_PERSISTENT_HANDLE, L"Persistent Handle" },
        { kernel::REMOTE_PROTOCOL_FLAG_PRIVACY,           L"Privacy" },
        { kernel::REMOTE_PROTOCOL_FLAG_INTEGRITY,         L"Integrity" },
        { kernel::REMOTE_PROTOCOL_FLAG_MUTUAL_AUTH,       L"Mutual Auth" },
    };

    static_assert(CompositesPrecedeComponents(REMOTE_PROTOCOL_FLAG_NAMES), "composite entry after its components in REMOTE_PROTOCOL_FLAG_NAMES");

    constexpr FlagName DISPOSITION_FLAG_NAMES[]{
        { kernel::FILE_DISPOSITION_DELETE,                    L"Delete" },
        { kernel::FILE_DISPOSITION_POSIX_SEMANTICS,           L"POSIX Semantics" },
        { kernel::FILE_DISPOSITION_FORCE_IMAGE_SECTION_CHECK, L"Force Image Section Check" },
        { kernel::FILE_DISPOSITION_ON_CLOSE,                  L"On Close" },
        { kernel::FILE_DISPOSITION_IGNORE_READONLY_ATTRIBUTE, L"Ignore Readonly Attribute" },
    };

    static_assert(CompositesPrecedeComponents(DISPOSITION_FLAG_NAMES), "composite entry after its components in DISPOSITION_FLAG_NAMES");

    constexpr FlagName RENAME_FLAG_NAMES[]{
        { kernel::FILE_RENAME_FORCE_RESIZE_SR,                      L"Force Resize SR" },
        { kernel::FILE_RENAME_PRESERVE_AVAILABLE_SPACE,             L"Preserve Available Space" },
        { kernel::FILE_RENAME_REPLACE_IF_EXISTS,                    L"Replace If Exists" },
        { kernel::FILE_RENAME_POSIX_SEMANTICS,                      L"POSIX Semantics" },
        { kernel::FILE_RENAME_SUPPRESS_PIN_STATE_INHERITANCE,       L"Suppress Pin State Inheritance" },
        { kernel::FILE_RENAME_SUPPRESS_STORAGE_RESERVE_INHERITANCE, L"Suppress Storage Reserve Inheritance" },
        { kernel::FILE_RENAME_NO_INCREASE_AVAILABLE_SPACE,          L"No Increase Available Space" },
        { kernel::FILE_RENAME_NO_DECREASE_AVAILABLE_SPACE,          L"No Decrease Available Space" },
        { kernel::FILE_RENAME_IGNORE_READONLY_ATTRIBUTE,            L"Ignore Readonly Attribute" },
        { kernel::FILE_RENAME_FORCE_RESIZE_TARGET_SR,               L"Force Resize Target SR" },
        { kernel::FILE_RENAME_FORCE_RESIZE_SOURCE_SR,               L"Force Resize Source SR" },
    };

    static_assert(CompositesPrecedeComponents(RENAME_FLAG_NAMES), "composite entry after its components in RENAME_FLAG_NAMES");

}

namespace mimo {

    namespace log {

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

                return std::format(L"{:02X}", static_cast<ULONG>(major));
            }


            std::wstring RenderMinorFunction(uint8_t major, uint8_t minor) {

                switch (major) {
                    case kernel::IRP_MJ_DIRECTORY_CONTROL:
                        switch (minor) {
                            case kernel::IRP_MN_QUERY_DIRECTORY:         return L"IRP_MN_QUERY_DIRECTORY";
                            case kernel::IRP_MN_NOTIFY_CHANGE_DIRECTORY: return L"IRP_MN_NOTIFY_CHANGE_DIRECTORY";
                        }
                        break;

                    case kernel::IRP_MJ_FILE_SYSTEM_CONTROL:
                        switch (minor) {
                            case kernel::IRP_MN_USER_FS_REQUEST:  return L"IRP_MN_USER_FS_REQUEST";
                            case kernel::IRP_MN_MOUNT_VOLUME:     return L"IRP_MN_MOUNT_VOLUME";
                            case kernel::IRP_MN_VERIFY_VOLUME:    return L"IRP_MN_VERIFY_VOLUME";
                            case kernel::IRP_MN_LOAD_FILE_SYSTEM: return L"IRP_MN_LOAD_FILE_SYSTEM";
                            case kernel::IRP_MN_TRACK_LINK:       return L"IRP_MN_TRACK_LINK";
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

                return std::format(L"{:02X}", static_cast<ULONG>(minor));
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


            std::wstring RenderByteOffset(int64_t byteOffset) {

                if (byteOffset == kernel::FILE_WRITE_TO_END_OF_FILE) return L"EOF";

                if (byteOffset == kernel::FILE_USE_FILE_POINTER_POSITION) return L"Current";

                return std::to_wstring(byteOffset);
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

        }

    }

}
