#ifndef BATTERY_H
#define BATTERY_H

#include <string>
#include <sys/epoll.h>

#define POLL_INTERVAL 2 // seconds

class Battery {
private:
  static int capacity;
  static std::string status;
  void monitor();

public:
  Battery();
  static std::string getBattery();
};
#endif // !BATTERY_H
