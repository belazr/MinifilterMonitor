#include "values.h"

#include "kernel.h"

#include <Windows.h>

#include <format>
#include <string>

namespace mimo {

    namespace trace {

        namespace values {

            std::wstring RenderBoolean(bool value) {

                return value ? L"True" : L"False";
            }


            std::wstring RenderByteOffset(int64_t byteOffset) {

                if (byteOffset == kernel::FILE_WRITE_TO_END_OF_FILE) return L"EOF";

                if (byteOffset == kernel::FILE_USE_FILE_POINTER_POSITION) return L"Current";

                return std::to_wstring(byteOffset);
            }


            std::wstring RenderFileTime(int64_t fileTime) {

                if (fileTime < 0) return std::to_wstring(fileTime);

                const ULONGLONG ticks = static_cast<ULONGLONG>(fileTime);

                FILETIME systemFileTime{};
                systemFileTime.dwLowDateTime = static_cast<DWORD>(ticks);
                systemFileTime.dwHighDateTime = static_cast<DWORD>(ticks >> 32);

                FILETIME localFileTime{};

                if (!FileTimeToLocalFileTime(&systemFileTime, &localFileTime)) return L"TIME ERROR";

                SYSTEMTIME localTime{};

                if (!FileTimeToSystemTime(&localFileTime, &localTime)) return L"TIME ERROR";

                return std::format(L"{:04}-{:02}-{:02} {:02}:{:02}:{:02}", localTime.wYear, localTime.wMonth, localTime.wDay, localTime.wHour, localTime.wMinute, localTime.wSecond);
            }

        }

    }

}
