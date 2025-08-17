/**
 * Copyright (c) 2025 Mamadou Babaei
 *
 * Author: Mamadou Babaei <info@babaei.net>
 *
 */


#include <atomic>
#include <string>
#include <vector>

#include "lib.hpp"

class Logger
{
private:
    static inline std::atomic_flag Locked = ATOMIC_FLAG_INIT;
    static inline std::vector<std::string> Messages;

public:
    static void Log(const std::string& message)
    {
        while (Locked.test_and_set(std::memory_order_acquire)) {
            /* busy-wait */
        }

        Messages.emplace_back(message);

        Locked.clear(std::memory_order_release);
    }
};

#include "main.cpp"
