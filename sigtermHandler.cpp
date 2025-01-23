#include "sigtermHandler.hpp"

std::atomic<bool> sigterm_received = false;

void signalHandler(int signal) {
    if (signal == SIGTERM) {
        sigterm_received.store(true);
    }
}