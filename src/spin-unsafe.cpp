/**
 * Copyright (c) 2025 Mamadou Babaei
 *
 * Author: Mamadou Babaei <info@babaei.net>
 *
 */


#include <string>
#include <vector>

#include "lib.hpp"

class Logger
{
private:
    static inline bool bLocked = false;
    static inline std::vector<std::string> Messages;

public:
    static void Log(const std::string& message)
    {
        while (bLocked) {
            /* busy-wait */
        }
        bLocked = true;

        Messages.emplace_back(message);

        bLocked = false;
    }
};

#include "main.cpp"
