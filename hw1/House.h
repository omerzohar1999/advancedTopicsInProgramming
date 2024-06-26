#ifndef HOUSE_H
#define HOUSE_H
#include "Direction.h"
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
  bool isWallInDirection(direction dir) const;
  uint32_t getDirtLevel() const;
  void decreaseDirtLevel();
  void addWall(direction dir);
  void printCell() const;
};

class House {
  std::vector<std::vector<HouseCell *>> cells;
  VaccumCleaner *robot;
  Sensor *sensor;
  u_int32_t robot_loc_i;
  u_int32_t robot_loc_j;
  u_int32_t docking_loc_i;
  u_int32_t docking_loc_j;
  u_int32_t house_size_rows;
  u_int32_t house_size_cols;
  u_int32_t max_steps;
  u_int32_t battery_max_size;
  float battery_current_size;
  std::vector<direction> stepsList;

public:
  House(std::string file_name);

  bool isThereWall(direction dir) const;

  int howMuchDirtHere() const;

  int getBatteryLeft() const;
  int getDirtLeft() const;

  bool isDirtLeft() const;

  bool isInDocking() const;

  bool cleaningFinished() const;

  bool end(direction decision) const;

  void updateRobotLocation(direction decision);

  bool changeState();
  void create_output(std::string output_file) const;
};
#endif