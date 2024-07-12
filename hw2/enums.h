#ifndef ROBOT_ENUMS_H_
#define ROBOT_ENUMS_H_

#include <string>

enum class Direction { North, East, South, West };
enum class Step { North, East, South, West, Stay, Finish };

inline bool isStationaryStep(Step s) {
  return s == Step::Stay || s == Step::Finish;
}

inline Direction stepToDir(Step step) {
  switch (step) {
  case Step::North:
    return Direction::North;
    break;
  case Step::East:
    return Direction::East;
    break;
  case Step::South:
    return Direction::South;
    break;
  case Step::West:
    return Direction::West;
    break;
  default:
    throw; // TODO
  }
}

inline Step dirToStep(Direction dir) {
  switch (dir) {
  case Direction::North:
    return Step::North;
    break;
  case Direction::East:
    return Step::East;
    break;
  case Direction::South:
    return Step::South;
    break;
  case Direction::West:
    return Step::West;
    break;
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

#endif // ROBOT_ENUMS_H_
