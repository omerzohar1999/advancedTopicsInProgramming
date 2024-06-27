#include <fstream>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <string>
#include <vector>

#include "Direction.h"
#include "House.h"
#include "Sensor.h"
#include "VaccumCleaner.h"

HouseCell::HouseCell(u_int32_t row, u_int32_t col, bool north, bool east,
                     bool south, bool west, u_int8_t dirt_level)
    : location_rows(row), location_cols(col),
      exists_wall{north, east, south, west}, dirt_level(dirt_level) {
  if (dirt_level > 9) {
    std::cerr << "invalid input: dirt_level above 9 for (" << row << "," << col
              << ")" << std::endl;
    exit(1);
  }
}

HouseCell::HouseCell(std::string line) {
  u_int32_t row;
  u_int32_t col;
  bool n;
  bool e;
  bool s;
  bool w;
  u_int8_t dirt;

  std::stringstream ss(line);
  std::string item;

  std::getline(ss, item, ',');
  row = std::stoul(item);

  std::getline(ss, item, ',');
  col = std::stoul(item);

  std::getline(ss, item, ',');
  n = (item == "true");

  std::getline(ss, item, ',');
  e = (item == "true");

  std::getline(ss, item, ',');
  s = (item == "true");

  std::getline(ss, item, ',');
  w = (item == "true");

  std::getline(ss, item, ',');
  dirt = (u_int8_t)(item[0] - '0');

  std::cout << "(" << row << "," << col << ")n" << n << "e" << e << "s" << s
            << "w" << w << "d" << dirt << std::endl;

  location_rows = row;
  location_cols = col;
  exists_wall[NORTH] = n;
  exists_wall[EAST] = e;
  exists_wall[SOUTH] = s;
  exists_wall[WEST] = w;
  dirt_level = dirt;

  printCell();
}

HouseCell::HouseCell(int x, int y, bool n, bool e, bool s, bool w)
    : location_rows(x), location_cols(y), exists_wall{n, e, s, w},
      dirt_level(0) {};

uint32_t HouseCell::getRow() const { return location_rows; }
uint32_t HouseCell::getCol() const { return location_cols; }
bool HouseCell::isWallInDirection(direction dir) const {
  if (dir > 3)
    return false;
  return exists_wall[dir];
}
uint32_t HouseCell::getDirtLevel() const { return (uint32_t)dirt_level; }
void HouseCell::decreaseDirtLevel() {
  if (dirt_level > 0)
    dirt_level--;
}
void HouseCell::addWall(direction dir) { exists_wall[dir] = true; }

void HouseCell::printCell() const {
  std::cout << "(" << location_rows << "," << location_cols << ")n"
            << exists_wall[NORTH] << "e" << exists_wall[EAST] << "s"
            << exists_wall[SOUTH] << "w" << exists_wall[WEST] << "d"
            << dirt_level << std::endl;
}

House::House(std::string file_name) {
  std::cout << "building house from file" << std::endl;
  std::ifstream file(file_name);
  if (!file.is_open()) {
    std::cerr << "Unable to open file: " << file_name << std::endl;
    exit(1);
  }

  std::string line;
  std::getline(file, line);

  std::stringstream ss(line);
  std::string item;

  std::getline(ss, item, ',');
  this->max_steps = std::stoul(item);

  std::getline(ss, item, ',');
  battery_max_size = std::stoul(item);
  battery_current_size = battery_max_size;

  // create linked list, read all into it, find max x,y
  std::vector<HouseCell *> givenCells;

  // read first line
  // create robot in this location
  std::getline(file, line);
  givenCells.push_back(new HouseCell(line));

  sensor = new Sensor(this);

  robot = new VaccumCleaner(battery_max_size, sensor);
  robot_loc_i = givenCells[0]->getRow();
  robot_loc_j = givenCells[0]->getCol();

  docking_loc_i = robot_loc_i;
  docking_loc_j = robot_loc_j;

  house_size_rows = docking_loc_i;
  house_size_cols = docking_loc_j;

  HouseCell *curr;

  while (std::getline(file, line)) {
    curr = new HouseCell(line);
    if (curr->getRow() + 1 > house_size_rows)
      house_size_rows = curr->getRow() + 1;
    if (curr->getCol() + 1 > house_size_cols)
      house_size_cols = curr->getCol() + 1;
    givenCells.push_back(curr);
  }

  cells.resize(house_size_rows);
  for (u_int32_t i = 0; i < house_size_rows; i++) {
    cells[i].resize(house_size_cols);
  }
  for (u_int32_t i = 0; i < house_size_rows; i++) {
    for (u_int32_t j = 0; j < house_size_cols; j++) {
      cells[i][j] = nullptr;
    }
  }
  HouseCell *cell;
  for (size_t i = 0; i < givenCells.size(); i++) {
    cell = givenCells.at(i);

    if (cells[cell->getRow()][cell->getCol()] != nullptr) {
      std::cout << "ERROR" << std::endl;
      std::cout << "cell (" << cell->getRow() << "," << cell->getCol()
                << ") was inserted twice" << std::endl;
      exit(1);
    }
    cells[cell->getRow()][cell->getCol()] = cell;
  }

  for (u_int32_t i = 0; i < house_size_rows; i++) {
    for (u_int32_t j = 0; j < house_size_cols; j++) {
      if (cells[i][j] == nullptr) {
        bool n = false;
        bool e = false;
        bool s = false;
        bool w = false;
        if (i == 0 || (i > 0 && cells[i - 1][j] != nullptr &&
                       cells[i - 1][j]->isWallInDirection(SOUTH))) {
          n = true;
        }
        if (j == 0 || (j > 0 && cells[i][j - 1] != nullptr &&
                       cells[i][j - 1]->isWallInDirection(EAST))) {
          w = true;
        }
        if (i == house_size_rows - 1 ||
            (i < house_size_rows - 1 && cells[i + 1][j] != nullptr &&
             cells[i - 1][j]->isWallInDirection(WEST))) {
          e = true;
        }
        if (j == house_size_cols - 1 ||
            (j < house_size_cols - 1 && cells[i][j + 1] != nullptr &&
             cells[i][j + 1]->isWallInDirection(NORTH))) {
          s = true;
        }
        cells[i][j] = new HouseCell(i, j, n, e, s, w);
      } else {
        // check consistency
        if (i > 0 && cells[i - 1][j] != nullptr &&
            cells[i][j]->isWallInDirection(NORTH) !=
                cells[i - 1][j]->isWallInDirection(SOUTH)) {
          std::cout << "consistency error:" << std::endl;
          std::cout << "(" << i << "," << j << "), wallInNorth:"
                    << cells[i][j]->isWallInDirection(NORTH) << std::endl;
          std::cout << "(" << i - 1 << "," << j << "), wallInSouth:"
                    << cells[i - 1][j]->isWallInDirection(SOUTH) << std::endl;
          exit(1);
        }
        if (i < house_size_rows - 1 && cells[i + 1][j] != nullptr &&
            cells[i][j]->isWallInDirection(SOUTH) !=
                cells[i + 1][j]->isWallInDirection(NORTH)) {
          std::cout << "consistency error:" << std::endl;
          std::cout << "(" << i << "," << j << "), wallInSouth:"
                    << cells[i][j]->isWallInDirection(SOUTH) << std::endl;
          std::cout << "(" << i + 1 << "," << j << "), wallInNorth:"
                    << cells[i + 1][j]->isWallInDirection(NORTH) << std::endl;
          exit(1);
        }
        if (j > 0 && cells[i][j - 1] != nullptr &&
            cells[i][j]->isWallInDirection(WEST) !=
                cells[i][j - 1]->isWallInDirection(EAST)) {
          std::cout << "consistency error:" << std::endl;
          std::cout << "(" << i << "," << j
                    << "), wallInWest:" << cells[i][j]->isWallInDirection(WEST)
                    << std::endl;
          std::cout << "(" << i << "," << j - 1 << "), wallInEast:"
                    << cells[i][j - 1]->isWallInDirection(EAST) << std::endl;
          exit(1);
        }
        if (j < house_size_cols - 1 && cells[i][j + 1] != nullptr &&
            cells[i][j]->isWallInDirection(EAST) !=
                cells[i][j + 1]->isWallInDirection(WEST)) {
          std::cout << "consistency error:" << std::endl;
          std::cout << "(" << i << "," << j
                    << "), wallInEast:" << cells[i][j]->isWallInDirection(EAST)
                    << std::endl;
          std::cout << "(" << i << "," << j + 1 << "), wallInWest:"
                    << cells[i][j + 1]->isWallInDirection(WEST) << std::endl;
          exit(1);
        }
      }
    }
  }
  for (u_int32_t i = 0; i < house_size_rows; i++) {
    cells[i][0]->addWall(WEST);
    cells[i][house_size_cols - 1]->addWall(EAST);
  }
  for (u_int32_t j = 0; j < house_size_cols; j++) {
    cells[0][j]->addWall(NORTH);
    cells[house_size_rows - 1][j]->addWall(SOUTH);
  }
  std::cout << "finished building house from file" << std::endl;
}

bool House::isThereWall(direction dir) const {
  return this->cells[this->robot_loc_i][this->robot_loc_j]->isWallInDirection(
      dir);
}

int House::howMuchDirtHere() const {
  std::cout << "(" << robot_loc_i << "," << robot_loc_j << ")" << std::endl;
  std::cout << cells[robot_loc_i][robot_loc_j] << std::endl;
  std::cout << cells[robot_loc_i][robot_loc_j]->getDirtLevel() << std::endl;
  return cells[robot_loc_i][robot_loc_j]->getDirtLevel();
}

int House::getBatteryLeft() const { return floor(battery_current_size); }
int House::getDirtLeft() const {
  int ret = 0;
  for (u_int32_t row = 0; row < house_size_rows; row++) {
    for (u_int32_t col = 0; col < house_size_cols; col++) {
      ret += (cells[row][col]->getDirtLevel());
    }
  }
  return ret;
}

bool House::isDirtLeft() const {
  for (u_int32_t row = 0; row < house_size_rows; row++) {
    for (u_int32_t col = 0; col < house_size_cols; col++) {
      if (cells[row][col]->getDirtLevel() > 0)
        return true;
    }
  }
  return false;
}

bool House::isInDocking() const {
  return robot_loc_i == docking_loc_i && robot_loc_j == docking_loc_j;
}

bool House::cleaningFinished() const { return isInDocking() && !isDirtLeft(); }

bool House::end(direction decision) const {
  return cleaningFinished() ||
         (!(isInDocking() && decision == STAY) && battery_current_size < 1) ||
         stepsList.size() > max_steps || decision == NOT_EXISTS;
}
void House::updateRobotLocation(direction decision) {
  switch (decision) {
  case NORTH:
    robot_loc_i++;
    break;
  case EAST:
    robot_loc_j--;
    break;
  case SOUTH:
    robot_loc_i--;
    break;
  case WEST:
    robot_loc_j++;
    break;
  default:
    break;
  }
}

bool House::changeState() {
  std::cout << "step no. " << stepsList.size() << std::endl;

  if (cleaningFinished()) {
    return true;
  }
  direction decision = robot->getStep();
  std::cout << "HOUSEPOV: robot is in (" << robot_loc_i << "," << robot_loc_j
            << ")" << std::endl;
  std::cout << "HOUSEPOV: decision is " << directionString(decision)
            << std::endl;
  stepsList.push_back(decision);

  if (end(decision))
    return true;

  if (!isInDocking() || decision != STAY) {
    battery_current_size -= 1;
  }

  if (cells[robot_loc_i][robot_loc_j]->isWallInDirection(decision)) {
    std::cerr << "ERROR: robot went into wall" << std::endl;
    exit(1);
  }
  switch (decision) {
  case STAY:
    cells[robot_loc_i][robot_loc_j]->decreaseDirtLevel();
    if (isInDocking()) {
      battery_current_size += ((float)battery_max_size / 20);
    }
    break;
  case NORTH:
    robot_loc_i--;
    break;
  case EAST:
    robot_loc_j++;
    break;
  case SOUTH:
    robot_loc_i++;
    break;
  case WEST:
    robot_loc_j--;
    break;
  default:
    return false;
    break;
  }
  return false;
}

void House::create_output(std::string output_file) const {
  std::cout << "creating output for house" << std::endl;
  std::cout << "creating output for house" << std::endl;
  std::ofstream file(output_file);
  if (!file.is_open()) {
    std::cerr << "Unable to open file: " << output_file << std::endl;
    exit(1);
  }

  for (direction d : stepsList) {
    file << directionString(d) << std::endl;
  }
  file << "Number of steps: " << stepsList.size() << std::endl;
  file << "Dirt left: " << getDirtLeft() << std::endl;
  file << "Vaccum cleaner is " << (getBatteryLeft() == 0 ? "" : "not ")
       << "dead" << std::endl;
  file << "Mission ended " << (cleaningFinished() ? "" : "not ")
       << "successfuly" << std::endl;

  file.close();
}
