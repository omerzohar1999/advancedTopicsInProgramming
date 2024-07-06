#ifndef MYSENSORS_H
#define MYSENSORS_H

#include "Sensors.h"
#include "Simulator.h"

class MyWallsSensor : public WallsSensor {
  const Simulator *sim;

public:
  MyWallsSensor() {};
  MyWallsSensor(MyWallsSensor &s);
  ~MyWallsSensor() {};
  void setHouse(const Simulator &sim);
  bool isWall(Direction d) const;
};

class MyDirtSensor : public DirtSensor {
  const Simulator *sim;

public:
  MyDirtSensor() {};
  MyDirtSensor(MyDirtSensor &s);
  ~MyDirtSensor() {};
  void setHouse(const Simulator &sim);
  int dirtLevel() const;
};

class MyBatteryMeter : public BatteryMeter {
  const Simulator *sim;

public:
  MyBatteryMeter() {};
  MyBatteryMeter(MyBatteryMeter &m);
  ~MyBatteryMeter() {};
  void setHouse(const Simulator &sim);
  std::size_t getBatteryState() const;
};
#endif