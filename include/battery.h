#ifndef BATTERY_H
#define BATTERY_H

#include <string>
#include <sys/epoll.h>

#define POLL_INTERVAL 2 // seconds

class Battery {
private:
  int epoll_fd = epoll_create1(0);
  static int capacity;
  static std::string status;

public:
  Battery();
  void monitor();
  static std::string getBattery();
};

#endif // !BATTERY_H
