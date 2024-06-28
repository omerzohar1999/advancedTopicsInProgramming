#ifndef DIRECTION_H
#define DIRECTION_H

#include <string>

enum Direction { NORTH, EAST, SOUTH, WEST, STAY, NOT_EXISTS };

inline Direction oppositeDirection(Direction dir) {
  switch (dir) {
  case NORTH:
    return SOUTH;
    break;
  case EAST:
    return WEST;
    break;
  case SOUTH:
    return NORTH;
    break;
  case WEST:
    return EAST;
    break;
  default:
    return dir;
    break;
  }
}

inline std::string directionString(Direction dir) {
  switch (dir) {
  case NORTH:
    return "North";
    break;
  case EAST:
    return "East";
    break;
  case SOUTH:
    return "South";
    break;
  case WEST:
    return "West";
    break;
  case STAY:
    return "Stay";
    break;
  default:
    return "";
    break;
  }
  return "";
}

inline u_int32_t locIByDirection(u_int32_t i, Direction dir) {
  switch (dir) {
  case NORTH:
    return i - 1;
    break;
  case SOUTH:
    return i + 1;
    break;
  default:
    return i;
    break;
  }
}

inline u_int32_t locJByDirection(u_int32_t j, Direction dir) {
  switch (dir) {
  case EAST:
    return j + 1;
    break;
  case WEST:
    return j - 1;
    break;
  default:
    return j;
    break;
  }
}

#endif