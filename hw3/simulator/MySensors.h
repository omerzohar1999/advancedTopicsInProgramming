#ifndef MYSENSORS_H
#define MYSENSORS_H

#include "BatteryMeter.h"
#include "DirtSensor.h"
#include "Simulator.h"
#include "WallSensor.h"
#include <memory>

class MyWallsSensor : public WallsSensor {
  std::weak_ptr<Simulator> sim;

public:
  MyWallsSensor() {};

  ~MyWallsSensor() {};

  void setHouse(std::weak_ptr<Simulator> sim);

  bool isWall(Direction d) const;
};

class MyDirtSensor : public DirtSensor {
  std::weak_ptr<Simulator> sim;

public:
  MyDirtSensor() {};

  ~MyDirtSensor() {};

  void setHouse(std::weak_ptr<Simulator> sim);

  int dirtLevel() const;
};

class MyBatteryMeter : public BatteryMeter {
  std::weak_ptr<Simulator> sim;

public:
  MyBatteryMeter() {};

  ~MyBatteryMeter() {};

  void setHouse(std::weak_ptr<Simulator> sim);

  std::size_t getBatteryState() const;
};

#endif