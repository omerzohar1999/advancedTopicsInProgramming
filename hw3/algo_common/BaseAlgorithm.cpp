#include "algo_common/BaseAlgorithm.h"
#include "algo_common/AlgorithmGraph.h"
#include "algo_common/AlgorithmRegistration.h"
#include "common/enums.h"
#include "common/enums_utils.h"
#include <iostream>
#include <memory>

size_t BaseAlgorithm::getBatteryStepsLeft() const {
  return std::min(batteryMeter->getBatteryState(), cur_steps_left);
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
  this->batteryMeter = &batteryMeter;
  this->battery_max_size = batteryMeter.getBatteryState();
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
