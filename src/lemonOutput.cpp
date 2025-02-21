#include "lemonOutput.h"
#include "battery.h"
#include "brightness.h"
#include "workspaces.h"
#include <iostream>

void lemonOutput() {
  std::cout << "%{l}" << " "
            << Workspaces::getWorkspaces()
            << "%{r}"
            << Brightness::getBrightness() << " | "
            << Battery::getBattery()
            << std::endl;
}
