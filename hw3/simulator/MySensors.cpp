#include "MySensors.h"
#include <cmath>
#include <iostream>
#include <memory>

void MyBatteryMeter::setHouse(Simulator *sim) { this->sim = sim; };

void MyWallsSensor::setHouse(Simulator *sim) { this->sim = sim; };

void MyDirtSensor::setHouse(Simulator *sim) { this->sim = sim; };

std::size_t MyBatteryMeter::getBatteryState() const {
  return (size_t)std::floor(sim->getBatteryLeft());
};

bool MyWallsSensor::isWall(Direction dir) const {
  return sim->isThereWall(dir);
};

int MyDirtSensor::dirtLevel() const { return sim->howMuchDirtHere(); };