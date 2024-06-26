#ifndef DIRECTION_H
#define DIRECTION_H

#include <string>

#define DEBUG

enum direction { NORTH, EAST, SOUTH, WEST, STAY, NOT_EXISTS };

inline direction oppositeDirection(direction dir) {
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

inline std::string directionString(direction dir) {
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

#endif