#ifndef CONFIG_H
#define CONFIG_H

#include <string>

struct Options {
    std::string terminal;
    std::string background_color;
    std::string foreground_color;
    std::string icon_color;
    std::string separator;
    std::string separator_color;

    struct Layout {
        std::string color;
        std::string background_color;
        std::string icon;
    };

    struct Volume {};
    struct Mic {};
    struct Brightness {};
    struct Battery {};
    struct Clock {};
    struct Workspaces {};
    struct Windows {};
};

#endif //CONFIG_H
