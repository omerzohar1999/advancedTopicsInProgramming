#ifndef VACUUMCLEANER_H
#define VACUUMCLEANER_H

#include "Direction.h"
#include "House.h"
#include "Sensor.h"
#include "VaccumGraph.h"
#include <functional>

class VaccumCleaner {
  Sensor *sensor;
  VaccumGraph *houseGraph;
  u_int32_t battery_max_size;
  u_int32_t getBatteryStepsLeft();
  bool isThereWall(Direction dir);
  int howMuchDirtHere();

public:
  VaccumCleaner(u_int32_t battery_max_size, Sensor *sensor);

  Direction getStep();
};
#endif