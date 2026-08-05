#include "sink.h"

#include "text.h"

#include <filesystem>
#include <format>
#include <utility>

namespace mimo {

    Sink::Sink(std::wstring_view headerText) : header(headerText) {}


    ConsoleSink::ConsoleSink(std::ostream& stream, std::wstring_view headerText) : Sink(headerText), out(stream), headerWritten(false) {}


    bool ConsoleSink::Write(uint32_t, std::wstring_view line) {

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


    bool FileSink::Write(uint32_t altitude, std::wstring_view line) {
        const std::wstring filePath = this->split ? this->MakeSplitPath(altitude) : this->basePath;
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


    std::wstring FileSink::MakeSplitPath(uint32_t altitude) const {
        std::filesystem::path path{ this->basePath };
        const std::wstring extension = path.extension().wstring();
        path.replace_extension();

        return std::format(L"{}.{}{}", path.wstring(), altitude, extension);
    }

}
