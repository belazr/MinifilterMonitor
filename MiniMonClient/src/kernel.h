#pragma once

#include <Windows.h>

namespace mimo {

    namespace kernel {

        struct FlagName {
            ULONG flag;
            const wchar_t* name;
        };

        // operation category flags carried in RecordData::flags

        inline constexpr ULONG FLT_CALLBACK_DATA_IRP_OPERATION       = 0x00000001u;
        inline constexpr ULONG FLT_CALLBACK_DATA_FAST_IO_OPERATION   = 0x00000002u;
        inline constexpr ULONG FLT_CALLBACK_DATA_FS_FILTER_OPERATION = 0x00000004u;

        // IRP flags carried in RecordData::irpFlags

        inline constexpr ULONG IRP_NOCACHE                 = 0x00000001u;
        inline constexpr ULONG IRP_PAGING_IO               = 0x00000002u;
        inline constexpr ULONG IRP_SYNCHRONOUS_API         = 0x00000004u;
        inline constexpr ULONG IRP_SYNCHRONOUS_PAGING_IO   = 0x00000040u;

        // stack-location SL_* flags carried in RecordData::operationFlags

        inline constexpr UCHAR SL_KEY_SPECIFIED = 0x01u;
        inline constexpr UCHAR SL_WRITE_THROUGH = 0x04u;

        // read/write ByteOffset sentinels, the 64-bit form of the wdm.h LowPart values with HighPart -1

        inline constexpr LONGLONG FILE_WRITE_TO_END_OF_FILE      = -1;
        inline constexpr LONGLONG FILE_USE_FILE_POINTER_POSITION = -2;

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


        inline constexpr ULONG FILE_SUPERSEDE    = 0x00000000u;
        inline constexpr ULONG FILE_OPEN         = 0x00000001u;
        inline constexpr ULONG FILE_CREATE       = 0x00000002u;
        inline constexpr ULONG FILE_OPEN_IF      = 0x00000003u;
        inline constexpr ULONG FILE_OVERWRITE    = 0x00000004u;
        inline constexpr ULONG FILE_OVERWRITE_IF = 0x00000005u;

        constexpr const wchar_t* CreateDispositionName(ULONG disposition) noexcept {

            switch (disposition) {
                case FILE_SUPERSEDE:    return L"Supersede";
                case FILE_OPEN:         return L"Open";
                case FILE_CREATE:       return L"Create";
                case FILE_OPEN_IF:      return L"OpenIf";
                case FILE_OVERWRITE:    return L"Overwrite";
                case FILE_OVERWRITE_IF: return L"OverwriteIf";
            }

            return L"";
        }


        inline constexpr ULONGLONG FILE_SUPERSEDED     = 0u;
        inline constexpr ULONGLONG FILE_OPENED         = 1u;
        inline constexpr ULONGLONG FILE_CREATED        = 2u;
        inline constexpr ULONGLONG FILE_OVERWRITTEN    = 3u;
        inline constexpr ULONGLONG FILE_EXISTS         = 4u;
        inline constexpr ULONGLONG FILE_DOES_NOT_EXIST = 5u;

        constexpr const wchar_t* OpenResultName(ULONGLONG information) noexcept {

            switch (information) {
                case FILE_SUPERSEDED:     return L"Superseded";
                case FILE_OPENED:         return L"Opened";
                case FILE_CREATED:        return L"Created";
                case FILE_OVERWRITTEN:    return L"Overwritten";
                case FILE_EXISTS:         return L"Exists";
                case FILE_DOES_NOT_EXIST: return L"Does Not Exist";
            }

            return L"";
        }


        inline constexpr ULONG FILE_DIRECTORY_FILE            = 0x00000001u;
        inline constexpr ULONG FILE_WRITE_THROUGH             = 0x00000002u;
        inline constexpr ULONG FILE_SEQUENTIAL_ONLY           = 0x00000004u;
        inline constexpr ULONG FILE_NO_INTERMEDIATE_BUFFERING = 0x00000008u;
        inline constexpr ULONG FILE_SYNCHRONOUS_IO_ALERT      = 0x00000010u;
        inline constexpr ULONG FILE_SYNCHRONOUS_IO_NONALERT   = 0x00000020u;
        inline constexpr ULONG FILE_NON_DIRECTORY_FILE        = 0x00000040u;
        inline constexpr ULONG FILE_CREATE_TREE_CONNECTION    = 0x00000080u;
        inline constexpr ULONG FILE_COMPLETE_IF_OPLOCKED      = 0x00000100u;
        inline constexpr ULONG FILE_NO_EA_KNOWLEDGE           = 0x00000200u;
        inline constexpr ULONG FILE_OPEN_REMOTE_INSTANCE      = 0x00000400u;
        inline constexpr ULONG FILE_RANDOM_ACCESS             = 0x00000800u;
        inline constexpr ULONG FILE_DELETE_ON_CLOSE           = 0x00001000u;
        inline constexpr ULONG FILE_OPEN_BY_FILE_ID           = 0x00002000u;
        inline constexpr ULONG FILE_OPEN_FOR_BACKUP_INTENT    = 0x00004000u;
        inline constexpr ULONG FILE_NO_COMPRESSION            = 0x00008000u;
        inline constexpr ULONG FILE_OPEN_REQUIRING_OPLOCK     = 0x00010000u;
        inline constexpr ULONG FILE_DISALLOW_EXCLUSIVE        = 0x00020000u;
        inline constexpr ULONG FILE_SESSION_AWARE             = 0x00040000u;
        inline constexpr ULONG FILE_RESERVE_OPFILTER          = 0x00100000u;
        inline constexpr ULONG FILE_OPEN_REPARSE_POINT        = 0x00200000u;
        inline constexpr ULONG FILE_OPEN_NO_RECALL            = 0x00400000u;
        inline constexpr ULONG FILE_OPEN_FOR_FREE_SPACE_QUERY = 0x00800000u;

        // composites precede their components
        inline constexpr FlagName DESIRED_ACCESS_NAMES[]{
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

        inline constexpr FlagName CREATE_OPTION_NAMES[]{
            { FILE_DIRECTORY_FILE,            L"Directory" },
            { FILE_WRITE_THROUGH,             L"Write Through" },
            { FILE_SEQUENTIAL_ONLY,           L"Sequential Access" },
            { FILE_NO_INTERMEDIATE_BUFFERING, L"No Buffering" },
            { FILE_SYNCHRONOUS_IO_ALERT,      L"Synchronous IO Alert" },
            { FILE_SYNCHRONOUS_IO_NONALERT,   L"Synchronous IO Non-Alert" },
            { FILE_NON_DIRECTORY_FILE,        L"Non-Directory File" },
            { FILE_CREATE_TREE_CONNECTION,    L"Create Tree Connection" },
            { FILE_COMPLETE_IF_OPLOCKED,      L"Complete If Oplocked" },
            { FILE_NO_EA_KNOWLEDGE,           L"No EA Knowledge" },
            { FILE_OPEN_REMOTE_INSTANCE,      L"Open Remote Instance" },
            { FILE_RANDOM_ACCESS,             L"Random Access" },
            { FILE_DELETE_ON_CLOSE,           L"Delete On Close" },
            { FILE_OPEN_BY_FILE_ID,           L"Open By ID" },
            { FILE_OPEN_FOR_BACKUP_INTENT,    L"Open For Backup" },
            { FILE_NO_COMPRESSION,            L"No Compression" },
            { FILE_OPEN_REQUIRING_OPLOCK,     L"Open Requiring Oplock" },
            { FILE_DISALLOW_EXCLUSIVE,        L"Disallow Exclusive" },
            { FILE_SESSION_AWARE,             L"Session Aware" },
            { FILE_RESERVE_OPFILTER,          L"Reserve OpFilter" },
            { FILE_OPEN_REPARSE_POINT,        L"Open Reparse Point" },
            { FILE_OPEN_NO_RECALL,            L"Open No Recall" },
            { FILE_OPEN_FOR_FREE_SPACE_QUERY, L"Open For Free Space Query" },
        };

        inline constexpr FlagName SHARE_ACCESS_NAMES[]{
            { FILE_SHARE_READ,   L"Read" },
            { FILE_SHARE_WRITE,  L"Write" },
            { FILE_SHARE_DELETE, L"Delete" },
        };

        inline constexpr FlagName FILE_ATTRIBUTE_LETTERS[]{
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

        static_assert(CompositesPrecedeComponents(DESIRED_ACCESS_NAMES), "composite entry after its components in DESIRED_ACCESS_NAMES");
        static_assert(CompositesPrecedeComponents(CREATE_OPTION_NAMES), "composite entry after its components in CREATE_OPTION_NAMES");
        static_assert(CompositesPrecedeComponents(SHARE_ACCESS_NAMES), "composite entry after its components in SHARE_ACCESS_NAMES");
        static_assert(CompositesPrecedeComponents(FILE_ATTRIBUTE_LETTERS), "composite entry after its components in FILE_ATTRIBUTE_LETTERS");

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
