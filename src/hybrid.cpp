/**
 * Copyright (c) 2025 Mamadou Babaei
 *
 * Author: Mamadou Babaei <info@babaei.net>
 *
 */


#include <atomic>
#include <cstddef>
#include <cstdint>
#include <mutex>
#include <string>
#include <vector>

#include "lib.hpp"

class HybridSpinMutex
{
private:
    static constexpr std::size_t SpinCount = 100;
    thread_local static inline bool bUsedFallbackMutex;

private:
    std::atomic_flag Locked = ATOMIC_FLAG_INIT;
    std::mutex FallbackMutex;

public:
    void lock()
    {
        for (std::size_t i = 0; i < SpinCount; ++i) {
            if (!Locked.test_and_set(std::memory_order_acquire)) {
                bUsedFallbackMutex = false;
                return;
            }
            std::this_thread::yield();
        }
        FallbackMutex.lock();
        while (Locked.test_and_set(std::memory_order_acquire)) {
            /* busy-wait */
        }
        bUsedFallbackMutex = true;
    }

    void unlock()
    {
        Locked.clear(std::memory_order_release);
        if (bUsedFallbackMutex) {
            FallbackMutex.unlock();
        }
    }
};

class Logger
{
private:
    static inline HybridSpinMutex Mutex;
    static inline std::vector<std::string> Messages;

public:
    static void Log(const std::string& message)
    {
        std::lock_guard<HybridSpinMutex> lock{Mutex};
        Messages.emplace_back(message);
    }
};

#include "main.cpp"
