#include "algo_common/AlgorithmRegistration.h"
#include "algo_common/BaseAlgorithm.h"
#include "common/enums.h"
#include "common/enums_utils.h"
#include <iostream>
#include <memory>

class Algorithm2 : public BaseAlgorithm {

public:
  ~Algorithm2() override {}
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

    Direction dockingDir = dockingDistAndDir.second;

    int unvisitedDist = unvisitedDistAndDir.first.first;
    Direction unvisitedDir = unvisitedDistAndDir.first.second;
    int unvisitedToDockingDist = unvisitedDistAndDir.second;

    int dirtyDist = dirtyDistAndDir.first.first;
    Direction dirtyDir = dirtyDistAndDir.first.second;
    int dirtyToDockingDist = dirtyDistAndDir.second;

    std::cout << "dirtyDist: " << dirtyDist
              << ", dirtyDir: " << directionString(dirtyDir)
              << ", dirtyToDockingDist: " << dirtyToDockingDist << std::endl;

    bool dirtyIsFeasible =
        dirtyDist >= 0 && hasEnoughChargeDirty(dirtyToDockingDist, dirtyDist);
    bool unvisitedIsFeasible =
        unvisitedDist >= 0 &&
        hasEnoughChargeUnvisited(unvisitedToDockingDist, unvisitedDist);

    if (houseGraph.isInDocking()) {
      if (!unvisitedIsFeasible && !dirtyIsFeasible)
        return Step::Finish;
    }

    if (!unvisitedIsFeasible && !dirtyIsFeasible) // job finished
      return dirToStep(dockingDir);

    else if (!unvisitedIsFeasible) // finished scanning, but dirty cells left
    {
      if (dirtyDist == 0)
        return Step::Stay;
      return dirToStep(dirtyDir);
    } else if (!dirtyIsFeasible) // no known dirty cells, but unknown cells left
      return dirToStep(unvisitedDir);

    // both known dirty cells and unvisited cells exist, pick closest
    if (dirtyDist == 0)
      return Step::Stay;

    return dirToStep(dirtyDist < unvisitedDist ? dirtyDir : unvisitedDir);
  }
  Step nextStep() {
    Step step = getNextStep();

    houseGraph.updateCurrent(step);
    if (step != Step::Finish)
      cur_steps_left--;
    return step;
  }
};

extern "C" {
REGISTER_ALGORITHM(Algorithm2);
}