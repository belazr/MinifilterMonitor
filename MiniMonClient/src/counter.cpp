#include "counter.h"

#include <cstddef>
#include <mutex>

namespace mimo {

    void Counter::AddDrained(size_t drainedCount, size_t totalDroppedCount) {
        const std::lock_guard<std::mutex> lock{ this->mutex };
        this->counts.polls++;
        this->counts.captured += drainedCount;
        this->counts.dropped = totalDroppedCount;

        return;
    }


    void Counter::AddWritten(size_t writtenCount) {
        const std::lock_guard<std::mutex> lock{ this->mutex };
        this->counts.written += writtenCount;

        return;
    }


    Counts Counter::Get() {
        const std::lock_guard<std::mutex> lock{ this->mutex };

        return this->counts;
    }

}
