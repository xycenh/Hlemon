#include "lemonOutput.h"
#include "battery.h"
#include <iostream>
#include <string>

std::string lemonDataGather() { return Battery::getBattery(); }

void lemonOutput() { std::cout << "%{r}" << lemonDataGather() << std::endl; }
