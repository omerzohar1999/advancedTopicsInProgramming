#include "MySensors.h"
#include <iostream>
#include <memory>

MyBatteryMeter::MyBatteryMeter(MyBatteryMeter &m) { this->sim = m.sim; }

void MyBatteryMeter::setHouse(std::weak_ptr<Simulator> sim) {
    this->sim = sim;
};

MyWallsSensor::MyWallsSensor(MyWallsSensor &s) { this->sim = s.sim; }

void MyWallsSensor::setHouse(std::weak_ptr<Simulator> sim) { this->sim = sim; };

MyDirtSensor::MyDirtSensor(MyDirtSensor &s) { this->sim = s.sim; }

void MyDirtSensor::setHouse(std::weak_ptr<Simulator> sim) { this->sim = sim; };

std::size_t MyBatteryMeter::getBatteryState() const {
    return sim.lock()->getBatteryLeft();
};

bool MyWallsSensor::isWall(Direction dir) const {
    return sim.lock()->isThereWall(dir);
};

int MyDirtSensor::dirtLevel() const { return sim.lock()->howMuchDirtHere(); };