#pragma once

#include <condition_variable>
#include <maix_image.hpp>
#include <memory>
#include <mutex>
#include <queue>

class FrameQueue
{
public:
    FrameQueue(size_t max_size = 1) : max_size(max_size)
    {
    }

    ~FrameQueue()
    {
        clear();
    }

    void push(std::shared_ptr<maix::image::Image> img)
    {
        std::unique_lock<std::mutex> lock(mtx);

        if (!queue.empty()) {
            queue.pop();
        }

        queue.push(img);
    }

    std::shared_ptr<maix::image::Image> pop_non_blocking()
    {
        std::lock_guard<std::mutex> lock(mtx);

        if (queue.empty()) {
            return nullptr;
        }

        auto img = queue.front();
        queue.pop();
        return img;
    }

    size_t size()
    {
        std::lock_guard<std::mutex> lock(mtx);
        return queue.size();
    }

    void clear()
    {
        std::lock_guard<std::mutex> lock(mtx);
        while (!queue.empty()) {
            queue.pop();
        }
    }

private:
    std::queue<std::shared_ptr<maix::image::Image>> queue;
    std::mutex mtx;
    size_t max_size;
};
