#include "VaccumCleaner.h"
#include "Direction.h"
#include "House.h"
#include "VaccumGraph.h"
#include <iostream>

u_int32_t VaccumCleaner::getBatteryStepsLeft() {
  return sensor->getBatteryStepsLeft();
}
bool VaccumCleaner::isThereWall(Direction dir) {
  bool ret = sensor->isThereWall(dir);
  return ret;
}
int VaccumCleaner::howMuchDirtHere() { return sensor->howMuchDirtHere(); }

VaccumCleaner::VaccumCleaner(u_int32_t battery_max_size, Sensor *sensor) {
  this->battery_max_size = battery_max_size;
  this->sensor = sensor;
  this->houseGraph = new VaccumGraph();
}

Direction VaccumCleaner::getStep() {

  houseGraph->visit(howMuchDirtHere(), isThereWall(NORTH), isThereWall(EAST),
                    isThereWall(SOUTH), isThereWall(WEST));
  if (houseGraph->isInDocking() && getBatteryStepsLeft() < battery_max_size) {
    // in docking and not full battery. stay!
    return STAY;
  }
  if (houseGraph->distanceFromDocking() >= (int)getBatteryStepsLeft() - 2) {
    // battery will run out if we dont go to docking
    Direction ret = houseGraph->dirForDocking();
    houseGraph->updateCurrent(ret);
    return ret;
  }
  if (!(houseGraph->houseWasFullyExplored())) {
    Direction ret = houseGraph->dirForUnvisited();
    this->houseGraph->updateCurrent(ret);
    if (ret != NOT_EXISTS)
      return ret;
  }
  if (!(houseGraph->houseWasFullyCleaned())) {
    Direction ret = houseGraph->dirForDirty();
    if (ret == NOT_EXISTS) {
      Direction ret = houseGraph->dirForDocking();
      this->houseGraph->updateCurrent(ret);
      return ret;
    }
    if (ret == STAY) {
      houseGraph->decreaseDirt();
    }
    houseGraph->updateCurrent(ret);
    return ret;
  }
  Direction ret = houseGraph->dirForDocking();
  this->houseGraph->updateCurrent(ret);
  if (ret == STAY) {
    // we do not need to charge, we discovered all that's reachable and cleaned
    // it, and we're already in the docking station.
    return NOT_EXISTS;
  }
  return ret;
}
