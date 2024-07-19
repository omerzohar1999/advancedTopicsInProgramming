#ifndef ROBOT_ENUMS_UTILS_H_
#define ROBOT_ENUMS_UTILS_H_

#include "enums.h"
#include <string>

inline bool isStationaryStep(Step s) {
    return s == Step::Stay || s == Step::Finish;
}

inline Direction stepToDir(Step step) {
    // precondition: step is not stationary
    switch (step) {
        case Step::North:
            return Direction::North;
        case Step::East:
            return Direction::East;
        case Step::South:
            return Direction::South;
        case Step::West:
            return Direction::West;
        default:
            return Direction::North; // should never get here
    }
}

inline Step dirToStep(Direction dir) {
    switch (dir) {
        case Direction::North:
            return Step::North;
        case Direction::East:
            return Step::East;
        case Direction::South:
            return Step::South;
        case Direction::West:
            return Step::West;
        default:
            return Step::Finish;
    }
}

inline Step oppositeDirection(Step dir) {
    switch (dir) {
        case Step::North:
            return Step::South;
        case Step::East:
            return Step::West;
        case Step::South:
            return Step::North;
        case Step::West:
            return Step::East;
        default:
            return dir;
    }
}

inline Direction oppositeDirection(Direction dir) {
    switch (dir) {
        case Direction::North:
            return Direction::South;
        case Direction::East:
            return Direction::West;
        case Direction::South:
            return Direction::North;
        case Direction::West:
            return Direction::East;
        default:
            return dir;
    }
}

inline std::string stepString(Step dir) {
    switch (dir) {
        case Step::North:
            return "North";
        case Step::East:
            return "East";
        case Step::South:
            return "South";
        case Step::West:
            return "West";
        case Step::Stay:
            return "Stay";
        case Step::Finish:
            return "Finish";
        default:
            return "";
    }
}

inline std::string stepStringOutput(Step dir) {
    switch (dir) {
        case Step::North:
            return "N";
        case Step::East:
            return "E";
        case Step::South:
            return "S";
        case Step::West:
            return "W";
        case Step::Stay:
            return "s";
        case Step::Finish:
            return "F";
        default:
            return "";
    }
}

inline std::string directionString(Direction dir) {
    switch (dir) {
        case Direction::North:
            return "North";
        case Direction::East:
            return "East";
        case Direction::South:
            return "South";
        case Direction::West:
            return "West";
        default:
            return "";
    }
}

inline u_int32_t locIByDirection(u_int32_t i, Direction dir) {
    switch (dir) {
        case Direction::North:
            return i - 1;
        case Direction::South:
            return i + 1;
        default:
            return i;
    }
}

inline u_int32_t locJByDirection(u_int32_t j, Direction dir) {
    switch (dir) {
        case Direction::East:
            return j + 1;
        case Direction::West:
            return j - 1;
        default:
            return j;
    }
}

#endif // ROBOT_ENUMS_UTILS_H_