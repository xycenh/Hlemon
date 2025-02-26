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
						/*<< "%{F#777777}|%{F-}"*/
            << Volume::getVolume() << "%{F#777777}|%{F-}"
						<< Mic::getMicVolume() << "%{F#777777}|%{F-}"
            << Brightness::getBrightness() << "%{F#777777}|%{F-}"
            << Battery::getBattery() << "%{F#777777}|%{F-}"
      	    << getClock() << ""
            << std::endl;
}
