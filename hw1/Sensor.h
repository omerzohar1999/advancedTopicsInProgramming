#ifndef SENSOR_H
#define SENSOR_H

#include "Direction.h"
#include "House.h"

class Sensor {
  House *house;

public:
  Sensor(House *house);
  u_int32_t getBatteryStepsLeft() const;
  bool isThereWall(direction dir) const;
  int howMuchDirtHere() const;
};
#endif