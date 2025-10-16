#include "lemonOutput.h"
#include "battery.h"
#include "brightness.h"
#include "volume.h"
#include "mic.h"
#include "workspaces.h"
#include "clock.h"
#include <iostream>
#include <string>

FILE *bar = nullptr;

void initLemonbar() {
    bar = popen("lemonbar -B \"#1e1e2e\" -g 1600x25+0+0 -f \"Iosevka Fixed:size=12\" -d", "w");
    if (!bar) {
        std::cerr << "Error initializing lemonbar\n";
        exit(1);
    }
}

/*void initLemonbar() {*/
/*    FILE* xrandr = popen("xrandr --query | grep '^eDP-1'", "r");*/
/*    if (!xrandr) {*/
/*        std::cerr << "Error running xrandr\n";*/
/*        exit(1);*/
/*    }*/
/**/
/*    char buffer[128];*/
/*    std::string output;*/
/*    while (fgets(buffer, sizeof(buffer), xrandr) != nullptr) {*/
/*        output += buffer;*/
/*    }*/
/*    pclose(xrandr);*/
/**/
/*    int width = 1600, height = 25, x = 0, y = 0;*/
/*    std::istringstream ss(output);*/
/*    std::string token;*/
/*    if (ss >> token >> token >> token) { // Extract resolution and position*/
/*        sscanf(token.c_str(), "%dx%d+%d+%d", &width, &height, &x, &y);*/
/*    }*/
/**/
/*    std::string cmd = "lemonbar -B \"#000000\" -g " + */
/*                      std::to_string(width) + "x" + std::to_string(height) +*/
/*                      "+" + std::to_string(x) + "+" + std::to_string(y) +*/
/*                      " -f \"JetBrains Mono NL:size=12\"";*/
/**/
/*    bar = popen(cmd.c_str(), "w");*/
/*    if (!bar) {*/
/*        std::cerr << "Error initializing lemonbar\n";*/
/*        exit(1);*/
/*    }*/
/*}*/

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
    // << Workspaces::getWorkspaces() << separator << " "
    // << Workspaces::getWindows()
    // << "%{r}"
    // << Volume::getVolume() << separator
    // << Mic::getMicVolume() << separator
    // << Brightness::getBrightness() << separator
    // << Battery::getBattery() << separator
    // << getClock()
    // << " " << std::endl;
    //
    // return "";
    return
        "%{l} "
        + Workspaces::getWorkspaces()
        + Workspaces::getWindows()
        + "%{r}"
        + Volume::getVolume() + separator
        + Mic::getMicVolume() + separator
        + Brightness::getBrightness() + separator
        + Battery::getBattery() + separator
        + getClock()
        + " ";
}
