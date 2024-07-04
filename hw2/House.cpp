#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdio.h>
#include <string>
#include <vector>

#include "AbstractAlgorithm.h"
#include "DevTools.h"
#include "Direction.h"
#include "House.h"
#include "MySensors.h"

HouseCell::HouseCell(bool is_wall, u_int8_t dirt_level)
    : dirt_level(dirt_level), is_wall(is_wall) {};

uint32_t HouseCell::getDirtLevel() const { return (uint32_t)dirt_level; }
void HouseCell::decreaseDirtLevel() {
  if (dirt_level > 0)
    dirt_level--;
}
void HouseCell::setWall() { is_wall = true; }

bool HouseCell::getIsWall() const { return is_wall; }

MySimulator::MySimulator(std::string file_name) {
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
    std::getline(file, line); // first line is name: ignore
    std::getline(file, line);

    std::stringstream ss(line);
    std::string item;

    std::getline(ss, item, '=');
    item.erase(std::remove(item.begin(), item.end(), ' '), item.end());
    if (item != "MaxSteps") {
      // error
    }
    std::getline(ss, item);
    // TODO: check the following value is indeed integer
    this->max_steps = std::stoul(item);

    std::getline(file, line);

    std::getline(ss, item, '=');
    item.erase(std::remove(item.begin(), item.end(), ' '), item.end());
    if (item != "MaxBattery") {
      // error
    }

    std::getline(ss, item, '=');
    item.erase(std::remove(item.begin(), item.end(), ' '), item.end());
    // TODO: check the following value is indeed integer
    battery_max_size = std::stoul(item);
    battery_current_size = battery_max_size;

    std::getline(file, line);

    std::getline(ss, item, '=');
    item.erase(std::remove(item.begin(), item.end(), ' '), item.end());
    if (item != "Rows") {
      // error
    }

    std::getline(ss, item, '=');
    item.erase(std::remove(item.begin(), item.end(), ' '), item.end());
    // TODO: check the following value is indeed integer
    house_size_rows = std::stoul(item);

    std::getline(file, line);

    std::getline(ss, item, '=');
    item.erase(std::remove(item.begin(), item.end(), ' '), item.end());
    if (item != "Cols") {
      // error
    }

    std::getline(ss, item, '=');
    item.erase(std::remove(item.begin(), item.end(), ' '), item.end());
    // TODO: check the following value is indeed integer
    house_size_cols = std::stoul(item);

  } catch (const std::ifstream::failure &e) {
    std::cerr << "Exception opening/reading file: " << e.what() << std::endl;
    error = true;
  } catch (const std::exception &e) {
    std::cerr << "Exception: " << e.what() << std::endl;
    error = true;
  }
}

bool MySimulator::isThereWall(Direction dir) const {
  switch (dir) {
  case Direction::North:
    return robot_loc_i == 0 || cells[robot_loc_i - 1][robot_loc_j].getIsWall();
    break;
  case Direction::East:
    return robot_loc_j == house_size_cols + 1 ||
           cells[robot_loc_i][robot_loc_j + 1].getIsWall();
    break;
  case Direction::South:
    return robot_loc_i == house_size_rows + 1 ||
           cells[robot_loc_i + 1][robot_loc_j].getIsWall();
    break;
  case Direction::West:
    return robot_loc_j == 0 || cells[robot_loc_i][robot_loc_j - 1].getIsWall();
    break;
  }
}

int MySimulator::howMuchDirtHere() const {
  return cells[robot_loc_i][robot_loc_j].getDirtLevel();
}

int MySimulator::getBatteryLeft() const {
  return std::floor(battery_current_size);
}
int MySimulator::getDirtLeft() const {
  int ret = 0;
  for (u_int32_t row = 0; row < house_size_rows; row++) {
    for (u_int32_t col = 0; col < house_size_cols; col++) {
      ret += (cells[row][col].getDirtLevel());
    }
  }
  return ret;
}

bool MySimulator::isDirtLeft() const {
  for (u_int32_t row = 0; row < house_size_rows; row++) {
    for (u_int32_t col = 0; col < house_size_cols; col++) {
      if (cells[row][col].getDirtLevel() > 0)
        return true;
    }
  }
  return false;
}

bool MySimulator::isInDocking() const {
  return robot_loc_i == docking_loc_i && robot_loc_j == docking_loc_j;
}

bool MySimulator::cleaningFinished() const {
  return isInDocking() && !isDirtLeft();
}

bool MySimulator::robotDied() const {
  return !isInDocking() && battery_current_size < 1;
}

bool MySimulator::end() const {
  return cleaningFinished() || robotDied() || stepsList.size() >= max_steps;
}

void MySimulator::updateRobotLocation(Step decision) {
  if (decision != Step::Stay && decision != Step::Finish) {
    robot_loc_i = locIByDirection(robot_loc_i, stepToDir(decision));
    robot_loc_j = locJByDirection(robot_loc_j, stepToDir(decision));
  }
}

void MySimulator::updateRobotBattery(Step decision) {
  if (isCharging(decision)) {
    battery_current_size += ((float)battery_max_size / 20);
  } else {
    battery_current_size -= 1;
  }
}

void MySimulator::updateHouseDirt(Step decision) {
  if (decision == Step::Stay) {
    cells[robot_loc_i][robot_loc_j].decreaseDirtLevel();
  }
}

inline bool MySimulator::isCharging(Step decision) const {
  return isInDocking() && decision == Step::Stay;
}

bool MySimulator::isBadStep(Step decision) const {
  if (!isStationaryStep(decision) && isThereWall(stepToDir(decision))) {
    return true;
  }
  if (battery_current_size < 1 && !isCharging(decision)) {
    return true;
  }
  return false;
}

bool MySimulator::changeState() {
  // gets one step from the robot and updates house accordingly.
  // returns whether the simulation finished.

  Step decision = robot->nextStep();

  if (decision == Step::Finish)
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

void MySimulator::setAlgorithm(AbstractAlgorithm &algo) {
  auto self = shared_from_this();
  robot = std::unique_ptr<AbstractAlgorithm>(&algo);
  auto batteryMeter = MyBatteryMeter(self);
  auto wallsSensor = MyWallsSensor(self);
  auto dirtSensor = MyDirtSensor(self);
  robot->setBatteryMeter(std::move(batteryMeter));
  robot->setWallsSensor(std::move(wallsSensor));
  robot->setDirtSensor(std::move(dirtSensor));
}

bool MySimulator::run() {
  // runs the whole simulation, stops when it ends or if an error occurred.
  // returns whether the cleaning finished successfully.

  while (!error && !changeState()) {
  }
  return !error;
}

bool MySimulator::createOutput(std::string input_file) const {
  // creates output file.
  // returns whether output creation finished successfully.

  try {
    std::filesystem::path input_file_path(input_file);

    std::filesystem::path output_file_path =
        addPrefixToFilePath(input_file_path, "output_");

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

    for (Step d : stepsList) {
      file << stepString(d) << std::endl;
    }
    file << "Number of steps: " << stepsList.size() << std::endl;
    file << "Dirt left: " << getDirtLeft() << std::endl;
    file << "Vaccum cleaner is " << (robotDied() ? "dead" : "alive")
         << std::endl;
    file << "Mission ended " << (cleaningFinished() ? "" : "un")
         << "successfuly" << std::endl;

    file.close();
    return true;

  } catch (const std::ofstream::failure &e) {
    std::cerr << "Exception writing to output file: " << e.what() << std::endl;
    return false;
  } catch (const std::exception &e) {
    std::cerr << "Exception: " << e.what() << std::endl;
    return false;
  }
}
/*
void MySimulator::updateVisualization(Step decision) {
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
    outFile << stepString(decision) << std::endl;
    outFile << std::endl; // Empty line

    // Convert cells to ASCII table
    size_t rows = cells.size();
    size_t cols = rows > 0 ? cells[0].size() : 0;

    for (size_t i = 0; i < rows; ++i) {
      // Top walls
      for (size_t j = 0; j < cols; ++j) {
        if (cells[i][j].isWallInDirection(Direction::North)) {
          outFile << "+---";
        } else {
          outFile << "+   ";
        }
      }
      outFile << "+" << std::endl;

      // Side walls and dirt level
      for (size_t j = 0; j < cols; ++j) {
        if (cells[i][j]->isWallInDirection(Direction::West)) {
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
      if (cells[rows - 1][j]->isWallInDirection(Direction::South)) {
        outFile << "+---";
      } else {
        outFile << "+   ";
      }
    }
    outFile << "+" << std::endl;
    outFile << std::endl; // Empty line
    outFile.close();
  } catch (const std::ofstream::failure &e) {
    std::cerr << "Exception writing to file: " << e.what() << std::endl;
  } catch (const std::exception &e) {
    std::cerr << "Exception: " << e.what() << std::endl;
  }
}
*/
std::filesystem::path
MySimulator::addPrefixToFilePath(const std::filesystem::path &file_path,
                                 const std::string &prefix) const {
  // Extract the parent path (directory)
  std::filesystem::path parent_path = file_path.parent_path();

  // Extract the filename and add the prefix
  std::string new_filename = prefix + file_path.filename().string();

  // Construct the new file path
  std::filesystem::path new_file_path = parent_path / new_filename;

  return new_file_path;
}
