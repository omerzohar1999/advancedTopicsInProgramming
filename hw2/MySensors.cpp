#include "MySensors.h"
#include <iostream>
#include <memory>

MyBatteryMeter::MyBatteryMeter(MyBatteryMeter &m) { this->sim = m.sim; }

void MyBatteryMeter::setHouse(const Simulator &house) { this->sim = &house; };
MyWallsSensor::MyWallsSensor(MyWallsSensor &s) { this->sim = s.sim; }
void MyWallsSensor::setHouse(const Simulator &house) { this->sim = &house; };
MyDirtSensor::MyDirtSensor(MyDirtSensor &s) { this->sim = s.sim; }
void MyDirtSensor::setHouse(const Simulator &house) { this->sim = &house; };
std::size_t MyBatteryMeter::getBatteryState() const {
  return sim->getBatteryLeft();
};
bool MyWallsSensor::isWall(Direction dir) const {
  return sim->isThereWall(dir);
};
int MyDirtSensor::dirtLevel() const { return sim->howMuchDirtHere(); };