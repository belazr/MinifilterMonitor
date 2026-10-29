#pragma once

#include <fstream>
#include <map>
#include <ostream>
#include <string>
#include <string_view>

namespace mimo {

    class Sink {
    public:
        virtual ~Sink() = default;

        Sink(const Sink&) = delete;

        Sink& operator=(const Sink&) = delete;

        virtual bool Write(std::wstring_view altitude, std::wstring_view line) = 0;

        virtual void Flush() = 0;

    protected:
        explicit Sink(std::wstring_view header);

        std::wstring headerText;
    };

    class ConsoleSink final : public Sink {
    public:
        ConsoleSink(std::ostream& stream, std::wstring_view header);

        bool Write(std::wstring_view altitude, std::wstring_view line) override;

        void Flush() override;

    private:
        std::ostream& outputStream;
        bool headerWritten = false;
    };

    class FileSink final : public Sink {
    public:
        FileSink(
            std::wstring path,
            bool split,
            std::wstring_view header
        );

        bool Write(std::wstring_view altitude, std::wstring_view line) override;

        void Flush() override;

    private:
        std::wstring basePath;
        bool splitByAltitude;
        std::map<std::wstring, std::ofstream> files;

        std::wstring MakeSplitPath(std::wstring_view altitude) const;
    };

}
