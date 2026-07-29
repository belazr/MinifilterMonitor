#pragma once

#include <cstdint>
#include <fstream>
#include <map>
#include <ostream>
#include <string>
#include <string_view>

namespace mimo {

    namespace log {

        class Sink {
        public:
            virtual ~Sink() = default;

            Sink(const Sink&) = delete;
            Sink& operator=(const Sink&) = delete;

            virtual bool Write(uint32_t altitude, std::wstring_view line) = 0;

            virtual void Flush() = 0;

        protected:
            explicit Sink(std::wstring_view headerText);

            std::wstring header;
        };


        class ConsoleSink final : public Sink {
            std::ostream& out;
            bool headerWritten;

        public:
            ConsoleSink(std::ostream& stream, std::wstring_view headerText);

            bool Write(uint32_t altitude, std::wstring_view line) override;

            void Flush() override;
        };


        class FileSink final : public Sink {
            std::wstring basePath;
            bool split;
            std::map<std::wstring, std::ofstream> files;

        public:
            FileSink(
                std::wstring path,
                bool splitByAltitude,
                std::wstring_view headerText
            );

            bool Write(uint32_t altitude, std::wstring_view line) override;

            void Flush() override;

        private:
            std::wstring MakeSplitPath(uint32_t altitude) const;
        };

    }

}
