#include "MySensors.h"
#include <memory>
#include <cmath>

void MyBatteryMeter::setHouse(std::weak_ptr<Simulator> sim) {
    this->sim = sim;
};

void MyWallsSensor::setHouse(std::weak_ptr<Simulator> sim) {
    this->sim = sim;
};

void MyDirtSensor::setHouse(std::weak_ptr<Simulator> sim) {
    this->sim = sim;
};

std::size_t MyBatteryMeter::getBatteryState() const {
    return (size_t) std::floor(sim.lock()->getBatteryLeft());
};

bool MyWallsSensor::isWall(Direction dir) const {
    return sim.lock()->isThereWall(dir);
};

int MyDirtSensor::dirtLevel() const {
    return sim.lock()->howMuchDirtHere();
};