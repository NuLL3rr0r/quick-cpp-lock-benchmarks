/**
 * Copyright (c) 2025 Mamadou Babaei
 *
 * Author: Mamadou Babaei <info@babaei.net>
 *
 */


#pragma once

#include <charconv>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <numeric>
#include <sstream>
#include <system_error>
#include <thread>

#include <unistd.h>

static constexpr std::chrono::microseconds FAKE_WORK_SLEEP_TIME{100};

std::size_t ParseNumThreads(const int32_t argc, const char* argv[])
{
    if (argc > 1) {
        int32_t value = 0;
        std::from_chars_result result =
            std::from_chars(argv[1], argv[1] + std::strlen(argv[1]), value);

        if (result.ec == std::errc() && value > 0) {
            return static_cast<std::size_t>(value);
        }
    }

    return 128;
}

void EraseLines(const std::size_t count)
{
    if (count > 0) {
        std::cout << "\x1b[2K";
        for (std::size_t i = 1; i < count; i++) {
            std::cout << "\x1b[1A"
                      << "\x1b[2K";
        }
        std::cout << "\r";
    }
}

float GetSystemCpuUsage()
{
    static std::size_t previousIdleTime = 0;
    static std::size_t previousTotalTime = 0;

    std::ifstream statFile("/proc/stat");
    if (!statFile.is_open()) {
        std::cerr << "Failed to open /proc/stat" << std::endl;
        return -1.0f;
    }
    statFile.ignore(5, ' ');

    std::vector<std::size_t> cpuTimes;
    for (std::size_t time; statFile >> time; cpuTimes.emplace_back(time));
    if (cpuTimes.size() < 4) {
        return -1.0f;
    }

    const std::size_t idleTime = cpuTimes[3];
    const std::size_t totalTime =
        std::accumulate(cpuTimes.begin(), cpuTimes.end(), 0);

    const float idleTimeDelta =
        static_cast<float>(idleTime - previousIdleTime);
    const float totalTimeDelta =
        static_cast<float>(totalTime - previousTotalTime);

    const float utilization =
        100.0f * (1.0f - (idleTimeDelta / totalTimeDelta));

    previousIdleTime = idleTime;
    previousTotalTime = totalTime;

    return utilization;
}

int32_t GetCpuTemperature()
{
    for (const std::filesystem::directory_entry& entry
            : std::filesystem::directory_iterator("/sys/class/thermal")) {
        if (!entry.path().filename().string().starts_with("thermal_zone")) {
            continue;
        }

        std::ifstream typeFile(entry.path() / "type");
        if (!typeFile) {
            continue;
        }

        std::string type;
        if (!std::getline(typeFile, type)) {
            continue;
        }

        if (type.find("cpu") == std::string::npos &&
            type.find("x86_pkg_temp") == std::string::npos &&
            type.find("soc") == std::string::npos &&
            type.find("coretemp") == std::string::npos &&
            type.find("k10temp") == std::string::npos &&
            type.find("zenpower") == std::string::npos &&
            type.find("tdie") == std::string::npos &&
            type.find("tctl") == std::string::npos) {
            continue;
        }

        std::ifstream tempFile{entry.path() / "temp"};
        int32_t millidegrees;
        if (tempFile >> millidegrees) {
            return millidegrees / 1000;
        }
    }

    for (const std::filesystem::directory_entry& hwmon
        : std::filesystem::directory_iterator("/sys/class/hwmon")) {
        std::ifstream nameFile(hwmon.path() / "name");
        std::string deviceName;
        if (nameFile && std::getline(nameFile, deviceName)) {
            for (char& c : deviceName) c = std::tolower(c);
            if (deviceName.find("coretemp") == std::string::npos &&
                deviceName.find("k10temp") == std::string::npos &&
                deviceName.find("zenpower") == std::string::npos) {
                continue;
            }
        }

        for (const std::filesystem::directory_entry& file
            : std::filesystem::directory_iterator(hwmon.path())) {
            std::filesystem::path path = file.path();
            if (!path.filename().string().starts_with("temp") ||
                !path.filename().string().ends_with("_input")) {
                continue;
            }

            std::string base{path.filename().string()};
            std::string labelFile{
                (hwmon.path() / (base.substr(0, base.find("_input")) + "_label")).string()
            };

            std::string label;
            if (std::filesystem::exists(labelFile)) {
                std::ifstream lf(labelFile);
                std::getline(lf, label);
                for (char& c : label) {
                    c = std::tolower(c);
                }
            }

            if (label.empty() &&
                label.find("tdie") == std::string::npos &&
                label.find("tctl") == std::string::npos &&
                label.find("package id 0") != std::string::npos) {
                continue;
            }

            std::ifstream tf(path);
            int32_t millidegrees;
            if (tf >> millidegrees) {
                return millidegrees / 1000;
            }
        }
    }

    return -1;
}

void ReportCpuStats(const std::size_t numThreads)
{
    for(;;) {
        std::println(
            "• Threads {} • CPU Usage: {:.2f}% • CPU Temperature: {}°C",
            numThreads, GetSystemCpuUsage(), GetCpuTemperature());
        EraseLines(2);
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}
