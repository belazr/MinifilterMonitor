#include "counter.h"

#include <cstddef>
#include <cstdint>
#include <mutex>

namespace mimo {

    void Counter::AddCaptured(size_t capturedCount, uint32_t droppedTotal) {
        const std::lock_guard<std::mutex> lock{ this->mutex };
        this->counts.captured += capturedCount;
        this->counts.dropped = droppedTotal;

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
