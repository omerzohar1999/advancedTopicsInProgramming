#include "VaccumCleaner.h"
#include "Direction.h"
#include "House.h"
#include "VaccumGraph.h"
#include <iostream>

u_int32_t VaccumCleaner::getBatteryStepsLeft() const {
  return sensor->getBatteryStepsLeft();
}
bool VaccumCleaner::isThereWall(Direction dir) {
  bool ret = sensor->isThereWall(dir);
  return ret;
}
int VaccumCleaner::howMuchDirtHere() const { return sensor->howMuchDirtHere(); }

VaccumCleaner::VaccumCleaner(u_int32_t battery_max_size, Sensor *sensor) {
  this->battery_max_size = battery_max_size;
  this->sensor = sensor;
  this->houseGraph = new VaccumGraph();
}

bool VaccumCleaner::isCharging() const {
  return houseGraph->isInDocking() && getBatteryStepsLeft() < battery_max_size;
}

bool VaccumCleaner::mustGoCharge() const {
  return houseGraph->distanceFromDocking() >= (int)getBatteryStepsLeft() - 1;
}

Direction VaccumCleaner::getStep() {
  Direction ret;

  houseGraph->visit(sensor);

  if (isCharging()) {
    return STAY;
  }

  if (mustGoCharge()) {
    ret = houseGraph->dirForDocking();
    houseGraph->updateCurrent(ret);
    return ret;
  }

  if (!(houseGraph->houseWasFullyExplored())) {
    ret = houseGraph->dirForUnvisited();
    if (ret != NOT_EXISTS) {
      houseGraph->updateCurrent(ret);
      return ret;
    }
  }

  if (!(houseGraph->houseWasFullyCleaned())) {
    ret = houseGraph->dirForDirty();
    if (ret != NOT_EXISTS) {
      houseGraph->updateCurrent(ret);
      return ret;
    }
  }

  ret = houseGraph->dirForDocking();
  if (ret == STAY) {
    // from robot POV, house cleaning finished;
    // no more tiles to discover or clean, and we're in docking already.
    // send "END signal"
    ret = NOT_EXISTS;
  }

  houseGraph->updateCurrent(ret);
  return ret;
}
