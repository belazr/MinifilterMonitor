#pragma once

#include <condition_variable>
#include <cstddef>
#include <deque>
#include <mutex>
#include <optional>
#include <utility>

namespace mimo {

    template <typename T>
    class BlockingQueue final {
    public:
        explicit BlockingQueue(size_t itemCount) : maxItemCount(itemCount) {}


        BlockingQueue(const BlockingQueue&) = delete;

        BlockingQueue& operator=(const BlockingQueue&) = delete;

        bool Push(T&& item) {
            std::unique_lock<std::mutex> lock{ this->mutex };
            this->notFull.wait(lock, [this]() { return this->items.size() < this->maxItemCount || this->closed; });

            if (this->closed) return false;

            this->items.push_back(std::move(item));
            this->notEmpty.notify_one();

            return true;
        }


        std::optional<T> Pop() {
            std::unique_lock<std::mutex> lock{ this->mutex };
            this->notEmpty.wait(lock, [this]() { return !this->items.empty() || this->closed; });

            if (this->items.empty()) return std::nullopt;

            T item = std::move(this->items.front());
            this->items.pop_front();
            this->notFull.notify_one();

            return item;
        }


        void Close() {
            const std::lock_guard<std::mutex> lock{ this->mutex };
            this->closed = true;
            this->notFull.notify_all();
            this->notEmpty.notify_all();

            return;
        }

    private:
        std::mutex mutex;
        std::condition_variable notFull;
        std::condition_variable notEmpty;
        std::deque<T> items;
        size_t maxItemCount;
        bool closed = false;
    };

}
