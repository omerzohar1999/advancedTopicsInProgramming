#ifndef HOUSE_H
#define HOUSE_H
#include "DevTools.h"
#include "Direction.h"
#include <filesystem>
#include <map>
#include <string>
#include <vector>

class AbstractAlgorithm;
class Sensor;

class HouseCell {
  bool is_wall = false;
  u_int8_t dirt_level;

public:
  HouseCell(bool is_wall, u_int8_t dirt_level);

  uint32_t getDirtLevel() const;
  void decreaseDirtLevel();
  void setWall();
  bool getIsWall() const;
};

class MySimulator : public std::enable_shared_from_this<MySimulator> {
  std::vector<std::vector<HouseCell>> cells;
  std::unique_ptr<AbstractAlgorithm> robot;
  std::shared_ptr<DevTools> devTools;
  u_int32_t robot_loc_i;
  u_int32_t robot_loc_j;
  u_int32_t docking_loc_i;
  u_int32_t docking_loc_j;
  u_int32_t house_size_rows;
  u_int32_t house_size_cols;
  u_int32_t max_steps;
  u_int32_t battery_max_size;
  float battery_current_size;
  bool error = false;

  std::vector<Step> stepsList;

  int getDirtLeft() const;
  bool isDirtLeft() const;
  bool isInDocking() const;
  bool cleaningFinished() const;
  bool robotDied() const;
  bool end() const;
  void updateRobotLocation(Step decision);
  void updateRobotBattery(Step decision);
  void updateHouseDirt(Step decision);
  HouseCell *robotCurrentCell() const;
  bool isCharging(Step decision) const;
  bool isBadStep(Step decision) const;
  bool changeState();
  void updateVisualization(Step decision);
  std::filesystem::path
  addPrefixToFilePath(const std::filesystem::path &file_path,
                      const std::string &prefix) const;

public:
  MySimulator(std::string file_name);
  bool isThereWall(Direction dir) const;
  int howMuchDirtHere() const;
  int getBatteryLeft() const;
  void setAlgorithm(AbstractAlgorithm &algo);
  bool run();
  bool createOutput(std::string input_file) const;
};
#endif