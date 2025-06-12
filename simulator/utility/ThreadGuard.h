#pragma once
#include <vector>
#include <thread>
class ThreadGuard {
    std::vector<std::thread>& threads;
public:
    explicit ThreadGuard(std::vector<std::thread>& threads) : threads(threads) {}
    ~ThreadGuard() {
        for (auto& t : threads) {
            if (t.joinable()) {
                t.join();
            }
        }
    }
};
