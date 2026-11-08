#include "sink.h"

#include "text.h"

#include <filesystem>
#include <format>
#include <fstream>
#include <ios>
#include <string>
#include <string_view>
#include <utility>

namespace mimo {

    Sink::Sink(
        std::wstring path,
        bool split,
        std::wstring_view header
    ) : basePath(std::move(path)), splitByAltitude(split), headerText(header) {}


    bool Sink::Open() {

        return static_cast<bool>(this->OpenFile(std::wstring_view{}));
    }


    bool Sink::Write(std::wstring_view altitude, std::wstring_view line) {
        std::ofstream* const pStream = this->OpenFile(this->splitByAltitude ? altitude : std::wstring_view{});

        if (!pStream) return false;

        WriteCrlfLine(*pStream, line);

        return static_cast<bool>(*pStream);
    }


    bool Sink::Flush() {
        bool flushed = true;

        for (std::pair<const std::wstring, std::ofstream>& entry : this->files) {

            if (!entry.second.flush()) {
                flushed = false;
            }

        }

        return flushed;
    }


    void Sink::WriteCrlfLine(std::ofstream& stream, std::wstring_view line) {
        std::string lineBytes = text::ConvertToUtf8(line);
        lineBytes.back() = '\r';
        lineBytes.push_back('\n');
        stream << lineBytes;

        return;
    }


    std::ofstream* Sink::OpenFile(std::wstring_view altitude) {
        constexpr std::streamsize FILE_BUFFER_SIZE = 1024 * 1024;
        auto it = this->files.find(altitude);

        if (it != this->files.end()) return &it->second;

        it = this->files.try_emplace(std::wstring{ altitude }).first;
        it->second.open(altitude.empty() ? this->basePath : this->MakeSplitPath(altitude), std::ios::binary);

        if (!it->second) {
            this->files.erase(it);

            return nullptr;
        }

        it->second.rdbuf()->pubsetbuf(nullptr, FILE_BUFFER_SIZE);
        WriteCrlfLine(it->second, this->headerText);

        return &it->second;
    }


    std::wstring Sink::MakeSplitPath(std::wstring_view altitude) const {
        std::filesystem::path path{ this->basePath };
        const std::wstring extension = path.extension().wstring();
        path.replace_extension();

        return std::format(L"{}.{}{}", path.wstring(), altitude, extension);
    }

}
