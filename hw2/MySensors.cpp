#include "MySensors.h"
#include <iostream>
#include <memory>

MyBatteryMeter::MyBatteryMeter(std::shared_ptr<const MySimulator> house) {
  this->sim = house;
};
MyWallsSensor::MyWallsSensor(std::shared_ptr<const MySimulator> house) {
  this->sim = house;
};
MyDirtSensor::MyDirtSensor(std::shared_ptr<const MySimulator> house) {
  this->sim = house;
};
std::size_t MyBatteryMeter::getBatteryState() const {
  return sim->getBatteryLeft();
};
bool MyWallsSensor::isWall(Direction dir) const {
  return sim->isThereWall(dir);
};
int MyDirtSensor::dirtLevel() const { return sim->howMuchDirtHere(); };