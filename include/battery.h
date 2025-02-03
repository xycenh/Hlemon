#ifndef BATTERY_H
#define BATTERY_H

#include <atomic>
#include <string>
#include <sys/epoll.h>

#define POLL_INTERVAL 2 // seconds

class Battery {
private:
  static int capacity;
  static std::string status;

public:
  Battery();
  void monitor(const std::atomic<bool> &running = true);
  static std::string getBattery();
};
#endif // !BATTERY_H
