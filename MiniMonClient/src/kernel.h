#pragma once

#include <Windows.h>

namespace mimo {

    namespace kernel {

        // operation category flags carried in RecordData::flags

        inline constexpr ULONG FLT_CALLBACK_DATA_IRP_OPERATION       = 0x00000001u;
        inline constexpr ULONG FLT_CALLBACK_DATA_FAST_IO_OPERATION   = 0x00000002u;
        inline constexpr ULONG FLT_CALLBACK_DATA_FS_FILTER_OPERATION = 0x00000004u;

        // standard IRP major codes

        inline constexpr UCHAR IRP_MJ_CREATE                   = 0x00u;
        inline constexpr UCHAR IRP_MJ_CREATE_NAMED_PIPE        = 0x01u;
        inline constexpr UCHAR IRP_MJ_CLOSE                    = 0x02u;
        inline constexpr UCHAR IRP_MJ_READ                     = 0x03u;
        inline constexpr UCHAR IRP_MJ_WRITE                    = 0x04u;
        inline constexpr UCHAR IRP_MJ_QUERY_INFORMATION        = 0x05u;
        inline constexpr UCHAR IRP_MJ_SET_INFORMATION          = 0x06u;
        inline constexpr UCHAR IRP_MJ_QUERY_EA                 = 0x07u;
        inline constexpr UCHAR IRP_MJ_SET_EA                   = 0x08u;
        inline constexpr UCHAR IRP_MJ_FLUSH_BUFFERS            = 0x09u;
        inline constexpr UCHAR IRP_MJ_QUERY_VOLUME_INFORMATION = 0x0Au;
        inline constexpr UCHAR IRP_MJ_SET_VOLUME_INFORMATION   = 0x0Bu;
        inline constexpr UCHAR IRP_MJ_DIRECTORY_CONTROL        = 0x0Cu;
        inline constexpr UCHAR IRP_MJ_FILE_SYSTEM_CONTROL      = 0x0Du;
        inline constexpr UCHAR IRP_MJ_DEVICE_CONTROL           = 0x0Eu;
        inline constexpr UCHAR IRP_MJ_INTERNAL_DEVICE_CONTROL  = 0x0Fu;
        inline constexpr UCHAR IRP_MJ_SHUTDOWN                 = 0x10u;
        inline constexpr UCHAR IRP_MJ_LOCK_CONTROL             = 0x11u;
        inline constexpr UCHAR IRP_MJ_CLEANUP                  = 0x12u;
        inline constexpr UCHAR IRP_MJ_CREATE_MAILSLOT          = 0x13u;
        inline constexpr UCHAR IRP_MJ_QUERY_SECURITY           = 0x14u;
        inline constexpr UCHAR IRP_MJ_SET_SECURITY             = 0x15u;
        inline constexpr UCHAR IRP_MJ_POWER                    = 0x16u;
        inline constexpr UCHAR IRP_MJ_SYSTEM_CONTROL           = 0x17u;
        inline constexpr UCHAR IRP_MJ_DEVICE_CHANGE            = 0x18u;
        inline constexpr UCHAR IRP_MJ_QUERY_QUOTA              = 0x19u;
        inline constexpr UCHAR IRP_MJ_SET_QUOTA                = 0x1Au;
        inline constexpr UCHAR IRP_MJ_PNP                      = 0x1Bu;
        inline constexpr UCHAR IRP_MJ_MAXIMUM_FUNCTION         = 0x1Bu;

        // FltMgr extension major codes

        inline constexpr UCHAR IRP_MJ_ACQUIRE_FOR_SECTION_SYNCHRONIZATION = static_cast<UCHAR>(-1);
        inline constexpr UCHAR IRP_MJ_RELEASE_FOR_SECTION_SYNCHRONIZATION = static_cast<UCHAR>(-2);
        inline constexpr UCHAR IRP_MJ_ACQUIRE_FOR_MOD_WRITE               = static_cast<UCHAR>(-3);
        inline constexpr UCHAR IRP_MJ_RELEASE_FOR_MOD_WRITE               = static_cast<UCHAR>(-4);
        inline constexpr UCHAR IRP_MJ_ACQUIRE_FOR_CC_FLUSH                = static_cast<UCHAR>(-5);
        inline constexpr UCHAR IRP_MJ_RELEASE_FOR_CC_FLUSH                = static_cast<UCHAR>(-6);
        inline constexpr UCHAR IRP_MJ_QUERY_OPEN                          = static_cast<UCHAR>(-7);

        inline constexpr UCHAR IRP_MJ_FAST_IO_CHECK_IF_POSSIBLE           = static_cast<UCHAR>(-13);
        inline constexpr UCHAR IRP_MJ_NETWORK_QUERY_OPEN                  = static_cast<UCHAR>(-14);
        inline constexpr UCHAR IRP_MJ_MDL_READ                            = static_cast<UCHAR>(-15);
        inline constexpr UCHAR IRP_MJ_MDL_READ_COMPLETE                   = static_cast<UCHAR>(-16);
        inline constexpr UCHAR IRP_MJ_PREPARE_MDL_WRITE                   = static_cast<UCHAR>(-17);
        inline constexpr UCHAR IRP_MJ_MDL_WRITE_COMPLETE                  = static_cast<UCHAR>(-18);
        inline constexpr UCHAR IRP_MJ_VOLUME_MOUNT                        = static_cast<UCHAR>(-19);
        inline constexpr UCHAR IRP_MJ_VOLUME_DISMOUNT                     = static_cast<UCHAR>(-20);

        // IRP minor codes, meaning is scoped by the major code

        // directory control
        inline constexpr UCHAR IRP_MN_QUERY_DIRECTORY         = 0x01u;
        inline constexpr UCHAR IRP_MN_NOTIFY_CHANGE_DIRECTORY = 0x02u;

        // file system control
        inline constexpr UCHAR IRP_MN_USER_FS_REQUEST  = 0x00u;
        inline constexpr UCHAR IRP_MN_MOUNT_VOLUME     = 0x01u;
        inline constexpr UCHAR IRP_MN_VERIFY_VOLUME    = 0x02u;
        inline constexpr UCHAR IRP_MN_LOAD_FILE_SYSTEM = 0x03u;
        inline constexpr UCHAR IRP_MN_TRACK_LINK       = 0x04u;

        // lock control
        inline constexpr UCHAR IRP_MN_LOCK              = 0x01u;
        inline constexpr UCHAR IRP_MN_UNLOCK_SINGLE     = 0x02u;
        inline constexpr UCHAR IRP_MN_UNLOCK_ALL        = 0x03u;
        inline constexpr UCHAR IRP_MN_UNLOCK_ALL_BY_KEY = 0x04u;

        // read / write
        inline constexpr UCHAR IRP_MN_NORMAL           = 0x00u;
        inline constexpr UCHAR IRP_MN_DPC              = 0x01u;
        inline constexpr UCHAR IRP_MN_MDL              = 0x02u;
        inline constexpr UCHAR IRP_MN_COMPLETE         = 0x04u;
        inline constexpr UCHAR IRP_MN_COMPRESSED       = 0x08u;
        inline constexpr UCHAR IRP_MN_MDL_DPC          = IRP_MN_MDL | IRP_MN_DPC;
        inline constexpr UCHAR IRP_MN_COMPLETE_MDL     = IRP_MN_COMPLETE | IRP_MN_MDL;
        inline constexpr UCHAR IRP_MN_COMPLETE_MDL_DPC = IRP_MN_COMPLETE_MDL | IRP_MN_DPC;

        // device control
        inline constexpr UCHAR IRP_MN_SCSI_CLASS = 0x01u;

        // PnP
        inline constexpr UCHAR IRP_MN_START_DEVICE                 = 0x00u;
        inline constexpr UCHAR IRP_MN_QUERY_REMOVE_DEVICE          = 0x01u;
        inline constexpr UCHAR IRP_MN_REMOVE_DEVICE                = 0x02u;
        inline constexpr UCHAR IRP_MN_CANCEL_REMOVE_DEVICE         = 0x03u;
        inline constexpr UCHAR IRP_MN_STOP_DEVICE                  = 0x04u;
        inline constexpr UCHAR IRP_MN_QUERY_STOP_DEVICE            = 0x05u;
        inline constexpr UCHAR IRP_MN_CANCEL_STOP_DEVICE           = 0x06u;
        inline constexpr UCHAR IRP_MN_QUERY_DEVICE_RELATIONS       = 0x07u;
        inline constexpr UCHAR IRP_MN_QUERY_INTERFACE              = 0x08u;
        inline constexpr UCHAR IRP_MN_QUERY_CAPABILITIES           = 0x09u;
        inline constexpr UCHAR IRP_MN_QUERY_RESOURCES              = 0x0Au;
        inline constexpr UCHAR IRP_MN_QUERY_RESOURCE_REQUIREMENTS  = 0x0Bu;
        inline constexpr UCHAR IRP_MN_QUERY_DEVICE_TEXT            = 0x0Cu;
        inline constexpr UCHAR IRP_MN_FILTER_RESOURCE_REQUIREMENTS = 0x0Du;
        inline constexpr UCHAR IRP_MN_READ_CONFIG                  = 0x0Fu;
        inline constexpr UCHAR IRP_MN_WRITE_CONFIG                 = 0x10u;
        inline constexpr UCHAR IRP_MN_EJECT                        = 0x11u;
        inline constexpr UCHAR IRP_MN_SET_LOCK                     = 0x12u;
        inline constexpr UCHAR IRP_MN_QUERY_ID                     = 0x13u;
        inline constexpr UCHAR IRP_MN_QUERY_PNP_DEVICE_STATE       = 0x14u;
        inline constexpr UCHAR IRP_MN_QUERY_BUS_INFORMATION        = 0x15u;
        inline constexpr UCHAR IRP_MN_DEVICE_USAGE_NOTIFICATION    = 0x16u;
        inline constexpr UCHAR IRP_MN_SURPRISE_REMOVAL             = 0x17u;
        inline constexpr UCHAR IRP_MN_QUERY_LEGACY_BUS_INFORMATION = 0x18u;

        // power
        inline constexpr UCHAR IRP_MN_WAIT_WAKE      = 0x00u;
        inline constexpr UCHAR IRP_MN_POWER_SEQUENCE = 0x01u;
        inline constexpr UCHAR IRP_MN_SET_POWER      = 0x02u;
        inline constexpr UCHAR IRP_MN_QUERY_POWER    = 0x03u;

        // system control (WMI)
        inline constexpr UCHAR IRP_MN_QUERY_ALL_DATA         = 0x00u;
        inline constexpr UCHAR IRP_MN_QUERY_SINGLE_INSTANCE  = 0x01u;
        inline constexpr UCHAR IRP_MN_CHANGE_SINGLE_INSTANCE = 0x02u;
        inline constexpr UCHAR IRP_MN_CHANGE_SINGLE_ITEM     = 0x03u;
        inline constexpr UCHAR IRP_MN_ENABLE_EVENTS          = 0x04u;
        inline constexpr UCHAR IRP_MN_DISABLE_EVENTS         = 0x05u;
        inline constexpr UCHAR IRP_MN_ENABLE_COLLECTION      = 0x06u;
        inline constexpr UCHAR IRP_MN_DISABLE_COLLECTION     = 0x07u;
        inline constexpr UCHAR IRP_MN_REGINFO                = 0x08u;
        inline constexpr UCHAR IRP_MN_EXECUTE_METHOD         = 0x09u;

        constexpr const wchar_t* MajorFunctionName(UCHAR major) noexcept {

            switch (major) {
                case IRP_MJ_CREATE:                              return L"IRP_MJ_CREATE";
                case IRP_MJ_CREATE_NAMED_PIPE:                   return L"IRP_MJ_CREATE_NAMED_PIPE";
                case IRP_MJ_CLOSE:                               return L"IRP_MJ_CLOSE";
                case IRP_MJ_READ:                                return L"IRP_MJ_READ";
                case IRP_MJ_WRITE:                               return L"IRP_MJ_WRITE";
                case IRP_MJ_QUERY_INFORMATION:                   return L"IRP_MJ_QUERY_INFORMATION";
                case IRP_MJ_SET_INFORMATION:                     return L"IRP_MJ_SET_INFORMATION";
                case IRP_MJ_QUERY_EA:                            return L"IRP_MJ_QUERY_EA";
                case IRP_MJ_SET_EA:                              return L"IRP_MJ_SET_EA";
                case IRP_MJ_FLUSH_BUFFERS:                       return L"IRP_MJ_FLUSH_BUFFERS";
                case IRP_MJ_QUERY_VOLUME_INFORMATION:            return L"IRP_MJ_QUERY_VOLUME_INFORMATION";
                case IRP_MJ_SET_VOLUME_INFORMATION:              return L"IRP_MJ_SET_VOLUME_INFORMATION";
                case IRP_MJ_DIRECTORY_CONTROL:                   return L"IRP_MJ_DIRECTORY_CONTROL";
                case IRP_MJ_FILE_SYSTEM_CONTROL:                 return L"IRP_MJ_FILE_SYSTEM_CONTROL";
                case IRP_MJ_DEVICE_CONTROL:                      return L"IRP_MJ_DEVICE_CONTROL";
                case IRP_MJ_INTERNAL_DEVICE_CONTROL:             return L"IRP_MJ_INTERNAL_DEVICE_CONTROL";
                case IRP_MJ_SHUTDOWN:                            return L"IRP_MJ_SHUTDOWN";
                case IRP_MJ_LOCK_CONTROL:                        return L"IRP_MJ_LOCK_CONTROL";
                case IRP_MJ_CLEANUP:                             return L"IRP_MJ_CLEANUP";
                case IRP_MJ_CREATE_MAILSLOT:                     return L"IRP_MJ_CREATE_MAILSLOT";
                case IRP_MJ_QUERY_SECURITY:                      return L"IRP_MJ_QUERY_SECURITY";
                case IRP_MJ_SET_SECURITY:                        return L"IRP_MJ_SET_SECURITY";
                case IRP_MJ_POWER:                               return L"IRP_MJ_POWER";
                case IRP_MJ_SYSTEM_CONTROL:                      return L"IRP_MJ_SYSTEM_CONTROL";
                case IRP_MJ_DEVICE_CHANGE:                       return L"IRP_MJ_DEVICE_CHANGE";
                case IRP_MJ_QUERY_QUOTA:                         return L"IRP_MJ_QUERY_QUOTA";
                case IRP_MJ_SET_QUOTA:                           return L"IRP_MJ_SET_QUOTA";
                case IRP_MJ_PNP:                                 return L"IRP_MJ_PNP";
                case IRP_MJ_ACQUIRE_FOR_SECTION_SYNCHRONIZATION: return L"IRP_MJ_ACQUIRE_FOR_SECTION_SYNCHRONIZATION";
                case IRP_MJ_RELEASE_FOR_SECTION_SYNCHRONIZATION: return L"IRP_MJ_RELEASE_FOR_SECTION_SYNCHRONIZATION";
                case IRP_MJ_ACQUIRE_FOR_MOD_WRITE:               return L"IRP_MJ_ACQUIRE_FOR_MOD_WRITE";
                case IRP_MJ_RELEASE_FOR_MOD_WRITE:               return L"IRP_MJ_RELEASE_FOR_MOD_WRITE";
                case IRP_MJ_ACQUIRE_FOR_CC_FLUSH:                return L"IRP_MJ_ACQUIRE_FOR_CC_FLUSH";
                case IRP_MJ_RELEASE_FOR_CC_FLUSH:                return L"IRP_MJ_RELEASE_FOR_CC_FLUSH";
                case IRP_MJ_QUERY_OPEN:                          return L"IRP_MJ_QUERY_OPEN";
                case IRP_MJ_FAST_IO_CHECK_IF_POSSIBLE:           return L"IRP_MJ_FAST_IO_CHECK_IF_POSSIBLE";
                case IRP_MJ_NETWORK_QUERY_OPEN:                  return L"IRP_MJ_NETWORK_QUERY_OPEN";
                case IRP_MJ_MDL_READ:                            return L"IRP_MJ_MDL_READ";
                case IRP_MJ_MDL_READ_COMPLETE:                   return L"IRP_MJ_MDL_READ_COMPLETE";
                case IRP_MJ_PREPARE_MDL_WRITE:                   return L"IRP_MJ_PREPARE_MDL_WRITE";
                case IRP_MJ_MDL_WRITE_COMPLETE:                  return L"IRP_MJ_MDL_WRITE_COMPLETE";
                case IRP_MJ_VOLUME_MOUNT:                        return L"IRP_MJ_VOLUME_MOUNT";
                case IRP_MJ_VOLUME_DISMOUNT:                     return L"IRP_MJ_VOLUME_DISMOUNT";
            }

            return L"";
        }


        constexpr const wchar_t* MinorFunctionName(UCHAR major, UCHAR minor) noexcept {

            switch (major) {
                case IRP_MJ_DIRECTORY_CONTROL:
                    switch (minor) {
                        case IRP_MN_QUERY_DIRECTORY:         return L"IRP_MN_QUERY_DIRECTORY";
                        case IRP_MN_NOTIFY_CHANGE_DIRECTORY: return L"IRP_MN_NOTIFY_CHANGE_DIRECTORY";
                    }
                    break;

                case IRP_MJ_FILE_SYSTEM_CONTROL:
                    switch (minor) {
                        case IRP_MN_USER_FS_REQUEST:  return L"IRP_MN_USER_FS_REQUEST";
                        case IRP_MN_MOUNT_VOLUME:     return L"IRP_MN_MOUNT_VOLUME";
                        case IRP_MN_VERIFY_VOLUME:    return L"IRP_MN_VERIFY_VOLUME";
                        case IRP_MN_LOAD_FILE_SYSTEM: return L"IRP_MN_LOAD_FILE_SYSTEM";
                        case IRP_MN_TRACK_LINK:       return L"IRP_MN_TRACK_LINK";
                    }
                    break;

                case IRP_MJ_LOCK_CONTROL:
                    switch (minor) {
                        case IRP_MN_LOCK:              return L"IRP_MN_LOCK";
                        case IRP_MN_UNLOCK_SINGLE:     return L"IRP_MN_UNLOCK_SINGLE";
                        case IRP_MN_UNLOCK_ALL:        return L"IRP_MN_UNLOCK_ALL";
                        case IRP_MN_UNLOCK_ALL_BY_KEY: return L"IRP_MN_UNLOCK_ALL_BY_KEY";
                    }
                    break;

                case IRP_MJ_READ:
                case IRP_MJ_WRITE:
                    switch (minor) {
                        case IRP_MN_NORMAL:           return L"IRP_MN_NORMAL";
                        case IRP_MN_DPC:              return L"IRP_MN_DPC";
                        case IRP_MN_MDL:              return L"IRP_MN_MDL";
                        case IRP_MN_COMPLETE:         return L"IRP_MN_COMPLETE";
                        case IRP_MN_COMPRESSED:       return L"IRP_MN_COMPRESSED";
                        case IRP_MN_MDL_DPC:          return L"IRP_MN_MDL_DPC";
                        case IRP_MN_COMPLETE_MDL:     return L"IRP_MN_COMPLETE_MDL";
                        case IRP_MN_COMPLETE_MDL_DPC: return L"IRP_MN_COMPLETE_MDL_DPC";
                    }
                    break;

                case IRP_MJ_DEVICE_CONTROL:
                case IRP_MJ_INTERNAL_DEVICE_CONTROL:
                    switch (minor) {
                        case IRP_MN_SCSI_CLASS: return L"IRP_MN_SCSI_CLASS";
                    }
                    break;

                case IRP_MJ_PNP:
                    switch (minor) {
                        case IRP_MN_START_DEVICE:                 return L"IRP_MN_START_DEVICE";
                        case IRP_MN_QUERY_REMOVE_DEVICE:          return L"IRP_MN_QUERY_REMOVE_DEVICE";
                        case IRP_MN_REMOVE_DEVICE:                return L"IRP_MN_REMOVE_DEVICE";
                        case IRP_MN_CANCEL_REMOVE_DEVICE:         return L"IRP_MN_CANCEL_REMOVE_DEVICE";
                        case IRP_MN_STOP_DEVICE:                  return L"IRP_MN_STOP_DEVICE";
                        case IRP_MN_QUERY_STOP_DEVICE:            return L"IRP_MN_QUERY_STOP_DEVICE";
                        case IRP_MN_CANCEL_STOP_DEVICE:           return L"IRP_MN_CANCEL_STOP_DEVICE";
                        case IRP_MN_QUERY_DEVICE_RELATIONS:       return L"IRP_MN_QUERY_DEVICE_RELATIONS";
                        case IRP_MN_QUERY_INTERFACE:              return L"IRP_MN_QUERY_INTERFACE";
                        case IRP_MN_QUERY_CAPABILITIES:           return L"IRP_MN_QUERY_CAPABILITIES";
                        case IRP_MN_QUERY_RESOURCES:              return L"IRP_MN_QUERY_RESOURCES";
                        case IRP_MN_QUERY_RESOURCE_REQUIREMENTS:  return L"IRP_MN_QUERY_RESOURCE_REQUIREMENTS";
                        case IRP_MN_QUERY_DEVICE_TEXT:            return L"IRP_MN_QUERY_DEVICE_TEXT";
                        case IRP_MN_FILTER_RESOURCE_REQUIREMENTS: return L"IRP_MN_FILTER_RESOURCE_REQUIREMENTS";
                        case IRP_MN_READ_CONFIG:                  return L"IRP_MN_READ_CONFIG";
                        case IRP_MN_WRITE_CONFIG:                 return L"IRP_MN_WRITE_CONFIG";
                        case IRP_MN_EJECT:                        return L"IRP_MN_EJECT";
                        case IRP_MN_SET_LOCK:                     return L"IRP_MN_SET_LOCK";
                        case IRP_MN_QUERY_ID:                     return L"IRP_MN_QUERY_ID";
                        case IRP_MN_QUERY_PNP_DEVICE_STATE:       return L"IRP_MN_QUERY_PNP_DEVICE_STATE";
                        case IRP_MN_QUERY_BUS_INFORMATION:        return L"IRP_MN_QUERY_BUS_INFORMATION";
                        case IRP_MN_DEVICE_USAGE_NOTIFICATION:    return L"IRP_MN_DEVICE_USAGE_NOTIFICATION";
                        case IRP_MN_SURPRISE_REMOVAL:             return L"IRP_MN_SURPRISE_REMOVAL";
                        case IRP_MN_QUERY_LEGACY_BUS_INFORMATION: return L"IRP_MN_QUERY_LEGACY_BUS_INFORMATION";
                    }
                    break;

                case IRP_MJ_POWER:
                    switch (minor) {
                        case IRP_MN_WAIT_WAKE:      return L"IRP_MN_WAIT_WAKE";
                        case IRP_MN_POWER_SEQUENCE: return L"IRP_MN_POWER_SEQUENCE";
                        case IRP_MN_SET_POWER:      return L"IRP_MN_SET_POWER";
                        case IRP_MN_QUERY_POWER:    return L"IRP_MN_QUERY_POWER";
                    }
                    break;

                case IRP_MJ_SYSTEM_CONTROL:
                    switch (minor) {
                        case IRP_MN_QUERY_ALL_DATA:         return L"IRP_MN_QUERY_ALL_DATA";
                        case IRP_MN_QUERY_SINGLE_INSTANCE:  return L"IRP_MN_QUERY_SINGLE_INSTANCE";
                        case IRP_MN_CHANGE_SINGLE_INSTANCE: return L"IRP_MN_CHANGE_SINGLE_INSTANCE";
                        case IRP_MN_CHANGE_SINGLE_ITEM:     return L"IRP_MN_CHANGE_SINGLE_ITEM";
                        case IRP_MN_ENABLE_EVENTS:          return L"IRP_MN_ENABLE_EVENTS";
                        case IRP_MN_DISABLE_EVENTS:         return L"IRP_MN_DISABLE_EVENTS";
                        case IRP_MN_ENABLE_COLLECTION:      return L"IRP_MN_ENABLE_COLLECTION";
                        case IRP_MN_DISABLE_COLLECTION:     return L"IRP_MN_DISABLE_COLLECTION";
                        case IRP_MN_REGINFO:                return L"IRP_MN_REGINFO";
                        case IRP_MN_EXECUTE_METHOD:         return L"IRP_MN_EXECUTE_METHOD";
                    }
                    break;
            }

            return L"";
        }


        // WSL reparse tags live in kernel-only ntifs.h
        inline constexpr ULONG IO_REPARSE_TAG_LX_SYMLINK = 0xA000001Du;
        inline constexpr ULONG IO_REPARSE_TAG_LX_FIFO    = 0x80000024u;
        inline constexpr ULONG IO_REPARSE_TAG_LX_CHR     = 0x80000025u;
        inline constexpr ULONG IO_REPARSE_TAG_LX_BLK     = 0x80000026u;

        constexpr const wchar_t* ReparseTagName(ULONG tag) noexcept {

            switch (tag) {
                case IO_REPARSE_TAG_HSM2:         return L"HSM2";
                case IO_REPARSE_TAG_SIS:          return L"SIS";
                case IO_REPARSE_TAG_WIM:          return L"WIM";
                case IO_REPARSE_TAG_CSV:          return L"CSV";
                case IO_REPARSE_TAG_DFS:          return L"DFS";
                case IO_REPARSE_TAG_DFSR:         return L"DFSR";
                case IO_REPARSE_TAG_DEDUP:        return L"DEDUP";
                case IO_REPARSE_TAG_NFS:          return L"NFS";
                case IO_REPARSE_TAG_WOF:          return L"WOF";
                case IO_REPARSE_TAG_WCI:          return L"WCI";
                case IO_REPARSE_TAG_APPEXECLINK:  return L"APPEXECLINK";
                case IO_REPARSE_TAG_STORAGE_SYNC: return L"STORAGE_SYNC";
                case IO_REPARSE_TAG_AF_UNIX:      return L"AF_UNIX";
                case IO_REPARSE_TAG_LX_FIFO:      return L"LX_FIFO";
                case IO_REPARSE_TAG_LX_CHR:       return L"LX_CHR";
                case IO_REPARSE_TAG_LX_BLK:       return L"LX_BLK";
                case IO_REPARSE_TAG_PROJFS:       return L"PROJFS";
                case IO_REPARSE_TAG_MOUNT_POINT:  return L"MOUNT_POINT";
                case IO_REPARSE_TAG_SYMLINK:      return L"SYMLINK";
                case IO_REPARSE_TAG_LX_SYMLINK:   return L"LX_SYMLINK";
                case IO_REPARSE_TAG_HSM:          return L"HSM";
            }

            return L"";
        }


        constexpr const wchar_t* TransactionNotifyName(ULONG notification) noexcept {

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

            return L"";
        }


    }

}
