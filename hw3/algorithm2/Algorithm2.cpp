#include "Algorithm2.h"
#include "AlgorithmRegistration.h"
#include "algo_common/AlgorithmGraph.h"
#include "enums.h"
#include "enums_utils.h"
#include <iostream>
#include <memory>

size_t Algorithm2::getBatteryStepsLeft() const {
  size_t battery_size = batteryMeter->getBatteryState();
  return battery_size < cur_steps_left ? battery_size : cur_steps_left;
}

void Algorithm2::setMaxSteps(size_t maxSteps) {
  this->cur_steps_left = maxSteps;
};

void Algorithm2::setWallsSensor(const WallsSensor &wallSensor) {
  this->wallSensor = &wallSensor;
};

void Algorithm2::setDirtSensor(const DirtSensor &dirtSensor) {
  this->dirtSensor = &dirtSensor;
};

void Algorithm2::setBatteryMeter(const BatteryMeter &batteryMeter) {
  this->batteryMeter = &batteryMeter;
  this->battery_max_size = batteryMeter.getBatteryState();
};

bool Algorithm2::isFullyCharged() {
  return getBatteryStepsLeft() == battery_max_size;
}

bool Algorithm2::hasEnoughChargeDirty(int dockingDist, int dirtyDist) {
  return dockingDist + dirtyDist <= (int)getBatteryStepsLeft();
}

bool Algorithm2::hasEnoughChargeUnvisited(int dockingDist, int unvisitedDist) {
  return dockingDist + unvisitedDist <= (int)getBatteryStepsLeft();
}

bool Algorithm2::hasEnoughChargeToClean(int dockingDist) {
  return (int)getBatteryStepsLeft() > dockingDist;
}

bool isDirtyDistanceZero(int dirtyDist) { return dirtyDist == 0; }

Step Algorithm2::nextStep() {
  Step step = getNextStep();

  houseGraph.updateCurrent(step);
  if (step != Step::Finish)
    cur_steps_left--;
  return step;
}

typedef std::pair<std::pair<int, Direction>, int> DistAndDir;

Step Algorithm2::getNextStep() {
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

  DistAndDir unvisitedDistAndDir;
  if (!houseGraph.finishedScanning())
    unvisitedDistAndDir = houseGraph.unvisitedBfs();
  else {
    unvisitedDistAndDir.first.first = -1;
  }

  DistAndDir dirtyDistAndDir;
  if (!houseGraph.finishedCleaning())
    dirtyDistAndDir = houseGraph.dirtyBfs();
  else {
    dirtyDistAndDir.first.first = -1;
  }

  int dockingDist = dockingDistAndDir.first;
  Direction dockingDir = dockingDistAndDir.second;

  int unvisitedDist = unvisitedDistAndDir.first.first;
  Direction unvisitedDir = unvisitedDistAndDir.first.second;
  int unvisitedToDockingDist = unvisitedDistAndDir.second;

  int dirtyDist = dirtyDistAndDir.first.first;
  Direction dirtyDir = dirtyDistAndDir.first.second;
  int dirtyToDockingDist = dirtyDistAndDir.second;

  bool dirtyIsFeasible =
      dirtyDist >= 0 && hasEnoughChargeDirty(dirtyToDockingDist, dirtyDist);
  bool unvisitedIsFeasible =
      unvisitedDist >= 0 &&
      hasEnoughChargeUnvisited(unvisitedToDockingDist, unvisitedDist);

  if (houseGraph.isInDocking()) {
    if (!unvisitedIsFeasible && !dirtyIsFeasible)
      return Step::Finish;
  }

  if (dockingDist <= (batteryMeter->getBatteryState()) + 1 ||
      dockingDist <= cur_steps_left + 1)
    return dirToStep(dockingDir);

  if (unvisitedDist == -1 && dirtyDist == -1) // job finished
    return dirToStep(dockingDir);

  else if (unvisitedDist == -1) // finished scanning, but dirty cells left
    return dirToStep(dirtyDir);

  else if (dirtyDist == -1) // no known dirty cells, but unknown cells left
    return dirToStep(unvisitedDir);

  else // both known dirty cells and unvisited cells exist, pick closest
    return dirToStep(dirtyDist < unvisitedDist ? dirtyDir : unvisitedDir);
}

REGISTER_ALGORITHM(Algorithm2);