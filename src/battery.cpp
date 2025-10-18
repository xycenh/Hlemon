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

  updateLemonbar(lemonOutput());
  monitor();
}

void Battery::monitor() {
    while (true) {
    int new_capacity = readIntFile(CAPACITY_FILE);
    std::string new_status = readFile(STATUS_FILE);

    if (capacity != new_capacity || status != new_status) {
      capacity = new_capacity;
      status = new_status;
      updateLemonbar(lemonOutput());
    }

    std::this_thread::sleep_for(std::chrono::seconds(POLL_INTERVAL));
  }
}

std::string Battery::getBattery() {
    std::string icon = "";
		std::string color = "#ffffff";

		if (status == "Not") {
			icon = iconColor("ADP");
		} else if (status == "Discharging") {
			icon = iconColor("BAT");
		} else if (status == "Charging") {
			icon = iconColor("CHA");
		} else if (status == "Full") {
			icon = iconColor("FUL");
		}
		
		/*std::string format = " %{F" + color + "}" + icon + " " + std::to_string(capacity) + "% %{F-}";*/
    return "%{A:alacritty -e htop &:} " + icon + " " + std::to_string(capacity) + "% %{A}";
}
