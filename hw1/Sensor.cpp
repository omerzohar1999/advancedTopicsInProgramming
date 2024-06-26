#include "Sensor.h"
#include <iostream>

Sensor::Sensor(House *house) : house(house) {};
u_int32_t Sensor::getBatteryStepsLeft() const {
  return house->getBatteryLeft();
};
bool Sensor::isThereWall(direction dir) const {
  return house->isThereWall(dir);
};
int Sensor::howMuchDirtHere() const { return house->howMuchDirtHere(); };