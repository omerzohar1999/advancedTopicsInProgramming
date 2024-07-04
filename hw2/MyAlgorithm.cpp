#include "MyAlgorithm.h"
#include "AlgorithmGraph.h"
#include "Direction.h"
#include "House.h"
#include <iostream>
#include <memory>

size_t MyAlgorithm::getBatteryStepsLeft() const {
  return batteryMeter->getBatteryState();
}

MyAlgorithm::MyAlgorithm() {};

bool MyAlgorithm::isCharging() const {
  return houseGraph.isInDocking() && getBatteryStepsLeft() < battery_max_size;
}

bool MyAlgorithm::mustGoCharge(int distFromDocking) const {
  return distFromDocking >= (int)getBatteryStepsLeft() - 1;
}

bool MyAlgorithm::isRunningOutOfStepsUnvisited() {
  return houseGraph.dockingDir().first + houseGraph.unvisitedDir().first >=
         (int)cur_steps_left;
}

bool MyAlgorithm::isRunningOutOfStepsDirty() {
  return houseGraph.dockingDir().first + houseGraph.dirtyDir().first >=
         (int)cur_steps_left;
}

void MyAlgorithm::setMaxSteps(std::size_t maxSteps) {};
void MyAlgorithm::setWallsSensor(const WallsSensor &wallSensor) {
  this->wallSensor = std::unique_ptr<const WallsSensor>(&wallSensor);
};
void MyAlgorithm::setDirtSensor(const DirtSensor &dirtSensor) {
  this->dirtSensor = std::unique_ptr<const DirtSensor>(&dirtSensor);
};
void MyAlgorithm::setBatteryMeter(const BatteryMeter &batteryMeter) {
  this->batteryMeter = std::unique_ptr<const BatteryMeter>(&batteryMeter);
};

Step MyAlgorithm::nextStep() {
  auto dockingDistAndDir = houseGraph.dockingDir();
  Step ret = dirToStep(dockingDistAndDir.second);

  houseGraph.visit(
      dirtSensor->dirtLevel(), wallSensor->isWall(Direction::North),
      wallSensor->isWall(Direction::East), wallSensor->isWall(Direction::South),
      wallSensor->isWall(Direction::West));

  if (isCharging()) {
    ret = Step::Stay;
    goto end;
  }

  if (mustGoCharge(dockingDistAndDir.first)) {
    ret = dirToStep(dockingDistAndDir.second);
    goto end;
  }

  if (!(houseGraph.houseWasFullyExplored())) {
    auto unvisitedDistAndDir = houseGraph.unvisitedDir();
    if (unvisitedDistAndDir.first != -1) {
      ret = dirToStep(unvisitedDistAndDir.second);
      if (isRunningOutOfStepsUnvisited()) {
        if (houseGraph.isInDocking())
          ret = Step::Stay;
        else
          ret = dirToStep(dockingDistAndDir.second);
      }
      goto end;
    }
  }

  if (!(houseGraph.houseWasFullyCleaned())) {
    auto dirtyDistAndDir = houseGraph.dirtyDir();
    if (dirtyDistAndDir.first != -1) {
      ret = dirToStep(dirtyDistAndDir.second);
      if (isRunningOutOfStepsDirty()) {
        if (houseGraph.isInDocking())
          ret = Step::Stay;
        else
          ret = dirToStep(dockingDistAndDir.second);
      }
      goto end;
    }
  }

  if (dockingDistAndDir.first == 0) {
    // from robot POV, house cleaning is finished, and we're in docking.
    ret = Step::Finish;
  }

end:
  houseGraph.updateCurrent(ret);
  if (ret != Step::Finish)
    cur_steps_left--;
  return ret;
}
