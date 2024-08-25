#ifndef MYSIM_H
#define MYSIM_H

#include "common/AbstractAlgorithm.h"
#include "common/enums.h"
#include "simulator/DevTools.h"
#include "simulator/MySensors.h"
#include "simulator/Simulator.h"
#include <filesystem>
#include <map>
#include <string>
#include <vector>

class HouseCell {
  bool is_wall = false;
  int dirt_level = 0;

public:
  HouseCell();

  int getDirtLevel() const;

  void setDirtLevel(int dirt_level);

  void decreaseDirtLevel();

  void setWall();

  bool getIsWall() const;
};

class MySimulator : public Simulator {
  std::string house_name;
  std::vector<std::vector<HouseCell>> cells;
  MyBatteryMeter batteryMeter;
  MyWallsSensor wallsSensor;
  MyDirtSensor dirtSensor;
  AbstractAlgorithm *robot;
  std::unique_ptr<DevTools> devTools;
  int robot_loc_i = -1;
  int robot_loc_j = -1;
  int docking_loc_i = -1;
  int docking_loc_j = -1;
  int house_size_rows = -1;
  int house_size_cols = -1;
  size_t max_steps = -1;
  size_t battery_max_size = -1;
  float battery_current_size = -1;
  bool error = false;
  int initial_dirt;

  std::vector<Step> stepsList;

  void printHouse();

  int getDirtLeft() const;

  bool isDirtLeft() const;

  bool isInDocking() const;

  bool cleaningFinished() const;

  bool robotDied() const;

  std::string statusString() const;

  bool end() const;

  void updateRobotLocation(Step decision);

  void updateRobotBattery(Step decision);

  void updateHouseDirt(Step decision);

  bool isCharging(Step decision) const;

  bool isBadStep(Step decision) const;

  bool changeState();

  void updateVisualization(Step decision);

  static std::filesystem::path
  addPrefixToFilePath(const std::filesystem::path &file_path,
                      const std::string &prefix);

public:
  MySimulator();

  MySimulator(const MySimulator &other);

  bool isThereWall(Direction dir) const;

  int howMuchDirtHere() const;

  float getBatteryLeft() const;

  void setAlgorithm(AbstractAlgorithm &algo);

  bool run();

  bool createOutput(std::string input_file, bool finished_in_time) const;

  bool readHouseFile(std::string file_name);

  int calculate_score(bool finished_in_time) const;

  int getMaxSteps() const;

  bool isError() const;
};

#endif