#include "VaccumCleaner.h"
#include "Direction.h"
#include "House.h"
#include "VaccumGraph.h"
#include <iostream>

u_int32_t VaccumCleaner::getBatteryStepsLeft() const {
  return sensor->getBatteryStepsLeft();
}

VaccumCleaner::VaccumCleaner(const u_int32_t battery_max_size,
                             const Sensor *sensor,
                             u_int32_t max_steps)
    : battery_max_size(battery_max_size),
      sensor(sensor),
      houseGraph(new VaccumGraph()),
      cur_steps_left(max_steps){}

bool VaccumCleaner::isCharging() const {
  return houseGraph->isInDocking() && getBatteryStepsLeft() < battery_max_size;
}

bool VaccumCleaner::mustGoCharge() const {
  return houseGraph->distanceFromDocking() >= (int)getBatteryStepsLeft() - 1;
}

bool VaccumCleaner::isRunningOutOfStepsUnvisited() const {
    return houseGraph->distanceFromDocking()
           + houseGraph->distanceFromUnvisited()
           >= (int)cur_steps_left;
}

bool VaccumCleaner::isRunningOutOfStepsDirty() const {
    return houseGraph->distanceFromDocking()
           + houseGraph->distanceFromDirty()
           >= (int)cur_steps_left;
}

Direction VaccumCleaner::getStep() {
  Direction ret;

  houseGraph->visit(sensor);

  if (isCharging()) {
    ret = STAY;
    goto end;
  }

  if (mustGoCharge()) {
    ret = houseGraph->dirForDocking();
    goto end;
  }

  if (!(houseGraph->houseWasFullyExplored())) {
    ret = houseGraph->dirForUnvisited();
    if (ret != NOT_EXISTS) {
        if (isRunningOutOfStepsUnvisited()) {
            if (houseGraph->isInDocking())
                ret = NOT_EXISTS;
            else
                ret = houseGraph->dirForDocking();
        }
        goto end;
    }
  }

  if (!(houseGraph->houseWasFullyCleaned())) {
    ret = houseGraph->dirForDirty();
    if (ret != NOT_EXISTS) {
        if (isRunningOutOfStepsDirty()) {
            if (houseGraph->isInDocking())
                ret = NOT_EXISTS;
            else
                ret = houseGraph->dirForDocking();
        }
        goto end;
    }
  }

  ret = houseGraph->dirForDocking();
  if (ret == STAY) {
    // from robot POV, house cleaning is finished.
    ret = NOT_EXISTS;
  }

end:
  houseGraph->updateCurrent(ret);
  if (ret != NOT_EXISTS)
      cur_steps_left--;
  return ret;
}
