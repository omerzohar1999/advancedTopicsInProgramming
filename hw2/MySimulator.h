#ifndef MYSIM_H
#define MYSIM_H

#include "DevTools.h"
#include "MyAlgorithm.h"
#include "MySensors.h"
#include "Simulator.h"
#include "enums.h"
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

class MySimulator : public Simulator,
                    public std::enable_shared_from_this<MySimulator> {
    std::vector<std::vector<HouseCell>> cells;
    MyBatteryMeter batteryMeter;
    MyWallsSensor wallsSensor;
    MyDirtSensor dirtSensor;
    AbstractAlgorithm *robot;
    DevTools devTools;
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
                        const std::string &prefix) ;

public:
    MySimulator();

    bool isThereWall(Direction dir) const;

    int howMuchDirtHere() const;

    int getBatteryLeft() const;

    void setAlgorithm(AbstractAlgorithm &algo);

    bool run();

    bool createOutput(std::string input_file) const;

    bool readHouseFile(std::string file_name);
};

#endif