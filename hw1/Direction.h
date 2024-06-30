#ifndef DIRECTION_H
#define DIRECTION_H

#include <string>

enum Direction { NORTH, EAST, SOUTH, WEST, STAY, NOT_EXISTS };

inline Direction oppositeDirection(Direction dir) {
  switch (dir) {
  case NORTH:
    return SOUTH;
  case EAST:
    return WEST;
  case SOUTH:
    return NORTH;
  case WEST:
    return EAST;
  default:
    return dir;
  }
}

inline std::string directionString(Direction dir) {
  switch (dir) {
  case NORTH:
    return "North";
  case EAST:
    return "East";
  case SOUTH:
    return "South";
  case WEST:
    return "West";
  case STAY:
    return "Stay";
  default:
    return "";
  }
}

inline u_int32_t locIByDirection(u_int32_t i, Direction dir) {
  switch (dir) {
  case NORTH:
    return i - 1;
  case SOUTH:
    return i + 1;
  default:
    return i;
  }
}

inline u_int32_t locJByDirection(u_int32_t j, Direction dir) {
  switch (dir) {
  case EAST:
    return j + 1;
  case WEST:
    return j - 1;
  default:
    return j;
  }
}

#endif