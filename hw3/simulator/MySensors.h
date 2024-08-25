#ifndef MYSENSORS_H
#define MYSENSORS_H

#include "common/BatteryMeter.h"
#include "common/DirtSensor.h"
#include "common/WallSensor.h"
#include "simulator/Simulator.h"
#include <memory>

class MyWallsSensor : public WallsSensor {
  Simulator *sim;

public:
  MyWallsSensor(){};

  ~MyWallsSensor(){};

  void setHouse(Simulator *sim);

  bool isWall(Direction d) const;
};

class MyDirtSensor : public DirtSensor {
  Simulator *sim;

public:
  MyDirtSensor(){};

  ~MyDirtSensor(){};

  void setHouse(Simulator *sim);

  int dirtLevel() const;
};

class MyBatteryMeter : public BatteryMeter {
  Simulator *sim;

public:
  MyBatteryMeter(){};

  ~MyBatteryMeter(){};

  void setHouse(Simulator *sim);

  std::size_t getBatteryState() const;
};

#endif