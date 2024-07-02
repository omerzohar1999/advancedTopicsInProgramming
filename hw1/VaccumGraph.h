#ifndef VACUUMGRAPH_H
#define VACUUMGRAPH_H

#include "Direction.h"
#include "Sensor.h"
#include <functional>
#include <map>
#include <queue>
#include <vector>

static u_int32_t vaccumGraphCellID = 0;

class VaccumGraphCell {
  VaccumGraphCell *neighbors[4];
  int loc_i;
  int loc_j;
  bool was_visited = false;
  bool is_docking = false;
  u_int8_t dirt = 0;
  u_int32_t id = vaccumGraphCellID++;

public:
  VaccumGraphCell(int loc_i, int loc_j);
  bool getIsDocking() const;
  bool getWasVisited() const;
  bool getIsDirty() const;
  void setDocking();
  void setVisited();
  void addNeighbor(Direction dir, VaccumGraphCell *neighbor);
  int getLocI() const;
  int getLocJ() const;
  u_int32_t getID() const;
  VaccumGraphCell *getNeighborInDirection(Direction dir);
  void decreaseDirt();
  void setDirt(u_int8_t dirt);
};

class VaccumGraph {
  std::unordered_map<int, std::unordered_map<int, VaccumGraphCell *>> cells;
  VaccumGraphCell *current;
  bool finished_scanning = false;
  bool finished_cleaning = false;
  int num_cells = 0;

public:
  VaccumGraph();

  void visit(const Sensor *sensor);
  VaccumGraphCell *getCellInCoordinates(int loc_i, int loc_j);
  int bfsDistance(
      const std::function<bool(const VaccumGraphCell *)> &condition);
  Direction
  bfsDirection(const std::function<bool(const VaccumGraphCell *)> &condition);
  int distanceFromDocking();
  Direction dirForDocking();
  Direction dirForUnvisited();
  Direction dirForDirty();
  bool isInDocking();
  bool houseWasFullyExplored() const;
  bool houseWasFullyCleaned() const;
  void decreaseDirt();
  void updateCurrent(Direction dir);
  void addCell(VaccumGraphCell *cell);
  int distanceFromUnvisited();
  int distanceFromDirty();
};
#endif