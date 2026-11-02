#pragma once

#include <fstream>
#include <map>
#include <string>
#include <string_view>

namespace mimo {

    class Sink final {
    public:
        explicit Sink(
            std::wstring path,
            bool split,
            std::wstring_view header
        );

        Sink(const Sink&) = delete;

        Sink& operator=(const Sink&) = delete;

        bool Write(std::wstring_view altitude, std::wstring_view line);

        void Flush();

    private:
        std::wstring basePath;
        bool splitByAltitude;
        std::wstring headerText;
        std::map<std::wstring, std::ofstream> files;

        static void WriteCrlfLine(std::ofstream& stream, std::wstring_view line);

        std::wstring MakeSplitPath(std::wstring_view altitude) const;
    };

}
