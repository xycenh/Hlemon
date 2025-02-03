#include "battery.h"
#include "brightness.h"
#include <atomic>
#include <csignal>
#include <iostream>
#include <thread>
#include <vector>

std::atomic<bool> keepRunning(true);

void signalHandler(int signum) {
  std::cout << "Received signal " << signum << ". Shutting down..."
            << std::endl;
  keepRunning = false;
}

int main() {
  struct sigaction sigIntHandler;
  sigIntHandler.sa_handler = signalHandler;
  sigemptyset(&sigIntHandler.sa_mask);
  sigIntHandler.sa_flags = 0;
  sigaction(SIGINT, &sigIntHandler, NULL);
  sigaction(SIGTERM, &sigIntHandler, NULL);

  std::vector<std::thread> threads;

  threads.emplace_back([&]() {
    Battery battery;
    battery.monitor(keepRunning);
  });

  threads.emplace_back([&]() {
    Brightness brightness;
    brightness.startMonitoring(keepRunning);
  });

  // Wait for threads to complete
  for (auto &t : threads) {
    if (t.joinable()) {
      t.join();
    }
  }

  std::cout << "All threads terminated successfully." << std::endl;
  return 0;
}
