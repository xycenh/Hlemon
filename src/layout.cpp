#include "layout.h"
#include "lemonOutput.h"
#include <cstdio>
#include <iostream>
#include <iterator>
#include <vector>
#include <string>
#include <mutex>
#include <X11/extensions/XKBrules.h>  // Add this for XkbRF_GetNamesProp

// static variables
std::string Keyboard::currentLayout = "unknown";
std::mutex Keyboard::layoutMutex;

Keyboard::Keyboard() {
    
    dpy = XOpenDisplay(nullptr);
    if (!dpy) throw std::runtime_error("Cannot open X display");

    if (!XkbQueryExtension(dpy, &xkbEventCode, &xkbErrorCode, nullptr, nullptr, nullptr))
        throw std::runtime_error("Xkb extension not available");

    desc = XkbGetKeyboard(dpy, XkbAllComponentsMask, XkbUseCoreKbd);
    XkbGetState(dpy, XkbUseCoreKbd, &state);

    // Use XkbRF_GetNamesProp for reliable short layout codes
    XkbRF_VarDefsRec vd;
    char *tmp = nullptr;
    if (XkbRF_GetNamesProp(dpy, &tmp, &vd)) {
        groupLayouts.resize(XkbNumKbdGroups, "unknown");
        if (vd.layout) {
            std::string layout_str = vd.layout;
            std::vector<std::string> layouts;
            std::string token;
            for (char c : layout_str) {
                if (c == ',') {
                    if (!token.empty()) {
                        layouts.push_back(token);
                        token.clear();
                    }
                } else {
                    token += c;
                }
            }
            if (!token.empty()) layouts.push_back(token);

            for (size_t i = 0; i < layouts.size() && i < groupLayouts.size(); ++i) {
                groupLayouts[i] = layouts[i];
            }
        }
        if (tmp) XFree(tmp);
    }

    {
        std::lock_guard<std::mutex> lock(layoutMutex);
        currentLayout = layout_from_group(state.group);
    }

    // start listening in this thread (blocks like Brightness)
    listen();
}

Keyboard::~Keyboard() {
    if (desc) XkbFreeKeyboard(desc, 0, True);
    if (dpy) XCloseDisplay(dpy);
}

std::string Keyboard::get_layout() {
    std::lock_guard<std::mutex> lock(layoutMutex);
    return currentLayout;
}

std::string Keyboard::layout_from_group(unsigned group) const {
    if (group < groupLayouts.size()) {
        return groupLayouts[group];
    }
    return "unknown";
}

void Keyboard::listen() {
    XkbSelectEventDetails(dpy, XkbUseCoreKbd, XkbStateNotify,
                          XkbAllStateComponentsMask, XkbGroupStateMask);

    while (true) {
        XEvent ev;
        XNextEvent(dpy, &ev);

        bool changed = false;
        XkbEvent* xkbev = (XkbEvent*)&ev;
        if (xkbev->any.xkb_type == XkbStateNotify && xkbev->state.group != state.group) {
            state.group = xkbev->state.group;
            std::lock_guard<std::mutex> lock(layoutMutex);
            currentLayout = layout_from_group(state.group);
            changed = true;
        }

        // Always refresh lemonbar
        if (changed) updateLemonbar(lemonOutput());
    }
}
