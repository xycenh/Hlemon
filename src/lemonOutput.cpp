#include "lemonOutput.h"
#include "battery.h"
#include "brightness.h"
#include "volume.h"
#include "mic.h"
#include "workspaces.h"
#include "clock.h"
#include "layout.h"
#include <iostream>
#include <string>
#include <xcb/xcb.h>

FILE *bar = nullptr;

int getPrimaryMonitorWidth() {
    xcb_connection_t* conn = xcb_connect(nullptr, nullptr);
    if (xcb_connection_has_error(conn)) {
        std::cerr << "Failed to connect to X server\n";
        return 1920; // fallback
    }

    const xcb_setup_t* setup = xcb_get_setup(conn);
    xcb_screen_iterator_t iter = xcb_setup_roots_iterator(setup);
    xcb_screen_t* screen = iter.data;

    // Get screen resources
    xcb_randr_get_screen_resources_current_cookie_t res_cookie =
        xcb_randr_get_screen_resources_current(conn, screen->root);
    xcb_randr_get_screen_resources_current_reply_t* res_reply =
        xcb_randr_get_screen_resources_current_reply(conn, res_cookie, nullptr);

    if (!res_reply) {
        std::cerr << "Failed to get screen resources\n";
        xcb_disconnect(conn);
        return 1920;
    }

    int width = 1920; // fallback
    xcb_randr_output_t* outputs = xcb_randr_get_screen_resources_current_outputs(res_reply);
    int output_count = xcb_randr_get_screen_resources_current_outputs_length(res_reply);

    for (int i = 0; i < output_count; i++) {
        xcb_randr_get_output_info_cookie_t info_cookie =
            xcb_randr_get_output_info(conn, outputs[i], XCB_CURRENT_TIME);
        xcb_randr_get_output_info_reply_t* info_reply =
            xcb_randr_get_output_info_reply(conn, info_cookie, nullptr);

        if (info_reply && info_reply->crtc != XCB_NONE) {
            xcb_randr_get_crtc_info_cookie_t crtc_cookie =
                xcb_randr_get_crtc_info(conn, info_reply->crtc, XCB_CURRENT_TIME);
            xcb_randr_get_crtc_info_reply_t* crtc_reply =
                xcb_randr_get_crtc_info_reply(conn, crtc_cookie, nullptr);

            if (crtc_reply && crtc_reply->x == 0) {
                width = crtc_reply->width;
                free(crtc_reply);
                free(info_reply);
                break;
            }

            if (crtc_reply)
                free(crtc_reply);
        }

        if (info_reply)
            free(info_reply);
    }

    free(res_reply);
    xcb_disconnect(conn);
    return width;
}

void initLemonbar() {
    int width = getPrimaryMonitorWidth();

    char cmd[256];
    snprintf(cmd, sizeof(cmd),
             "lemonbar -B \"#1e1e2e\" -g %dx25+0+0 -f \"Iosevka Fixed:size=12\" -d -a 100",
             width);

    bar = popen(cmd, "w");
    if (!bar) {
        std::cerr << "Error initializing lemonbar\n";
        exit(1);
    }
}

void updateLemonbar(const std::string& content) {
    if (bar) {
        fprintf(bar, "%s\n", content.c_str());
        fflush(bar);
    }
}

std::string lemonOutput() {
    std::string separator = "%{F#7f849c}|%{F-}";

    // std::cout << 
    // "%{l} "
    // << workspaces::getworkspaces()
    // << workspaces::getwindows()
    // << "%{r}"
    // << keyboard::get_layout() << separator
    // << volume::getvolume() << separator
    // << mic::getmicvolume() << separator
    // << brightness::getbrightness() << separator
    // << battery::getbattery() << separator
    // << getclock()
    // << " " << std::endl;
    // return "";

    return
        "%{l} "
        + Workspaces::getWorkspaces()
        + Workspaces::getWindows()
        + "%{r}"
        + Keyboard::get_layout() + separator
        + Volume::getVolume() + separator
        + Mic::getMicVolume() + separator
        + Brightness::getBrightness() + separator
        + Battery::getBattery() + separator
        + getClock()
        + " ";
}
