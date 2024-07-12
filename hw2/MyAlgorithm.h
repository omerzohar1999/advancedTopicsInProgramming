#ifndef MYALGO_H
#define MYALGO_H
#include "AlgorithmGraph.h"
#include "abstract_algorithm.h"
#include <memory>

class MyAlgorithm : public AbstractAlgorithm,
                    public std::enable_shared_from_this<MyAlgorithm> {
  u_int32_t battery_max_size = 0;
  const WallsSensor *wallSensor;
  const BatteryMeter *batteryMeter;
  const DirtSensor *dirtSensor;
  AlgorithmGraph houseGraph;
  size_t cur_steps_left = 0;

  size_t getBatteryStepsLeft() const;
  bool isCharging() const;
  bool mustGoCharge(int distFromDocking) const;
  bool isRunningOutOfStepsUnvisited();
  bool isRunningOutOfStepsDirty();

public:
  MyAlgorithm();
  ~MyAlgorithm() {};
  void setMaxSteps(std::size_t maxSteps);
  void setWallsSensor(const WallsSensor &);
  void setDirtSensor(const DirtSensor &);
  void setBatteryMeter(const BatteryMeter &);
  Step nextStep();
};
#endif