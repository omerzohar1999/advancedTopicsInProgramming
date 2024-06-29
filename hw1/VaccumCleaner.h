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

  u_int32_t getBatteryStepsLeft() const;
  bool isCharging() const;
  bool mustGoCharge() const;

public:
  VaccumCleaner(const u_int32_t battery_max_size, const Sensor *sensor);

  Direction getStep();
};
#endif