#pragma once

#include <Windows.h>

#include <utility>

namespace mimo {

    template <auto CloseFn, typename T = HANDLE, T hSentinel = nullptr>
    class Handle final {
    public:
        Handle() = default;

        explicit Handle(T h) : hNative(h) {}


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


        T Get() const {

            return this->hNative;
        }


        T Release() {

            return std::exchange(this->hNative, hSentinel);
        }


        T* Put() {
            this->Close();
            this->hNative = hSentinel;

            return &this->hNative;
        }


        explicit operator bool() const {

            return this->hNative != hSentinel;
        }

    private:
        T hNative = hSentinel;

        void Close() {

            if (this->hNative != hSentinel) {
                CloseFn(this->hNative);
            }

            return;
        }

    };

    using NullHandle = Handle<CloseHandle>;
    using InvalidHandle = Handle<CloseHandle, HANDLE, INVALID_HANDLE_VALUE>;
    using RegistryKeyHandle = Handle<RegCloseKey, HKEY>;

}
