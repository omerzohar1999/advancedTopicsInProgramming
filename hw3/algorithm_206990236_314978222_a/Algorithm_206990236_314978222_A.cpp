#include "algo_common/AlgorithmRegistration.h"
#include "algo_common/BaseAlgorithm.h"
#include "common/enums.h"
#include "common/enums_utils.h"
#include <iostream>

class Algorithm_206990236_314978222_A : public BaseAlgorithm {

public:
  ~Algorithm_206990236_314978222_A() override {}
  Step getNextStep() {
    // Update house graph with current sensor readings and distances from
    // docking
    houseGraph.visit(dirtSensor->dirtLevel(),
                     wallSensor->isWall(Direction::North),
                     wallSensor->isWall(Direction::East),
                     wallSensor->isWall(Direction::South),
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

    // calculate distances from current location to docking, unvisited and
    // dirty. if finished scanning/cleaning avoid unnecessary bfs calculations
    std::pair<int, Direction> dockingDistAndDir =
        houseGraph.dockingDistAndDir();

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

      if (houseGraph.isInDocking()) {
        return Step::Finish;
      }
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
      // if the robot doesn't have enough charge to clean the nearest dirty
      // cell, go to the docking station
      // if the robot is already in the docking station, finish the job.
      if (houseGraph.isInDocking()) {
        return Step::Finish;
      }
      return dirToStep(dockingDistAndDir.second);
    }

    // if the house was fully scanned and cleaned, go to the docking station.
    // if the robot is already in the docking station, finish the job.
    if (houseGraph.isInDocking()) {
      return Step::Finish;
    }
    return dirToStep(dockingDistAndDir.second);
  }

  Step nextStep() {
    Step step = getNextStep();

    houseGraph.updateCurrent(step);
    if (step != Step::Finish)
      cur_steps_left--;
    return step;
  }
};

REGISTER_ALGORITHM(Algorithm_206990236_314978222_A);