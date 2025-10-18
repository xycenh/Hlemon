#include "brightness.h"
#include "functions.h"
#include "lemonOutput.h"
#include <climits>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <fcntl.h>
#include <iostream>
#include <unistd.h>

const char *MAX_BRIGHTNESS_FILE =
    "/sys/class/backlight/intel_backlight/max_brightness";
const char *ACTUAL_BRIGHTNESS_FILE =
    "/sys/class/backlight/intel_backlight/brightness";

int Brightness::max_brightness = -1;
int Brightness::actual_brightness = -1;

Brightness::Brightness() : udev(nullptr), mon(nullptr), isMonitoring(false) {
  max_brightness = readIntFile(MAX_BRIGHTNESS_FILE);
  actual_brightness = readIntFile(ACTUAL_BRIGHTNESS_FILE);
  updateLemonbar(lemonOutput());
  startMonitoring();
}

Brightness::~Brightness() { stopMonitoring(); }

void Brightness::initializeUdev() {
  udev = udev_new();
  if (!udev) {
    throw std::runtime_error("Failed to create udev context");
  }
}

void Brightness::setupMonitor() {
  mon = udev_monitor_new_from_netlink(udev, "kernel");
  if (!mon) {
    udev_unref(udev);
    throw std::runtime_error("Failed to create udev monitor");
  }

  udev_monitor_filter_add_match_subsystem_devtype(mon, "backlight", NULL);
  udev_monitor_enable_receiving(mon);

  int fd = udev_monitor_get_fd(mon);
  fds[0].fd = fd;
  fds[0].events = POLLIN;
}

void Brightness::monitorLoop() {
  while (true) {

    int timeout_ms = 500; // Reduced timeout
    int ret = poll(fds, 1, timeout_ms);
    if (ret > 0 && (fds[0].revents & POLLIN)) {
      struct udev_device *dev = udev_monitor_receive_device(mon);
      if (dev) {
        updateLemonbar(lemonOutput());
        udev_device_unref(dev);
      }
    }
  }
  stopMonitoring();
}

void Brightness::cleanup() {
  if (mon) {
    udev_monitor_unref(mon);
    mon = nullptr;
  }

  if (udev) {
    udev_unref(udev);
    udev = nullptr;
  }
}

void Brightness::startMonitoring() {
  try {
    initializeUdev();
    setupMonitor();
    monitorLoop();
  } catch (const std::exception &e) {
    std::cerr << "Error in monitoring: " << e.what() << std::endl;
    stopMonitoring();
  }
}

void Brightness::stopMonitoring() {
  isMonitoring = false;
  cleanup();
}

std::string Brightness::getBrightness() {
    actual_brightness = readIntFile(ACTUAL_BRIGHTNESS_FILE);
    max_brightness = readIntFile(MAX_BRIGHTNESS_FILE);
		int brightness = ((actual_brightness + 10) * 100) / max_brightness;
		std::string icon = iconColor("BRI");

    return 
        "%{A4:brightnessctl set +1%:}"
        "%{A5:brightnessctl set 1%-:}"
        " "
        + icon
        + " "
        + std::to_string(brightness) + "% "
        "%{A}%{A}";
}
