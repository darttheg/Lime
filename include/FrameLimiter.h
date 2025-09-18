#pragma once
#include <chrono>
#include <thread>

class FrameLimiter {
    using clock = std::chrono::steady_clock;

    double targetFPS = 60.0;
    clock::duration frameDur{};
    clock::time_point prev{}, next{};

    explicit FrameLimiter(double fps = 60.0) { setFPS(fps); prev = next = clock::now(); }

    void setFPS(double fps) {
        targetFPS = fps;
        frameDur = (fps > 0.0) ? std::chrono::duration_cast<clock::duration>(std::chrono::duration<double>(1.0 / fps)) : clock::duration::zero();
        prev = next = clock::now();
    }

    double beginFrame() {
        auto now = clock::now();
        double dt = std::chrono::duration<double>(now - prev).count();
        prev = now;
        return dt;
    }

    void endFrame() {
        if (targetFPS <= 0.0) return;
        next += frameDur;
        std::this_thread::sleep_until(next);
        if (clock::now() - next > frameDur * 3) next = clock::now();
    }
};