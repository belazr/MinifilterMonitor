#pragma once

#include <Windows.h>
#include <fltUser.h>

#include <utility>

namespace mimo {

    template <auto CloseFn, HANDLE sentinel = nullptr>
    class Handle final {
    public:
        Handle() = default;

        explicit Handle(HANDLE h) : handle(h) {}


        ~Handle() {
            this->Close();
        }


        Handle(const Handle&) = delete;

        Handle& operator=(const Handle&) = delete;

        Handle(Handle&& h) noexcept : handle(std::exchange(h.handle, sentinel)) {}


        Handle& operator=(Handle&& h) noexcept {

            if (this != &h) {
                this->Close();
                this->handle = std::exchange(h.handle, sentinel);
            }

            return *this;
        }


        HANDLE Get() const {

            return this->handle;
        }


        HANDLE Release() {

            return std::exchange(this->handle, sentinel);
        }


        HANDLE* Put() {
            this->Close();
            this->handle = sentinel;

            return &this->handle;
        }


        explicit operator bool() const {

            return this->handle != sentinel;
        }

    private:
        HANDLE handle = sentinel;

        void Close() {

            if (this->handle != sentinel) {
                CloseFn(this->handle);
            }

        }

    };

    inline LSTATUS CloseRegKey(HANDLE h) {

        return RegCloseKey(static_cast<HKEY>(h));
    }


    using NullHandle = Handle<CloseHandle>;
    using InvHandle = Handle<CloseHandle, INVALID_HANDLE_VALUE>;
    using RegKeyHandle = Handle<CloseRegKey>;

}
