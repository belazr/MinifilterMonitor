#pragma once

#include "..\handle.h"

#include <Windows.h>
#include <winternl.h>

#include <string>
#include <string_view>

namespace mimo {

    namespace test {

        class Target final {
        public:
            explicit Target(std::wstring_view ntDirectory, std::wstring_view name);

            Target(const Target&) = delete;

            Target& operator=(const Target&) = delete;

            NTSTATUS Create(ACCESS_MASK desiredAccess);

            HANDLE Get() const;

        private:
            std::wstring path;
            Handle<NtClose> file;
        };

    }

}
