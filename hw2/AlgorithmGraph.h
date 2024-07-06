#ifndef ALGOGRAPH_H
#define ALGOGRAPH_H

#include "Direction.h"
#include "Sensors.h"
#include <functional>
#include <map>
#include <queue>
#include <vector>

static u_int32_t vaccumGraphCellID = 0;

class VaccumGraphCell {
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
  u_int32_t getID() const;
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

  bool cellExists(int loc_i, int loc_j) const;

public:
  AlgorithmGraph();

  void visit(size_t dirt, bool wallInNorth, bool wallInEast, bool wallInSouth,
             bool wallInWest);
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