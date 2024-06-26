#include "VaccumCleaner.h"
#include "Direction.h"
#include "House.h"
#include "VaccumGraph.h"
#include <iostream>

u_int32_t VaccumCleaner::getBatteryStepsLeft() {
  std::cout << "a" << std::endl;
  return sensor->getBatteryStepsLeft();
}
bool VaccumCleaner::isThereWall(direction dir) {
  std::cout << directionString(dir) << std::endl;
  bool ret = sensor->isThereWall(dir);
  return ret;
}
int VaccumCleaner::howMuchDirtHere() {
  std::cout << "b" << std::endl;
  return sensor->howMuchDirtHere();
}

VaccumCleaner::VaccumCleaner(u_int32_t battery_max_size, Sensor *sensor) {
  this->battery_max_size = battery_max_size;
  this->sensor = sensor;
  this->houseGraph = new VaccumGraph();
}

direction VaccumCleaner::getStep() {

  houseGraph->visit(howMuchDirtHere(), isThereWall(NORTH), isThereWall(EAST),
                    isThereWall(SOUTH), isThereWall(WEST));
  if (houseGraph->isInDocking() && getBatteryStepsLeft() < battery_max_size) {
    // in docking and not full battery. stay!
    return STAY;
  }
  if (houseGraph->distanceFromDocking() >= (int)getBatteryStepsLeft() - 1) {
    // battery will run out if we dont go to docking
    direction ret = houseGraph->dirForDocking();
    houseGraph->updateCurrent(ret);
    return ret;
  }
  if (!(houseGraph->houseWasFullyExplored())) {
    std::cout << "ROBOPOV: going to explore house" << std::endl;
    direction ret = houseGraph->dirForUnvisited();
    std::cout << "ROBOPOV: made decision " << directionString(ret) << std::endl;
    this->houseGraph->updateCurrent(ret);
    std::cout << "ROBOPOV: updated internal graph" << std::endl;
    if (ret != NOT_EXISTS)
      return ret;
  }
  if (!(houseGraph->houseWasFullyCleaned())) {
    direction ret = houseGraph->dirForDirty();
    if (ret == NOT_EXISTS) {
      direction ret = houseGraph->dirForDocking();
      this->houseGraph->updateCurrent(ret);
      return ret;
    }
    if (ret == STAY) {
      houseGraph->decreaseDirt();
    }
    houseGraph->updateCurrent(ret);
    return ret;
  }
  direction ret = houseGraph->dirForDocking();
  this->houseGraph->updateCurrent(ret);
  if (ret == STAY) {
    // we do not need to charge, we discovered all that's reachable and cleaned
    // it, and we're already in the docking station.
    return NOT_EXISTS;
  }
  return ret;
}
