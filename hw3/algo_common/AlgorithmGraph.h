#ifndef ALGOGRAPH_H
#define ALGOGRAPH_H

#include "common/enums.h"
#include <functional>
#include <map>
#include <queue>
#include <vector>

class VaccumGraphCell {
  int distance_from_docking = -1;
  Direction direction_to_docking = Direction::North;
  bool was_visited = false;
  bool is_docking = false;
  int dirt = 0;
  int id = -1;

public:
  VaccumGraphCell();

  void setID(int id);

  bool getIsDocking() const;

  bool getWasVisited() const;

  bool getIsDirty() const;

  void setDocking();

  void setVisited();

  int getID() const;

  void decreaseDirt();

  void setDirt(int dirt);

  void setDistanceFromDocking(int distance);

  int getDistanceFromDocking() const;

  void setDirectionToDocking(Direction dir);

  Direction getDirectionToDocking() const;
};

class AlgorithmGraph {
  std::unordered_map<int, std::unordered_map<int, VaccumGraphCell>> cells;
  bool finished_scanning = false;
  bool finished_cleaning = false;
  int curr_i = 0;
  int curr_j = 0;
  size_t num_cells = 0;

  bool cellExists(int loc_i, int loc_j) const;

public:
  void updateDistancesFromDocking();
  bool finishedScanning() const;
  bool finishedCleaning() const;
  bool finishedJob() const;
  AlgorithmGraph();

  void visit(int dirt, bool wallInNorth, bool wallInEast, bool wallInSouth,
             bool wallInWest);

  std::pair<std::pair<int, Direction>, int>
  bfs(const std::function<bool(const VaccumGraphCell &)> &condition) const;

  std::pair<int, Direction> dockingDistAndDir();

  std::pair<std::pair<int, Direction>, int> unvisitedBfs();

  std::pair<std::pair<int, Direction>, int> dirtyBfs();

  bool isInDocking() const;

  bool houseWasFullyExplored() const;

  bool houseWasFullyCleaned() const;

  void decreaseDirt();

  void updateCurrent(Step dir);

  void addCell(int loc_i, int loc_j);
};

#endif