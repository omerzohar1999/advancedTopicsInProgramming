#ifndef VACUUMCLEANER_H
#define VACUUMCLEANER_H

#include "Direction.h"
#include "House.h"
#include "Sensor.h"
#include "VaccumGraph.h"
#include <functional>

class VaccumCleaner {
  const u_int32_t battery_max_size;
  const Sensor *sensor;
  VaccumGraph *houseGraph;
  u_int32_t cur_steps_left;

  u_int32_t getBatteryStepsLeft() const;
  bool isCharging() const;
  bool mustGoCharge() const;
  bool isRunningOutOfStepsUnvisited() const;
  bool isRunningOutOfStepsDirty() const;

public:
  VaccumCleaner(const u_int32_t battery_max_size, const Sensor *sensor,
                u_int32_t max_steps);

  Direction getStep();

};
#endif