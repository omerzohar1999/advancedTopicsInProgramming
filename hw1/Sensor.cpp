#include "Sensor.h"
#include <iostream>

Sensor::Sensor(const House *house) : house(house) {};
u_int32_t Sensor::getBatteryStepsLeft() const {
  return house->getBatteryLeft();
};
bool Sensor::isThereWall(Direction dir) const {
  return house->isThereWall(dir);
};
int Sensor::howMuchDirtHere() const { return house->howMuchDirtHere(); };