#pragma once

#include "handle.h"

#include "..\..\inc\protocol.h"

#include <Windows.h>

#include <span>
#include <string>

namespace mimo {

    namespace filter {

        HRESULT Load();

        HRESULT Attach(std::span<const std::wstring> volumeNames);

        HRESULT AttachAll();

        HRESULT Unload();

        HRESULT Connect(InvalidHandle& port);

        HRESULT GetVersion(const InvalidHandle& port, protocol::Version& version);

        HRESULT GetRecords(const InvalidHandle& port, std::span<protocol::Record>& records);

    }

}
