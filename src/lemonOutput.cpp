#include "lemonOutput.h"
#include "battery.h"
#include "brightness.h"
#include <iostream>
#include <string>

void lemonOutput() {
  std::cout << "%{r}" << Battery::getBattery() << " "
            << Brightness::getBrightness() << std::endl;
}
