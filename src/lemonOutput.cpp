#include "lemonOutput.h"
#include "battery.h"
#include "brightness.h"
#include <iostream>

void lemonOutput() {
  std::cout << "%{r}"
            << Brightness::getBrightness() << " | "
            << Battery::getBattery()
            << std::endl;
}
