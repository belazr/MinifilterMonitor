#pragma once

#include <Windows.h>
#include <fltUser.h>

#include <utility>

namespace mimo {

    template <auto CloseFn, HANDLE sentinel = nullptr>
    class Handle {
    public:
        Handle() noexcept = default;
        explicit Handle(HANDLE h) noexcept : handle(h) {}

        ~Handle() { this->Close(); }

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


        HANDLE Get() const noexcept {

            return this->handle;
        }


        HANDLE Release() noexcept {

            return std::exchange(this->handle, sentinel);
        }


        HANDLE* Put() noexcept {
            this->Close();
            this->handle = sentinel;

            return &this->handle;
        }


        explicit operator bool() const noexcept {

            return this->handle != sentinel;
        }

    private:
        HANDLE handle = sentinel;

        void Close() noexcept {

            if (this->handle != sentinel) {
                CloseFn(this->handle);
            }

        }

    };

    inline LSTATUS CloseRegKey(HANDLE h) noexcept { return RegCloseKey(static_cast<HKEY>(h)); }

    using NullHandle = Handle<CloseHandle>;
    using InvHandle = Handle<CloseHandle, INVALID_HANDLE_VALUE>;
    using FltFindHandle = Handle<FilterVolumeFindClose, INVALID_HANDLE_VALUE>;
    using FltInstFindHandle = Handle<FilterVolumeInstanceFindClose, INVALID_HANDLE_VALUE>;
    using RegKeyHandle = Handle<CloseRegKey>;

}
