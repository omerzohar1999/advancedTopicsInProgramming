#ifndef HOUSE_H
#define HOUSE_H
#include "Direction.h"
#include "DevTools.h"
#include <string>
#include <vector>

class VaccumCleaner;
class Sensor;

class HouseCell {
  u_int32_t location_rows;
  u_int32_t location_cols;
  bool exists_wall[4];
  u_int8_t dirt_level;
  HouseCell(u_int32_t row, u_int32_t col, bool north, bool east, bool south,
            bool west, u_int8_t dirt_level);

public:
  HouseCell(std::string line);
  HouseCell(int x, int y, bool n, bool e, bool s, bool w);

  uint32_t getRow() const;
  uint32_t getCol() const;
  bool isWallInDirection(Direction dir) const;
  uint32_t getDirtLevel() const;
  void decreaseDirtLevel();
  void addWall(Direction dir);

  void printCell() const;
};

class House {
  std::vector<std::vector<HouseCell *>> cells;
  VaccumCleaner *robot;
  Sensor *sensor;
  DevTools *devTools;
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

  std::vector<Direction> stepsList;

  int getDirtLeft() const;
  bool isDirtLeft() const;
  bool isInDocking() const;
  bool cleaningFinished() const;
  bool robotDied() const;
  bool end() const;
  void updateRobotLocation(Direction decision);
  void updateRobotBattery(Direction decision);
  void updateHouseDirt(Direction decision);
  HouseCell *robotCurrentCell() const;
  bool isCharging(Direction decision) const;
  bool isBadStep(Direction decision) const;
  bool changeState();
  void updateVisualization();

public:
  House(std::string file_name);
  bool isThereWall(Direction dir) const;
  int howMuchDirtHere() const;
  int getBatteryLeft() const;
  bool clean();
  bool createOutput(std::string output_file) const;

    void updateVisualization(Direction decision);
};
#endif