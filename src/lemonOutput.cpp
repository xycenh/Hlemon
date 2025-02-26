#include "lemonOutput.h"
#include "battery.h"
#include "brightness.h"
#include "volume.h"
#include "mic.h"
#include "workspaces.h"
#include "clock.h"
#include <iostream>

void lemonOutput() {
  std::cout << "%{l}" << " "
            << Workspaces::getWorkspaces()
            << "%{r}"
						<< " | "
            << Volume::getVolume() << " | "
						<< Mic::getMicVolume() << " | "
            << Brightness::getBrightness() << " | "
            << Battery::getBattery() << " | "
      	    << getClock() << " "
            << std::endl;
}

void lemonOutputWrapper(int value) {
    lemonOutput(); // Call the original function
}

