#pragma once

#include <Windows.h>
#include <fltUser.h>

#include <utility>

namespace mimo {

    template <auto CloseFn, HANDLE hSentinel = nullptr>
    class Handle final {
    public:
        Handle() = default;

        explicit Handle(HANDLE h) : hNative(h) {}


        ~Handle() {
            this->Close();
        }


        Handle(const Handle&) = delete;

        Handle& operator=(const Handle&) = delete;

        Handle(Handle&& h) noexcept : hNative(std::exchange(h.hNative, hSentinel)) {}


        Handle& operator=(Handle&& h) noexcept {

            if (this != &h) {
                this->Close();
                this->hNative = std::exchange(h.hNative, hSentinel);
            }

            return *this;
        }


        HANDLE Get() const {

            return this->hNative;
        }


        HANDLE Release() {

            return std::exchange(this->hNative, hSentinel);
        }


        HANDLE* Put() {
            this->Close();
            this->hNative = hSentinel;

            return &this->hNative;
        }


        explicit operator bool() const {

            return this->hNative != hSentinel;
        }

    private:
        HANDLE hNative = hSentinel;

        void Close() {

            if (this->hNative != hSentinel) {
                CloseFn(this->hNative);
            }

        }

    };

    inline LSTATUS CloseRegistryKey(HANDLE h) {

        return RegCloseKey(static_cast<HKEY>(h));
    }


    using NullHandle = Handle<CloseHandle>;
    using InvalidHandle = Handle<CloseHandle, INVALID_HANDLE_VALUE>;
    using RegistryKeyHandle = Handle<CloseRegistryKey>;

}
