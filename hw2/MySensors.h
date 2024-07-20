#ifndef MYSENSORS_H
#define MYSENSORS_H

#include "Simulator.h"
#include "battery_meter.h"
#include "dirt_sensor.h"
#include "wall_sensor.h"
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