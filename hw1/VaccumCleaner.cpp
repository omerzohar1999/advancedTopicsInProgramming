#include "VaccumCleaner.h"
#include "Direction.h"
#include "House.h"
#include "VaccumGraph.h"
#include <iostream>

u_int32_t VaccumCleaner::getBatteryStepsLeft() const {
  return sensor->getBatteryStepsLeft();
}

VaccumCleaner::VaccumCleaner(const u_int32_t battery_max_size,
                             const Sensor *sensor)
    : battery_max_size(battery_max_size), sensor(sensor),
      houseGraph(new VaccumGraph()) {}

bool VaccumCleaner::isCharging() const {
  return houseGraph->isInDocking() && getBatteryStepsLeft() < battery_max_size;
}

bool VaccumCleaner::mustGoCharge() const {
  return houseGraph->distanceFromDocking() >= (int)getBatteryStepsLeft() - 1;
}

Direction VaccumCleaner::getStep() {
  Direction ret;

  houseGraph->visit(sensor);

  if (isCharging()) {
    return STAY;
  }

  if (mustGoCharge()) {
    ret = houseGraph->dirForDocking();
    goto end;
  }

  if (!(houseGraph->houseWasFullyExplored())) {
    ret = houseGraph->dirForUnvisited();
    if (ret != NOT_EXISTS)
      goto end;
  }

  if (!(houseGraph->houseWasFullyCleaned())) {
    ret = houseGraph->dirForDirty();
    if (ret != NOT_EXISTS)
      goto end;
  }

  ret = houseGraph->dirForDocking();
  if (ret == STAY) {
    // from robot POV, house cleaning is finished.
    ret = NOT_EXISTS;
  }

end:
  houseGraph->updateCurrent(ret);
  return ret;
}
