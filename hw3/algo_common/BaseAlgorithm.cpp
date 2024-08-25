#include "algo_common/BaseAlgorithm.h"
#include "algo_common/AlgorithmGraph.h"
#include "algo_common/AlgorithmRegistration.h"
#include "common/enums.h"
#include "common/enums_utils.h"
#include <iostream>
#include <memory>

size_t BaseAlgorithm::getBatteryStepsLeft() const {
  size_t battery_size = batteryMeter->getBatteryState();
  return battery_size < cur_steps_left ? battery_size : cur_steps_left;
}

void BaseAlgorithm::setMaxSteps(size_t maxSteps) {
  this->cur_steps_left = maxSteps;
};

void BaseAlgorithm::setWallsSensor(const WallsSensor &wallSensor) {
  this->wallSensor = &wallSensor;
};

void BaseAlgorithm::setDirtSensor(const DirtSensor &dirtSensor) {
  this->dirtSensor = &dirtSensor;
};

void BaseAlgorithm::setBatteryMeter(const BatteryMeter &batteryMeter) {
  std::cout << "Setting battery meter" << std::endl;
  this->batteryMeter = &batteryMeter;
  std::cout << "Set battery meter" << std::endl;
  std::cout << "Setting battery max size" << std::endl;
  this->battery_max_size = batteryMeter.getBatteryState();
  std::cout << "Set battery max size" << std::endl;
};

bool BaseAlgorithm::isFullyCharged() {
  return getBatteryStepsLeft() == battery_max_size;
}

bool BaseAlgorithm::hasEnoughChargeDirty(int dockingDist, int dirtyDist) {
  return dockingDist + dirtyDist + 1 <= (int)getBatteryStepsLeft();
}

bool BaseAlgorithm::hasEnoughChargeUnvisited(int dockingDist,
                                             int unvisitedDist) {
  return dockingDist + unvisitedDist <= (int)getBatteryStepsLeft();
}

bool BaseAlgorithm::hasEnoughChargeToClean(int dockingDist) {
  return (int)getBatteryStepsLeft() > dockingDist;
}

bool isDirtyDistanceZero(int dirtyDist) { return dirtyDist == 0; }
