#pragma once

#include "buffer.h"
#include "handle.h"

#include <Windows.h>

#include <string>

namespace mimo {

    namespace filter {

        HRESULT Attach(const std::wstring& volumeName);

        HRESULT AttachAll(const std::wstring& volumeName);

        HRESULT Unload();

        HRESULT Connect(InvalidHandle& port);

        HRESULT GetRecords(const InvalidHandle& port, AlignedBuffer& records);

    }

}
