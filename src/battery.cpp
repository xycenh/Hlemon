#include "battery.h"
#include "functions.h"
#include "lemonOutput.h"
#include <chrono>
#include <string>
#include <thread>

const char *STATUS_FILE = "/sys/class/power_supply/BAT0/status";
const char *CAPACITY_FILE = "/sys/class/power_supply/BAT0/capacity";

int Battery::capacity = -1;
std::string Battery::status = "Unknown";

Battery::Battery() {
  status = readFile(STATUS_FILE);
  capacity = readIntFile(CAPACITY_FILE);

  lemonOutput();
  monitor();
}

void Battery::monitor() {
    while (true) {

    int new_capacity = readIntFile(CAPACITY_FILE);
    std::string new_status = readFile(STATUS_FILE);

    if (capacity != new_capacity || status != new_status) {
      capacity = new_capacity;
      status = new_status;
      lemonOutput();
    }

    std::this_thread::sleep_for(std::chrono::seconds(POLL_INTERVAL));
  }
}

std::string Battery::getBattery() {
    std::string icon = "";
    if      (status == "Not"        )  icon = "ADP";
    else if (status == "Discharging")  icon = "BAT";
    else if (status == "Charging"   )  icon = "CHA";
    else if (status == "Full"       )  icon = "FUL";
    else                               icon = "UNK";
    return icon + ' ' + std::to_string(capacity) + '%';
}
