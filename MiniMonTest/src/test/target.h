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
            static constexpr std::wstring_view NAME = L"file";

            explicit Target(std::wstring_view ntDirectory);

            Target(const Target&) = delete;

            Target& operator=(const Target&) = delete;

            NTSTATUS Open(ACCESS_MASK desiredAccess);

            HANDLE Get() const;

        private:
            std::wstring path;
            Handle<NtClose> file;
        };

    }

}
