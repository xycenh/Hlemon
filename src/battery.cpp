#include "battery.h"
#include "functions.h"
#include "lemonOutput.h"
#include <iostream>
#include <string>
#include <sys/epoll.h>

const char *STATUS_FILE = "/sys/class/power_supply/BAT0/status";
const char *CAPACITY_FILE = "/sys/class/power_supply/BAT0/capacity";

int Battery::capacity = -1;
std::string Battery::status = "Unknown";

Battery::Battery() {
  if (epoll_fd == -1) {
    std::cerr << "Failed to create epoll instance." << std::endl;
  }

  status = readFile(STATUS_FILE);
  capacity = readIntFile(CAPACITY_FILE);

  lemonOutput();
  monitor();
}

void Battery::monitor() {
  while (true) {
    struct epoll_event event;
    int nfsd = epoll_wait(epoll_fd, &event, 1, POLL_INTERVAL * 1000);

    int new_capacity = readIntFile(CAPACITY_FILE);
    std::string new_status = readFile(STATUS_FILE);

    if (capacity != new_capacity) {
      capacity = new_capacity;
      lemonOutput();
    }
    if (status != new_status) {
      status = new_status;
      lemonOutput();
    }
  }
}

std::string Battery::getBattery() {
  return status + ' ' + std::to_string(capacity) + '%';
}
