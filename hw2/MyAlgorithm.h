#ifndef MYALGORITHM_H
#define MYALGORITHM_H
#include <AbstractAlgorithm.h>

class MyAlgorithm : public AbstractAlgorithm {
  const u_int32_t battery_max_size = 0;
  std::unique_ptr<const WallsSensor> wallSensor;
  std::unique_ptr<const BatteryMeter> batteryMeter;
  std::unique_ptr<const DirtSensor> dirtSensor;
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
  void setMaxSteps(std::size_t maxSteps) = 0;
  void setWallsSensor(const WallsSensor &) = 0;
  void setDirtSensor(const DirtSensor &) = 0;
  void setBatteryMeter(const BatteryMeter &) = 0;
  Step nextStep() = 0;
};
#endif