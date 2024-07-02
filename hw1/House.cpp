#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <string>
#include <vector>
#include <filesystem>

#include "Direction.h"
#include "House.h"
#include "Sensor.h"
#include "VaccumCleaner.h"
#include "DevTools.h"

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

  location_rows = row;
  location_cols = col;
  exists_wall[NORTH] = n;
  exists_wall[EAST] = e;
  exists_wall[SOUTH] = s;
  exists_wall[WEST] = w;
  dirt_level = dirt;
}

HouseCell::HouseCell(int x, int y, bool n, bool e, bool s, bool w)
    : location_rows(x), location_cols(y), exists_wall{n, e, s, w},
      dirt_level(0) {};

uint32_t HouseCell::getRow() const { return location_rows; }
uint32_t HouseCell::getCol() const { return location_cols; }
bool HouseCell::isWallInDirection(Direction dir) const {
  if (dir > 3)
    return false;
  return exists_wall[dir];
}
uint32_t HouseCell::getDirtLevel() const { return (uint32_t)dirt_level; }
void HouseCell::decreaseDirtLevel() {
  if (dirt_level > 0)
    dirt_level--;
}
void HouseCell::addWall(Direction dir) {
  if (dir < STAY)
    exists_wall[dir] = true;
}

House::House(std::string file_name) {
    try {
        std::filesystem::path file_path(file_name);

        // Check if the path is relative or absolute
        if (!file_path.is_absolute()) {
            // Resolve relative path
            file_path = std::filesystem::absolute(file_path);
        }

        std::ifstream file(file_path.string());
        if (!file.is_open()) {
            std::cerr << "Unable to open file: " << file_path << std::endl;
            error = true;
            return;
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
        devTools = new DevTools();

        robot = new VaccumCleaner(battery_max_size, sensor, max_steps);
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
                std::cerr << "ERROR: cell (" << cell->getRow() << "," << cell->getCol()
                          << ") was inserted twice" << std::endl;
                error = true;
                return;
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
                        std::cerr << "consistency error:" << std::endl;
                        std::cerr << "(" << i << "," << j << "), wallInNorth:"
                                  << cells[i][j]->isWallInDirection(NORTH) << std::endl;
                        std::cerr << "(" << i - 1 << "," << j << "), wallInSouth:"
                                  << cells[i - 1][j]->isWallInDirection(SOUTH) << std::endl;
                        error = true;
                        return;
                    }
                    if (i < house_size_rows - 1 && cells[i + 1][j] != nullptr &&
                        cells[i][j]->isWallInDirection(SOUTH) !=
                        cells[i + 1][j]->isWallInDirection(NORTH)) {
                        std::cerr << "consistency error:" << std::endl;
                        std::cerr << "(" << i << "," << j << "), wallInSouth:"
                                  << cells[i][j]->isWallInDirection(SOUTH) << std::endl;
                        std::cerr << "(" << i + 1 << "," << j << "), wallInNorth:"
                                  << cells[i + 1][j]->isWallInDirection(NORTH) << std::endl;
                        error = true;
                        return;
                    }
                    if (j > 0 && cells[i][j - 1] != nullptr &&
                        cells[i][j]->isWallInDirection(WEST) !=
                        cells[i][j - 1]->isWallInDirection(EAST)) {
                        std::cerr << "consistency error:" << std::endl;
                        std::cerr << "(" << i << "," << j
                                  << "), wallInWest:" << cells[i][j]->isWallInDirection(WEST)
                                  << std::endl;
                        std::cerr << "(" << i << "," << j - 1 << "), wallInEast:"
                                  << cells[i][j - 1]->isWallInDirection(EAST) << std::endl;
                        error = true;
                        return;
                    }
                    if (j < house_size_cols - 1 && cells[i][j + 1] != nullptr &&
                        cells[i][j]->isWallInDirection(EAST) !=
                        cells[i][j + 1]->isWallInDirection(WEST)) {
                        std::cerr << "consistency error:" << std::endl;
                        std::cerr << "(" << i << "," << j
                                  << "), wallInEast:" << cells[i][j]->isWallInDirection(EAST)
                                  << std::endl;
                        std::cerr << "(" << i << "," << j + 1 << "), wallInWest:"
                                  << cells[i][j + 1]->isWallInDirection(WEST) << std::endl;
                        error = true;
                        return;
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
    } catch (const std::ifstream::failure& e) {
        std::cerr << "Exception opening/reading file: " << e.what() << std::endl;
        error = true;
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        error = true;
    }

}

HouseCell *House::robotCurrentCell() const {
  return cells[robot_loc_i][robot_loc_j];
}
bool House::isThereWall(Direction dir) const {
  return robotCurrentCell()->isWallInDirection(dir);
}

int House::howMuchDirtHere() const {
  return robotCurrentCell()->getDirtLevel();
}

int House::getBatteryLeft() const { return std::floor(battery_current_size); }
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

bool House::robotDied() const {
  return !isInDocking() && battery_current_size < 1;
}

bool House::end() const {
  return cleaningFinished() || robotDied() || stepsList.size() >= max_steps;
}

void House::updateRobotLocation(Direction decision) {
  robot_loc_i = locIByDirection(robot_loc_i, decision);
  robot_loc_j = locJByDirection(robot_loc_j, decision);
}

void House::updateRobotBattery(Direction decision) {
  if (isCharging(decision)) {
    battery_current_size += ((float)battery_max_size / 20);
  } else {
    battery_current_size -= 1;
  }
}

void House::updateHouseDirt(Direction decision) {
  if (decision == STAY) {
    robotCurrentCell()->decreaseDirtLevel();
  }
}

inline bool House::isCharging(Direction decision) const {
  return isInDocking() && decision == STAY;
}

bool House::isBadStep(Direction decision) const {
  if (robotCurrentCell()->isWallInDirection(decision)) {
    return true;
  }
  if (battery_current_size < 1 && !isCharging(decision)) {
    return true;
  }
  return false;
}

bool House::changeState() {
  // gets one step from the robot and updates house accordingly.
  // returns whether the simulation finished.

  Direction decision = robot->getStep();

  if (decision == NOT_EXISTS)
    return true;

  if (isBadStep(decision)) {
    error = true;
    return true;
  }

  stepsList.push_back(decision);

  updateRobotLocation(decision);
  updateRobotBattery(decision);
  updateHouseDirt(decision);

  updateVisualization(decision);

  return end();
}

bool House::clean() {
  // runs the whole simulation, stops when it ends or if an error occurred.
  // returns whether the cleaning finished successfully.

  while (!error && !changeState()) {
  }
  return !error;
}

bool House::createOutput(std::string input_file) const {
  // creates output file.
  // returns whether output creation finished successfully.

  try {
      std::filesystem::path input_file_path(input_file);

      std::filesystem::path output_file_path = addPrefixToFilePath(input_file_path, "output_");

      // Check if the path is relative or absolute
      if (!output_file_path.is_absolute()) {
          // Resolve relative path
          output_file_path = std::filesystem::absolute(output_file_path);
      }

      std::ofstream file(output_file_path.string());
      if (!file.is_open()) {
          std::cerr << "Unable to open file: " << input_file << std::endl;
          return false;
      }

      for (Direction d : stepsList) {
          file << directionString(d) << std::endl;
      }
      file << "Number of steps: " << stepsList.size() << std::endl;
      file << "Dirt left: " << getDirtLeft() << std::endl;
      file << "Vaccum cleaner is " << (robotDied() ? "dead" : "alive") << std::endl;
      file << "Mission ended " << (cleaningFinished() ? "" : "un") << "successfuly"
           << std::endl;

      file.close();
      return true;

  } catch (const std::ofstream::failure& e) {
      std::cerr << "Exception writing to output file: " << e.what() << std::endl;
      return false;
  } catch (const std::exception& e) {
      std::cerr << "Exception: " << e.what() << std::endl;
      return false;
  }
}

void House::updateVisualization(Direction decision) {
    if (!devTools->isVisualizationEnabled())
        return;

    try {
        std::string file_name = devTools->getVisualizationFileName();
        std::ofstream outFile(file_name, std::ios::app);
        if (!outFile) {
            std::cerr << "Error opening file for writing." << std::endl;
            return;
        }

        // Write the direction
        outFile << directionString(decision) << std::endl;
        outFile << std::endl;  // Empty line

        // Convert cells to ASCII table
        size_t rows = cells.size();
        size_t cols = rows > 0 ? cells[0].size() : 0;

        for (size_t i = 0; i < rows; ++i) {
            // Top walls
            for (size_t j = 0; j < cols; ++j) {
                if (cells[i][j]->isWallInDirection(NORTH)) {
                    outFile << "+---";
                } else {
                    outFile << "+   ";
                }
            }
            outFile << "+" << std::endl;

            // Side walls and dirt level
            for (size_t j = 0; j < cols; ++j) {
                if (cells[i][j]->isWallInDirection(WEST)) {
                    outFile << "|";
                } else {
                    outFile << " ";
                }
                outFile << " ";
                if (i == robot_loc_i && j == robot_loc_j) {
                    outFile << "V";
                } else if (i == docking_loc_i && j == docking_loc_j) {
                    outFile << "D";
                } else {
                    outFile << cells[i][j]->getDirtLevel();
                }
                outFile << " ";
            }
            outFile << "|" << std::endl;
        }

        // Bottom walls
        for (size_t j = 0; j < cols; ++j) {
            if (cells[rows - 1][j]->isWallInDirection(SOUTH)) {
                outFile << "+---";
            } else {
                outFile << "+   ";
            }
        }
        outFile << "+" << std::endl;
        outFile << std::endl;  // Empty line
        outFile.close();
    } catch (const std::ofstream::failure& e) {
        std::cerr << "Exception writing to file: " << e.what() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
}

std::filesystem::path House::addPrefixToFilePath(const std::filesystem::path& file_path, const std::string& prefix) const {
    // Extract the parent path (directory)
    std::filesystem::path parent_path = file_path.parent_path();

    // Extract the filename and add the prefix
    std::string new_filename = prefix + file_path.filename().string();

    // Construct the new file path
    std::filesystem::path new_file_path = parent_path / new_filename;

    return new_file_path;
}
