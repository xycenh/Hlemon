#ifndef BRIGHTNESS_H
#define BRIGHTNESS_H
#include <atomic>
#include <libudev.h>
#include <poll.h>

class Brightness {
public:
  Brightness();
  ~Brightness();
  void startMonitoring(std::atomic<bool> &running);
  void stopMonitoring();
  static int getBrightness();

private:
  struct udev *udev;
  struct udev_monitor *mon;
  struct pollfd fds[1];
  bool isMonitoring;
  static int max_brightness;
  static int actual_brightness;

  void initializeUdev();
  void setupMonitor();
  void monitorLoop(std::atomic<bool> &running);
  void cleanup();
};
#endif // BRIGHTNESS_H
