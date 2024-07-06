#ifndef SENSOR_H
#define SENSOR_H

#include "Direction.h"

class Sensor {};

class WallsSensor : public Sensor {
public:
  virtual ~WallsSensor() {};
  virtual bool isWall(Direction d) const = 0;
};

class DirtSensor : public Sensor {
public:
  virtual ~DirtSensor() {};
  virtual int dirtLevel() const = 0;
};

class BatteryMeter : public Sensor {
public:
  virtual ~BatteryMeter() {}
  virtual std::size_t getBatteryState() const = 0;
};
#endif