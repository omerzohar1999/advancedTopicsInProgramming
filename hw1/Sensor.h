#ifndef SENSOR_H
#define SENSOR_H

#include "Direction.h"
#include "House.h"

class Sensor {
  const House *house;

public:
  Sensor(House *house);
  u_int32_t getBatteryStepsLeft() const;
  bool isThereWall(Direction dir) const;
  int howMuchDirtHere() const;
};
#endif