#ifndef MYALGO_H
#define MYALGO_H

#include "algo_common/AlgorithmGraph.h"
#include "common/AbstractAlgorithm.h"
#include <memory>

typedef std::pair<std::pair<int, Direction>, int> DistAndDir;

class BaseAlgorithm : public AbstractAlgorithm {

public:
  size_t battery_max_size = 0;
  const WallsSensor *wallSensor;
  const BatteryMeter *batteryMeter;
  const DirtSensor *dirtSensor;
  AlgorithmGraph houseGraph;
  size_t cur_steps_left = 0;

  size_t getBatteryStepsLeft() const;

  bool isFullyCharged();

  bool hasEnoughChargeDirty(int dockingDist, int dirtyDist);

  bool hasEnoughChargeUnvisited(int dockingDist, int unvisitedDist);

  void setMaxSteps(size_t maxSteps);

  void setWallsSensor(const WallsSensor &);

  void setDirtSensor(const DirtSensor &);

  void setBatteryMeter(const BatteryMeter &);
};

#endif