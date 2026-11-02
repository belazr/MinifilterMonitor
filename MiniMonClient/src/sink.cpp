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

    Sink::Sink(std::wstring_view header) : headerText(header) {}


    FileSink::FileSink(
        std::wstring path,
        bool split,
        std::wstring_view header
    ) : Sink(header), basePath(std::move(path)), splitByAltitude(split) {}


    bool FileSink::Write(std::wstring_view altitude, std::wstring_view line) {
        constexpr std::streamsize FILE_BUFFER_SIZE = 1024 * 1024;
        const std::wstring filePath = this->splitByAltitude && !altitude.empty() ? this->MakeSplitPath(altitude) : this->basePath;
        const auto [it, inserted] = this->files.try_emplace(filePath);

        if (inserted) {
            it->second.open(filePath, std::ios::binary);

            if (!it->second) {
                this->files.erase(it);

                return false;
            }

            it->second.rdbuf()->pubsetbuf(nullptr, FILE_BUFFER_SIZE);
            WriteCrlfLine(it->second, this->headerText);
        }

        WriteCrlfLine(it->second, line);

        return static_cast<bool>(it->second);
    }


    void FileSink::Flush() {

        for (std::pair<const std::wstring, std::ofstream>& entry : this->files) {
            entry.second.flush();
        }

        return;
    }


    void FileSink::WriteCrlfLine(std::ofstream& stream, std::wstring_view line) {
        std::string lineBytes = text::ConvertToUtf8(line);
        lineBytes.back() = '\r';
        lineBytes.push_back('\n');
        stream << lineBytes;

        return;
    }


    std::wstring FileSink::MakeSplitPath(std::wstring_view altitude) const {
        std::filesystem::path path{ this->basePath };
        const std::wstring extension = path.extension().wstring();
        path.replace_extension();

        return std::format(L"{}.{}{}", path.wstring(), altitude, extension);
    }

}
