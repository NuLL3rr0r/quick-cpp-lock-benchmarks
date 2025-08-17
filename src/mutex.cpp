/**
 * Copyright (c) 2025 Mamadou Babaei
 *
 * Author: Mamadou Babaei <info@babaei.net>
 *
 */


#include <mutex>
#include <string>
#include <vector>

#include "lib.hpp"

class Logger
{
private:
    static inline std::mutex Mutex;
    static inline std::vector<std::string> Messages;

public:
    static void Log(const std::string& message)
    {
        std::lock_guard<std::mutex> lock{Mutex};
        Messages.emplace_back(message);
    }
};

#include "main.cpp"
