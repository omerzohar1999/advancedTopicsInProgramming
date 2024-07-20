#include "MyAlgorithm.h"
#include "AlgorithmGraph.h"
#include "enums.h"
#include "enums_utils.h"
#include <iostream>
#include <memory>

size_t MyAlgorithm::getBatteryStepsLeft() const {
    return batteryMeter->getBatteryState();
}

bool MyAlgorithm::isRunningOutOfStepsUnvisited() {
    int dockingDist = houseGraph.dockingBfs().first;
    int unvisitedDist = houseGraph.unvisitedBfs().first;
    return dockingDist + unvisitedDist >= (int) cur_steps_left;
}

bool MyAlgorithm::isRunningOutOfStepsDirty() {
    int dockingDist = houseGraph.dockingBfs().first;
    int dirtyDist = houseGraph.dirtyBfs().first;

    return dockingDist + dirtyDist >= (int) cur_steps_left;
}

void MyAlgorithm::setMaxSteps(size_t maxSteps) {
    this->cur_steps_left = maxSteps;
};

void MyAlgorithm::setWallsSensor(const WallsSensor &wallSensor) {
    this->wallSensor = &wallSensor;
};

void MyAlgorithm::setDirtSensor(const DirtSensor &dirtSensor) {
    this->dirtSensor = &dirtSensor;
};

void MyAlgorithm::setBatteryMeter(const BatteryMeter &batteryMeter) {
    this->batteryMeter = &batteryMeter;
    this->battery_max_size = batteryMeter.getBatteryState();
};

bool MyAlgorithm::isFullyCharged() {
    return getBatteryStepsLeft() == battery_max_size;
}

bool MyAlgorithm::hasEnoughChargeDirty(int dockingDist, int dirtyDist) {
    return dockingDist + dirtyDist <= (int) getBatteryStepsLeft();
}

bool MyAlgorithm::hasEnoughChargeUnvisited(int dockingDist, int unvisitedDist) {
    return dockingDist + unvisitedDist <= (int) getBatteryStepsLeft();
}

bool MyAlgorithm::hasEnoughChargeToClean(int dockingDist) {
    return (int) getBatteryStepsLeft() > dockingDist;
}

bool isDirtyDistanceZero(int dirtyDist) {
    return dirtyDist == 0;
}


Step MyAlgorithm::nextStep() {
    Step step = getNextStep();

    houseGraph.updateCurrent(step);
    if (step != Step::Finish)
        cur_steps_left--;
    std::cout << "returned step " << stepString(step) << std::endl;
    return step;
}

Step MyAlgorithm::getNextStep() {
    houseGraph.visit(
            dirtSensor->dirtLevel(), wallSensor->isWall(Direction::North),
            wallSensor->isWall(Direction::East), wallSensor->isWall(Direction::South),
            wallSensor->isWall(Direction::West));
    auto dockingDistAndDir = houseGraph.dockingBfs();
    auto unvisitedDistAndDir = houseGraph.unvisitedBfs();
    auto dirtyDistAndDir = houseGraph.dirtyBfs();
    int dockingDist, unvisitedDist, dirtyDist;
    Direction dockingDir, unvisitedDir, dirtyDir;
    dockingDist = dockingDistAndDir.first;
    dockingDir = dockingDistAndDir.second;
    unvisitedDist = unvisitedDistAndDir.first;
    unvisitedDir = unvisitedDistAndDir.second;
    dirtyDist = dirtyDistAndDir.first;
    dirtyDir = dirtyDistAndDir.second;

    if (houseGraph.isInDocking()) {
        if (houseGraph.houseWasFullyExplored()) {
            if (houseGraph.houseWasFullyCleaned()) {
                return Step::Finish;
            } else {
                if (isRunningOutOfStepsDirty()) {
                    return Step::Finish;
                } else {
                    if (isFullyCharged()) {
                        if (hasEnoughChargeDirty(dockingDist, dirtyDist)) {
                            return dirToStep(dirtyDir);
                        } else {
                            return Step::Finish;
                        }
                    } else {
                        return Step::Stay;
                    }
                }
            }
        } else {
            if (isRunningOutOfStepsUnvisited()) {
                return Step::Finish;
            } else {
                if (isFullyCharged()) {
                    if (hasEnoughChargeUnvisited(dockingDist, unvisitedDist)) {
                        return dirToStep(unvisitedDir);
                    } else {
                        return Step::Finish;
                    }
                } else {
                    return Step::Stay;
                }
            }
        }
    } else {
        if (houseGraph.houseWasFullyExplored()) {
            if (houseGraph.houseWasFullyCleaned()) {
                return dirToStep(dockingDir);
            } else {
                if (isRunningOutOfStepsDirty()) {
                    return dirToStep(dockingDir);
                } else {
                    if (hasEnoughChargeDirty(dockingDist, dirtyDist)) {
                        if (isDirtyDistanceZero(dirtyDist)) {
                            if (hasEnoughChargeToClean(dockingDist)) {
                                return Step::Stay;
                            } else {
                                return dirToStep(dockingDir);
                            }
                        } else {
                            return dirToStep(dirtyDir);
                        }
                    } else {
                        return dirToStep(dockingDir);
                    }
                }
            }
        } else {
            if (isRunningOutOfStepsUnvisited()) {
                return dirToStep(dockingDir);
            } else {
                if (hasEnoughChargeUnvisited(dockingDist, unvisitedDist)) {
                    return dirToStep(unvisitedDir);
                } else {
                    return dirToStep(dockingDir);
                }
            }
        }
    }
}
