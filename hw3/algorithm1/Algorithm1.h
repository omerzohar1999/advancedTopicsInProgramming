#ifndef MYALGO_H
#define MYALGO_H

#include "AbstractAlgorithm.h"
#include "AlgorithmGraph.h"
#include <memory>

class Algorithm1 : public AbstractAlgorithm,
                   public std::enable_shared_from_this<Algorithm1> {
  size_t battery_max_size = 0;
  const WallsSensor *wallSensor;
  const BatteryMeter *batteryMeter;
  const DirtSensor *dirtSensor;
  AlgorithmGraph houseGraph;
  size_t cur_steps_left = 0;

  size_t getBatteryStepsLeft() const;

  Step getNextStep();

  bool isFullyCharged();

  bool hasEnoughChargeDirty(int dockingDist, int dirtyDist);

  bool hasEnoughChargeUnvisited(int dockingDist, int unvisitedDist);

  bool hasEnoughChargeToClean(int dockingDist);

public:
  ~Algorithm1() {};

  void setMaxSteps(size_t maxSteps);

  void setWallsSensor(const WallsSensor &);

  void setDirtSensor(const DirtSensor &);

  void setBatteryMeter(const BatteryMeter &);

  Step nextStep();
};

#endif