#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdio.h>
#include <string>
#include <vector>

#include "DevTools.h"
#include "MySensors.h"
#include "MySimulator.h"
#include "abstract_algorithm.h"
#include "enums.h"
#include "enums_utils.h"
#include <exception>

HouseCell::HouseCell() {};

uint32_t HouseCell::getDirtLevel() const { return (uint32_t)dirt_level; }
void HouseCell::setDirtLevel(u_int8_t dirt_level) {
    this->dirt_level = dirt_level;
}
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
            error = true;
            return;
        }
        std::getline(ss, item);
        item.erase(std::remove(item.begin(), item.end(), ' '), item.end());
        try {
            this->max_steps = std::stoi(item);
        } catch (int errnum) {
            error = true;
            return;
        }
        std::getline(file, line);
        ss = std::stringstream(line);

        std::getline(ss, item, '=');
        item.erase(std::remove(item.begin(), item.end(), ' '), item.end());
        if (item != "MaxBattery") {
            error = true;
            return;
        }

        std::getline(ss, item, '=');
        item.erase(std::remove(item.begin(), item.end(), ' '), item.end());
        try {
            battery_max_size = std::stoi(item);
        } catch (int errnum) {
            error = true;
            return;
        }
        battery_current_size = battery_max_size;

        std::getline(file, line);
        ss = std::stringstream(line);

        std::getline(ss, item, '=');
        item.erase(std::remove(item.begin(), item.end(), ' '), item.end());
        if (item != "Rows") {
            error = true;
            return;
        }

        std::getline(ss, item, '=');
        item.erase(std::remove(item.begin(), item.end(), ' '), item.end());
        try {
            house_size_rows = std::stoi(item);
        } catch (int errnum) {
            error = true;
            return;
        }
        std::getline(file, line);
        ss = std::stringstream(line);

        std::getline(ss, item, '=');
        item.erase(std::remove(item.begin(), item.end(), ' '), item.end());
        if (item != "Cols") {
            error = true;
            return;
        }

        std::getline(ss, item, '=');
        item.erase(std::remove(item.begin(), item.end(), ' '), item.end());
        try {
            house_size_cols = std::stoi(item);
        } catch (int errnum) {
            error = true;
            return;
        }

        std::cout << "house size is (" << house_size_rows << "," << house_size_cols
                  << ")" << std::endl;

        cells.resize(house_size_rows + 2);
        std::cout << "including outer walls, numOfRows:" << cells.size()
                  << std::endl;
        for (size_t i = 0; i < cells.size(); i++) {
            cells[i].resize(house_size_cols + 2);
            cells[i][0].setWall();
            cells[i][cells[i].size() - 1].setWall();
            if (i == 0 || i == cells.size() - 1) {
                for (size_t j = 1; j < cells[i].size() - 1; j++) {
                    cells[i][j].setWall();
                }
            }
        }
        int ptr_i = 1;

        while (ptr_i <= house_size_rows && std::getline(file, line)) {
            int ptr_j = 1;
            while (ptr_j <= house_size_cols && ptr_j <= (int)line.size()) {
                // switch by value of cell
                switch (line[ptr_j - 1]) {
                    case '1':
                        cells[ptr_i][ptr_j].setDirtLevel(1);
                        break;
                    case '2':
                        cells[ptr_i][ptr_j].setDirtLevel(2);
                        break;
                    case '3':
                        cells[ptr_i][ptr_j].setDirtLevel(3);
                        break;
                    case '4':
                        cells[ptr_i][ptr_j].setDirtLevel(4);
                        break;
                    case '5':
                        cells[ptr_i][ptr_j].setDirtLevel(5);
                        break;
                    case '6':
                        cells[ptr_i][ptr_j].setDirtLevel(6);
                        break;
                    case '7':
                        cells[ptr_i][ptr_j].setDirtLevel(7);
                        break;
                    case '8':
                        cells[ptr_i][ptr_j].setDirtLevel(8);
                        break;
                    case '9':
                        cells[ptr_i][ptr_j].setDirtLevel(9);
                        break;
                    case 'D':
                        if (robot_loc_i != -1) {
                            error = true;
                            return;
                        }
                        robot_loc_i = ptr_i;
                        robot_loc_j = ptr_j;
                        docking_loc_i = ptr_i;
                        docking_loc_j = ptr_j;
                        break;
                    case 'W':
                        cells[ptr_i][ptr_j].setDirtLevel(0);
                        cells[ptr_i][ptr_j].setWall();
                        break;
                    default:
                        break;
                }
                ptr_j++;
            }
            ptr_i++;
        }
        if (robot_loc_i == -1) {
            error = true;
            return;
        }

        printHouse();

    } catch (const std::ifstream::failure &e) {
        std::cerr << "Exception opening/reading file: " << e.what() << std::endl;
        error = true;
    } catch (const std::exception &e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        error = true;
    }
}

bool MySimulator::isThereWall(Direction dir) const {
    std::cout << "checking if there's wall" << std::endl;
    std::cout << "robot location: (" << robot_loc_i << "," << robot_loc_j << ")"
              << std::endl;
    std::cout << "move direction: " << directionString(dir) << std::endl;
    switch (dir) {
        case Direction::North:
            return robot_loc_i <= 1 || cells[robot_loc_i - 1][robot_loc_j].getIsWall();
            break;
        case Direction::East:
            return robot_loc_j >= house_size_cols ||
                   cells[robot_loc_i][robot_loc_j + 1].getIsWall();
            break;
        case Direction::South:
            return robot_loc_i >= house_size_rows ||
                   cells[robot_loc_i + 1][robot_loc_j].getIsWall();
            break;
        case Direction::West:
            return robot_loc_j <= 1 || cells[robot_loc_i][robot_loc_j - 1].getIsWall();
            break;
        default:
            return true;
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
    for (size_t row = 0; row < cells.size(); row++) {
        for (size_t col = 0; col < cells[row].size(); col++) {
            ret += (cells[row][col].getDirtLevel());
        }
    }
    return ret;
}

bool MySimulator::isDirtLeft() const {
    for (size_t row = 0; row < cells.size(); row++) {
        for (size_t col = 0; col < cells[row].size(); col++) {
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

std::string MySimulator::statusString() const {
    if (stepsList[stepsList.size() - 1] == Step::Finish)
        return "FINISHED";
    if (robotDied())
        return "DEAD";
    return "WORKING";
}

bool MySimulator::end() const {
    return robotDied() || stepsList.size() >= (size_t)max_steps;
}

void MySimulator::updateRobotLocation(Step decision) {
    if (!isStationaryStep(decision)) {
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
        std::cout << "step is into a wall" << std::endl;
        return true;
    }
    std::cout << "step is not into a wall" << std::endl;
    if (battery_current_size < 1 && !isCharging(decision)) {
        return true;
    }
    return false;
}

bool MySimulator::changeState() {
    // gets one step from the robot and updates house accordingly.
    // returns whether the simulation finished.

    Step decision = robot->nextStep();

    std::cout << "got step " << stepString(decision) << std::endl;

    stepsList.push_back(decision);

    if (decision == Step::Finish) {
        // in the case Finish was returned,
        // we want to validate that the cleaning finished successfully.
        if (!cleaningFinished())
            error = true;
        return true;
    }

    std::cout << "step was not finish" << std::endl;

    if (isBadStep(decision)) {
        error = true;
        return true;
    }

    std::cout << "step was not bad" << std::endl;

    updateRobotLocation(decision);
    updateRobotBattery(decision);
    updateHouseDirt(decision);

    printHouse();

    updateVisualization(decision);

    return end();
}

void MySimulator::setAlgorithm(AbstractAlgorithm &algo) {
    robot = &algo;
    robot->setMaxSteps(max_steps);

    batteryMeter.setHouse(weak_from_this());
    wallsSensor.setHouse(weak_from_this());
    dirtSensor.setHouse(weak_from_this());

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

        size_t steps = stepsList.size();
        if (steps > 0 && stepsList[steps - 1] == Step::Finish) {
            steps--;
        }

        file << "NumSteps = " << steps << std::endl;
        file << "DirtLeft = " << getDirtLeft() << std::endl;
        file << "Status = " << statusString() << std::endl;
        file << "Steps:" << std::endl;
        for (Step d : stepsList) {
            file << stepStringOutput(d);
        }

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
void MySimulator::printHouse() {

    for (size_t i = 0; i < cells.size(); i++) {
        for (size_t j = 0; j < cells[i].size(); j++) {
            if (cells[i][j].getIsWall())
                std::cout << "W";
            else if (i == (size_t)robot_loc_i && j == (size_t)robot_loc_j)
                std::cout << "R";
            else if (i == (size_t)docking_loc_i && j == (size_t)docking_loc_j)
                std::cout << "D";
            else if (cells[i][j].getDirtLevel() > 0)
                std::cout << cells[i][j].getDirtLevel();
            else
                std::cout << " ";
        }
        std::cout << std::endl;
    }
}

void MySimulator::updateVisualization(Step decision) {
    if (!devTools.isVisualizationEnabled())
        return;

    try {
        std::string file_name = devTools.getVisualizationFileName();
        std::ofstream outFile(file_name, std::ios::app);
        if (!outFile) {
            std::cerr << "Error opening file for writing." << std::endl;
            return;
        }

        std::cout << "writing to file: " << file_name << std::endl;

        // Write the direction
        outFile << stepString(decision) << std::endl;
        outFile << std::endl; // Empty line

        for (size_t i = 0; i < cells.size(); i++) {
            for (size_t j = 0; j < cells[i].size(); j++) {
                if (cells[i][j].getIsWall())
                    outFile << "W";
                else if (i == (size_t)robot_loc_i && j == (size_t)robot_loc_j)
                    outFile << "R";
                else if (i == (size_t)docking_loc_i && j == (size_t)docking_loc_j)
                    outFile << "D";
                else if (cells[i][j].getDirtLevel() > 0)
                    outFile << cells[i][j].getDirtLevel();
                else
                    outFile << " ";
            }
            outFile << std::endl;
        }
        outFile << std::endl; // Empty line
        outFile.close();
    } catch (const std::ofstream::failure &e) {
        std::cerr << "Exception writing to file: " << e.what() << std::endl;
    } catch (const std::exception &e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
}

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