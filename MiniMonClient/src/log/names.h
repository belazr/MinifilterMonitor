#pragma once

#include <Windows.h>

#include <string>

namespace mimo {

    namespace log {

        namespace names {

            std::wstring RenderOperationCategory(ULONG flags);

            std::wstring RenderMajorFunction(UCHAR major);

            std::wstring RenderMinorFunction(UCHAR major, UCHAR minor);

            std::wstring RenderReparseTag(ULONG tag);

            std::wstring RenderTransactionNotify(ULONG notification);

            std::wstring RenderCreateDisposition(ULONG disposition);

            std::wstring RenderOpenResult(ULONGLONG information);

            std::wstring RenderDesiredAccess(ULONG desiredAccess);

            std::wstring RenderCreateOptions(ULONG createOptions);

            std::wstring RenderShareAccess(ULONG shareAccess);

            std::wstring RenderFileAttributes(ULONG fileAttributes);

            std::wstring RenderByteOffset(LONGLONG byteOffset);

            std::wstring RenderFileInformationClass(ULONG fileInformationClass);

            std::wstring RenderCompressionFormat(USHORT format);

            std::wstring RenderRemoteProtocol(ULONG protocol);

            std::wstring RenderRemoteProtocolFlags(ULONG flags);

            std::wstring RenderDispositionFlags(ULONG flags);

            std::wstring RenderRenameFlags(ULONG flags);

        }

    }

}
