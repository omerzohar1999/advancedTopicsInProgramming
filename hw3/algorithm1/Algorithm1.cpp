#include "Algorithm1.h"
#include "AlgorithmGraph.h"
#include "AlgorithmRegistration.h"
#include "enums.h"
#include "enums_utils.h"
#include <iostream>
#include <memory>

size_t Algorithm1::getBatteryStepsLeft() const {
  size_t battery_size = batteryMeter->getBatteryState();
  return battery_size < cur_steps_left ? battery_size : cur_steps_left;
}

void Algorithm1::setMaxSteps(size_t maxSteps) {
  this->cur_steps_left = maxSteps;
};

void Algorithm1::setWallsSensor(const WallsSensor &wallSensor) {
  this->wallSensor = &wallSensor;
};

void Algorithm1::setDirtSensor(const DirtSensor &dirtSensor) {
  this->dirtSensor = &dirtSensor;
};

void Algorithm1::setBatteryMeter(const BatteryMeter &batteryMeter) {
  this->batteryMeter = &batteryMeter;
  this->battery_max_size = batteryMeter.getBatteryState();
};

bool Algorithm1::isFullyCharged() {
  return getBatteryStepsLeft() == battery_max_size;
}

bool Algorithm1::hasEnoughChargeDirty(int dockingDist, int dirtyDist) {
  return dockingDist + dirtyDist + 1 <= (int)getBatteryStepsLeft();
}

bool Algorithm1::hasEnoughChargeUnvisited(int dockingDist, int unvisitedDist) {
  return dockingDist + unvisitedDist <= (int)getBatteryStepsLeft();
}

bool Algorithm1::hasEnoughChargeToClean(int dockingDist) {
  return (int)getBatteryStepsLeft() > dockingDist;
}

bool isDirtyDistanceZero(int dirtyDist) { return dirtyDist == 0; }

Step Algorithm1::nextStep() {
  Step step = getNextStep();

  houseGraph.updateCurrent(step);
  if (step != Step::Finish)
    cur_steps_left--;
  return step;
}

typedef std::pair<std::pair<int, Direction>, int> DistAndDir;

Step Algorithm1::getNextStep() {
  // Update house graph with current sensor readings and distances from docking
  houseGraph.visit(
      dirtSensor->dirtLevel(), wallSensor->isWall(Direction::North),
      wallSensor->isWall(Direction::East), wallSensor->isWall(Direction::South),
      wallSensor->isWall(Direction::West));

  if (!houseGraph.finishedScanning())
    houseGraph.updateDistancesFromDocking();

  // if in docking, check for conditions that will allow to avoid bfs
  // calculations

  if (houseGraph.isInDocking()) {
    if (houseGraph.finishedJob())
      return Step::Finish;
    if (!isFullyCharged())
      return Step::Stay;
  }

  // calculate distances from current location to docking, unvisited and dirty.
  // if finished scanning/cleaning avoid unnecessary bfs calculations
  std::pair<int, Direction> dockingDistAndDir = houseGraph.dockingDistAndDir();

  // if house was not fully scanned yet, go to the nearest unvisited cell
  if (!houseGraph.finishedScanning()) {
    DistAndDir unvisitedDistAndDir = houseGraph.unvisitedBfs();

    int unvisitedDist = unvisitedDistAndDir.first.first;
    Direction unvisitedDir = unvisitedDistAndDir.first.second;
    int unvisitedToDockingDist = unvisitedDistAndDir.second;

    // if it has enough charge to go to the nearest unvisited cell, go there.
    // else go to the docking station
    if (hasEnoughChargeUnvisited(unvisitedToDockingDist, unvisitedDist))
      return dirToStep(unvisitedDir);
    return dirToStep(dockingDistAndDir.second);
  }

  // if house was fully scanned but not cleaned, go to the nearest dirty cell
  if (!houseGraph.finishedCleaning()) {
    DistAndDir dirtyDistAndDir = houseGraph.dirtyBfs();

    // it is possible that only here we discover that the house is fully clean
    // if so, go to docking, and finish when got there
    // this can only happen once
    if (houseGraph.finishedCleaning()) {
      if (houseGraph.isInDocking()) {
        return Step::Finish;
      }
      return dirToStep(dockingDistAndDir.second);
    }

    int dirtyDist = dirtyDistAndDir.first.first;
    Direction dirtyDir = dirtyDistAndDir.first.second;
    int dirtyToDockingDist = dirtyDistAndDir.second;

    if (hasEnoughChargeDirty(dirtyToDockingDist, dirtyDist)) {
      // if the robot is already in a dirty cell, clean it.
      // else go to the closest
      if (dirtyDist == 0)
        return Step::Stay;
      return dirToStep(dirtyDir);
    }
    // if the robot doesn't have enough charge to clean the nearest dirty cell,
    // go to the docking station
    return dirToStep(dockingDistAndDir.second);
  }

  // if the house was fully scanned and cleaned, go to the docking station.
  // if the robot is already in the docking station, finish the job.
  if (houseGraph.isInDocking()) {
    return Step::Finish;
  }
  return dirToStep(dockingDistAndDir.second);
}

REGISTER_ALGORITHM(Algorithm1);