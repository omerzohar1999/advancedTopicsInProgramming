#include "MyAlgorithm.h"
#include "AlgorithmGraph.h"
#include "enums.h"
#include "enums_utils.h"
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
    return distFromDocking >= (int) getBatteryStepsLeft() - 1;
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

void MyAlgorithm::setMaxSteps(std::size_t maxSteps) {
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

Step MyAlgorithm::nextStep() {
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
    Step ret = dirToStep(dockingDistAndDir.second);

    if (houseGraph.isInDocking() && houseGraph.houseWasFullyExplored() &&
        houseGraph.houseWasFullyCleaned())
        return Step::Finish;

    if (isCharging()) {
        ret = Step::Stay;
        goto end;
    }

    if (mustGoCharge(dockingDist)) {
        ret = dirToStep(dockingDir);
        goto end;
    }

    if (!(houseGraph.houseWasFullyExplored())) {
        std::cout << "looking for an unvisited cell" << std::endl;
        if (unvisitedDist != -1) {
            std::cout << "found one!" << std::endl;
            std::cout << "distance is " << unvisitedDist << std::endl;
            std::cout << "direction is "
                      << directionString(unvisitedDir) << std::endl;
            if (unvisitedDist == 0)
                ret = Step::Stay;
            else
                ret = dirToStep(unvisitedDir);

            /*if (isRunningOutOfStepsUnvisited()) {
              if (houseGraph.isInDocking())
                ret = Step::Stay;
              else
                ret = dirToStep(dockingDistAndDir.second);
            }*/
            goto end;
        }
    }

    if (!(houseGraph.houseWasFullyCleaned())) {
        std::cout << "looking for a dirty cell" << std::endl;
        if (dirtyDist != -1) {
            ret = dirToStep(dirtyDir);
            if (isRunningOutOfStepsDirty()) {
                if (houseGraph.isInDocking())
                    ret = Step::Stay;
                else
                    ret = dirToStep(dockingDir);
            } else if (dockingDist == 0)
                ret = Step::Stay;
            goto end;
        }
    }

    if (dockingDist == 0) {
        // from robot POV, house cleaning is finished, and we're in docking.
        ret = Step::Finish;
    }

    end:
    houseGraph.updateCurrent(ret);
    if (ret != Step::Finish)
        cur_steps_left--;
    std::cout << "returned step " << stepString(ret) << std::endl;
    return ret;
}
