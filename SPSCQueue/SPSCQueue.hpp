#pragma once

/*
 * SPSCQueue.h
 *
 * This file contains the implementation of a single-producer single-consumer queue (SPSCQueue).
 * The original implementation is by Erik Rigtorp, available at:
 * https://github.com/rigtorp/SPSCQueue
 * 
 * License: MIT License (see LICENSE file in the project root for details)
 *
 * Author: Erik Rigtorp
 * GitHub: https://github.com/rigtorp
*/

#include <atomic>
#include <iostream>
#include <thread>
#include <vector>

static constexpr int CACHELINE_SIZE = 64;

template <typename T> 
struct SPSCQueue {
    std::vector<T> data_{};
    alignas(CACHELINE_SIZE) std::atomic<size_t> readIdx_{0};
    alignas(CACHELINE_SIZE) size_t writeIdxCached_{0};
    alignas(CACHELINE_SIZE) std::atomic<size_t> writeIdx_{0};
    alignas(CACHELINE_SIZE) size_t readIdxCached_{0};

    SPSCQueue(size_t capacity) { data_.resize(capacity); }

    bool push(T val) {
        auto const writeIdx = writeIdx_.load(std::memory_order_relaxed);
        auto nextWriteIdx = writeIdx + 1;
        if (nextWriteIdx == data_.size()) {
            nextWriteIdx = 0;
        }
        if (nextWriteIdx == readIdxCached_) {
            readIdxCached_ = readIdx_.load(std::memory_order_acquire);
            if (nextWriteIdx == readIdxCached_) {
                return false;
            }
        }
        data_[writeIdx] = val;
        writeIdx_.store(nextWriteIdx, std::memory_order_release);
        return true;
    }

    bool pop(T &val) {
        auto const readIdx = readIdx_.load(std::memory_order_relaxed);
        if (readIdx == writeIdxCached_) {
            writeIdxCached_ = writeIdx_.load(std::memory_order_acquire);
            if (readIdx == writeIdxCached_) {
                return false;
            }
        }
        val = data_[readIdx];
        auto nextReadIdx = readIdx + 1;
        if (nextReadIdx == data_.size()) {
            nextReadIdx = 0;
        }
        readIdx_.store(nextReadIdx, std::memory_order_release);
        return true;
    }
};


