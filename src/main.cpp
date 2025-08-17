/**
 * Copyright (c) 2025 Mamadou Babaei
 *
 * Author: Mamadou Babaei <info@babaei.net>
 *
 */


#include <cstdint>
#include <cstdlib>
#include <thread>
#include <vector>

int32_t main(const int32_t argc, const char* argv[])
{
    std::size_t numThreads = ParseNumThreads(argc, argv);

    std::vector<std::thread> threads;

    for (int32_t t = 0; t < numThreads; ++t) {
        threads.emplace_back([&] {
            for (;;) {
                Logger::Log("hello");

                // We create fake work here, since we don't want this outer
                // loop to keep the CPU busy and affect the benchmark too much!
                std::this_thread::sleep_for(FAKE_WORK_SLEEP_TIME);
            }
        });
    }

    threads.emplace_back([&] {
        ReportCpuStats(numThreads);
    });

    for (std::thread& th : threads) {
        th.join();
    }

    return EXIT_SUCCESS;
}
