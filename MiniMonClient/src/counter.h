#pragma once

#include <cstddef>
#include <cstdint>
#include <mutex>

namespace mimo {

    struct Counts {
        size_t captured = 0u;
        size_t written = 0u;
        uint32_t dropped = 0u;
    };

    class Counter final {
    public:
        Counter() = default;

        Counter(const Counter&) = delete;

        Counter& operator=(const Counter&) = delete;

        void AddCaptured(size_t capturedCount, uint32_t droppedTotal);

        void AddWritten(size_t writtenCount);

        Counts Get();

    private:
        std::mutex mutex;
        Counts counts;
    };

}
