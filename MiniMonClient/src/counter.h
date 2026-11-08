#pragma once

#include <cstddef>
#include <mutex>

namespace mimo {

    struct Counts {
        size_t polls = 0u;
        size_t captured = 0u;
        size_t written = 0u;
        size_t dropped = 0u;
    };

    class Counter final {
    public:
        Counter() = default;

        Counter(const Counter&) = delete;

        Counter& operator=(const Counter&) = delete;

        void AddDrained(size_t drainedCount, size_t totalDroppedCount);

        void AddWritten(size_t writtenCount);

        Counts Get();

    private:
        std::mutex mutex;
        Counts counts;
    };

}
