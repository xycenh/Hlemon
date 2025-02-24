#include "battery.h"
#include "brightness.h"
#include "workspaces.h"
#include "functions.h"
#include "clock.h"
#include <csignal>
#include <cstdlib>
#include <iostream>
#include <thread>
#include <vector>

void signalHandler(int signum) {
  exit(signum);
}

// void lemonbarHandler() {
//     FILE *pipe = popen("./lemon | lemonbar -f 'monospace' -B '#000000'", "r");
//     if (!pipe) {
//         std::cerr << "Failed to start lemonbar" << std::endl;
//         return;
//     }

//     char command[256];
//     while (fgets(command, sizeof(command), pipe)) {
//         std::string cmdStr(command);
//         if (cmdStr.find("bspc") != std::string::npos) { // ✅ Only allow bspc commands
//             system(cmdStr.c_str());
//         }
//     }

//     pclose(pipe);
// }

int main() {
    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = signalHandler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, NULL);
    sigaction(SIGTERM, &sigIntHandler, NULL);

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


    // Wait for threads to complete
    for (auto &t : threads) {
        if (t.joinable()) {
        t.join();
        }
    }

    std::cout << "All threads terminated successfully." << std::endl;
    return 0;
}
