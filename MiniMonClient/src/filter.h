#pragma once

#include "handle.h"

#include "..\..\inc\protocol.h"

#include <Windows.h>

#include <string>
#include <vector>

namespace mimo {

    namespace filter {

        HRESULT Attach(const std::wstring& volumeName);

        HRESULT AttachAll(const std::wstring& volumeName);

        HRESULT Unload();

        HRESULT Connect(InvalidHandle& port);

        HRESULT GetVersion(const InvalidHandle& port, protocol::Version& version);

        HRESULT GetRecords(const InvalidHandle& port, std::vector<protocol::Record>& records);

    }

}
