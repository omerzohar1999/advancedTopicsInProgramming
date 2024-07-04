#ifndef MYSENSORS_H
#define MYSENSORS_H

#include <Sensors.h>

class MyWallsSensor : public WallsSensor {
  std::shared_ptr<const MySimulator> sim;

public:
  MyWallsSensor(std::shared_ptr<const MySimulator> sim);
  ~MyWallsSensor() {};
  bool isWall(Direction d) const;
};

class MyDirtSensor : public DirtSensor {
  std::shared_ptr<const MySimulator> sim;

public:
  MyDirtSensor(std::shared_ptr<const MySimulator> sim);
  ~MyDirtSensor() {};
  int dirtLevel() const;
};

class MyBatteryMeter : public BatteryMeter {
  std::shared_ptr<const MySimulator> sim;

public:
  MyBatteryMeter(std::shared_ptr<const MySimulator> sim);
  ~MyBatteryMeter() {}
  std::size_t getBatteryState() const;
};
#endif