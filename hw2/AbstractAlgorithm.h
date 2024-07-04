#ifndef VACUUMCLEANER_H
#define VACUUMCLEANER_H

#include "AlgorithmGraph.h"
#include "Direction.h"
#include "House.h"
#include "Sensors.h"
#include <functional>

class AbstractAlgorithm {

public:
  virtual ~AbstractAlgorithm() {};
  virtual void setMaxSteps(std::size_t maxSteps) = 0;
  virtual void setWallsSensor(const WallsSensor &) = 0;
  virtual void setDirtSensor(const DirtSensor &) = 0;
  virtual void setBatteryMeter(const BatteryMeter &) = 0;

  virtual Step nextStep() = 0;
};
#endif