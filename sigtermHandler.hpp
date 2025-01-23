#pragma once
#include <atomic>
#include <csignal>

extern std::atomic<bool> sigterm_received;

void signalHandler(int signal);

inline bool keep_running() {
    return !sigterm_received.load();
}