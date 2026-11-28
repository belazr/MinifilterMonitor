#pragma once

#include <Windows.h>
#include <winternl.h>

#include <string_view>

namespace mimo {

    namespace test {

        struct Description {
            std::string_view major;
            std::wstring_view name;
            NTSTATUS expectedStatus;
        };

        bool Run(std::wstring_view directory);

    }

}
