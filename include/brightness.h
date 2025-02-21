#ifndef BRIGHTNESS_H
#define BRIGHTNESS_H
#include <libudev.h>
#include <poll.h>
#include <string>

class Brightness {
public:
    static std::string getBrightness();
    Brightness();
    ~Brightness();

private:
    void startMonitoring();
    void stopMonitoring();
    struct udev *udev;
    struct udev_monitor *mon;
    struct pollfd fds[1];
    bool isMonitoring;
    static int max_brightness;
    static int actual_brightness;

    void initializeUdev();
    void setupMonitor();
    void monitorLoop();
    void cleanup();
};
#endif // BRIGHTNESS_H
