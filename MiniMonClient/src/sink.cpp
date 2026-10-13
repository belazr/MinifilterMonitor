#include "sink.h"

#include "text.h"

#include <filesystem>
#include <format>
#include <fstream>
#include <ostream>
#include <string>
#include <string_view>
#include <utility>

namespace mimo {

    Sink::Sink(std::wstring_view headerText) : header(headerText) {}


    ConsoleSink::ConsoleSink(std::ostream& stream, std::wstring_view headerText) : Sink(headerText), out(stream) {}


    bool ConsoleSink::Write(std::wstring_view, std::wstring_view line) {

        if (!this->headerWritten) {
            this->out << text::ConvertToUtf8(this->header);
            this->headerWritten = true;
        }

        this->out << text::ConvertToUtf8(line);

        return static_cast<bool>(this->out);
    }


    void ConsoleSink::Flush() {
        this->out.flush();

        return;
    }


    FileSink::FileSink(
        std::wstring path,
        bool splitByAltitude,
        std::wstring_view headerText
    ) : Sink(headerText), basePath(std::move(path)), split(splitByAltitude) {}


    bool FileSink::Write(std::wstring_view altitude, std::wstring_view line) {
        const std::wstring filePath = this->split && !altitude.empty() ? this->MakeSplitPath(altitude) : this->basePath;
        const auto [it, inserted] = this->files.try_emplace(filePath);

        if (inserted) {
            it->second.open(filePath);

            if (!it->second) {
                this->files.erase(it);

                return false;
            }

            it->second << text::ConvertToUtf8(this->header);
        }

        it->second << text::ConvertToUtf8(line);

        return static_cast<bool>(it->second);
    }


    void FileSink::Flush() {

        for (std::pair<const std::wstring, std::ofstream>& entry : this->files) {
            entry.second.flush();
        }

        return;
    }


    std::wstring FileSink::MakeSplitPath(std::wstring_view altitude) const {
        std::filesystem::path path{ this->basePath };
        const std::wstring extension = path.extension().wstring();
        path.replace_extension();

        return std::format(L"{}.{}{}", path.wstring(), altitude, extension);
    }

}
