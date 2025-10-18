#ifndef LAYOUT_H
#define LAYOUT_H

#include <X11/Xlib.h>
#include <X11/XKBlib.h>
#include <X11/extensions/XKBstr.h>
#include <string>
#include <thread>
#include <mutex>
#include <vector>  // Add this include for std::vector

class Keyboard {
public:
    Keyboard();
    ~Keyboard();

    // Access layout from anywhere
    static std::string get_layout();

private:
    void listen();
    std::string layout_from_group(unsigned group) const;

    Display* dpy;
    XkbDescPtr desc;
    XkbStateRec state;
    int xkbEventCode;
    int xkbErrorCode;

    static std::string currentLayout;
    static std::mutex layoutMutex;

    std::vector<std::string> groupLayouts;  // Add this declaration
};

#endif // LAYOUT_H
