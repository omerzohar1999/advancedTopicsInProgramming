#ifndef VACUUMGRAPH_H
#define VACUUMGRAPH_H

#include "Direction.h"
#include "Sensors.h"
#include <functional>
#include <map>
#include <queue>
#include <vector>

static u_int32_t vaccumGraphCellID = 0;

class VaccumGraphCell {
  std::map<Direction, VaccumGraphCell *> neighbors[4];
  int loc_i;
  int loc_j;
  bool was_visited = false;
  bool is_docking = false;
  size_t dirt = 0;
  u_int32_t id = vaccumGraphCellID++;

public:
  VaccumGraphCell();
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
  void setDirt(size_t dirt);
};

class AlgorithmGraph {
  std::unordered_map<int, std::unordered_map<int, VaccumGraphCell>> cells;
  bool finished_scanning = false;
  bool finished_cleaning = false;
  int curr_i = 0;
  int curr_j = 0;
  int num_cells = 0;

public:
  AlgorithmGraph();

  void visit(size_t dirt, bool wallInNorth, bool wallInEast, bool wallInSouth,
             bool wallInWest);
  VaccumGraphCell getCellInCoordinates(int loc_i, int loc_j);
  std::pair<int, Direction>
  bfs(const std::function<bool(const VaccumGraphCell &)> &condition) const;
  std::pair<int, Direction> dockingDir();
  std::pair<int, Direction> unvisitedDir();
  std::pair<int, Direction> dirtyDir();
  bool isInDocking() const;
  bool houseWasFullyExplored() const;
  bool houseWasFullyCleaned() const;
  void decreaseDirt();
  void updateCurrent(Step dir);
  void addCell(int loc_i, int loc_j);
};
#endif