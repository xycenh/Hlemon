#include "battery.h"
#include "brightness.h"
#include "lemonOutput.h"
#include "volume.h"
#include "mic.h"
#include "workspaces.h"
#include "clock.h"
#include "layout.h"
#include <csignal>
#include <cstdlib>
#include <iostream>
#include <thread>
#include <vector>

void signalHandler(int signum) {
  exit(signum);
}

int main() {
    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = signalHandler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, NULL);
    sigaction(SIGTERM, &sigIntHandler, NULL);

    initLemonbar();
    std::vector<std::thread> threads;

    // threads.emplace_back(lemonbarHandler);

    threads.emplace_back([&]() {
        Battery battery;
    });

    threads.emplace_back([&]() {
        Brightness brightness;
    });

    threads.emplace_back([&]() {
        Workspaces workspaces;
    });

    threads.emplace_back([&](){
        displayTime();
    });

    threads.emplace_back([&](){
        Volume volume;
    });

    threads.emplace_back([&](){
        Mic mic;
    });

    threads.emplace_back([&](){
        Keyboard layout;
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
