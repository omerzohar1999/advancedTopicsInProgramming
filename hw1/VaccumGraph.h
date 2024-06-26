#ifndef VACUUMGRAPH_H
#define VACUUMGRAPH_H

#include "Direction.h"
#include <functional>
#include <queue>
#include <vector>

static u_int32_t vaccumGraphCellID = 0;

class VaccumGraphCell {
  VaccumGraphCell *neighbors[4];
  int loc_i;
  int loc_j;
  bool wasVisited = false;
  bool isDocking = false;
  u_int8_t dirt = 0;
  u_int32_t id = vaccumGraphCellID++;

public:
  VaccumGraphCell(int loc_i, int loc_j);
  bool getIsDocking() const;
  bool getWasVisited() const;
  bool getIsDirty() const;
  void setDocking();
  void setVisited(u_int8_t dirt);
  void addNeighbor(direction dir, VaccumGraphCell *neighbor);
  int getLocI() const;
  int getLocJ() const;
  u_int32_t getID() const;
  VaccumGraphCell *getNeighborInDirection(direction dir);
  void decreaseDirt();
};

class VaccumGraph {
  std::vector<VaccumGraphCell *> cells;
  VaccumGraphCell *current;
  bool finishedScanning = false;
  bool finishedCleaning = false;

public:
  VaccumGraph();

  void visit(u_int8_t dirt, bool wallNorth, bool wallEast, bool wallSouth,
             bool wallWest);
  VaccumGraphCell *getCellInCoordinates(int loc_i, int loc_j);
  int bfsDistance(
      const std::function<bool(const VaccumGraphCell *)> &condition);
  direction
  bfsDirection(const std::function<bool(const VaccumGraphCell *)> &condition);
  int distanceFromDocking();

  direction dirForDocking();

  int distanceFromUnvisited();

  direction dirForUnvisited();
  int distanceFromDirty();

  direction dirForDirty();

  bool isInDocking();

  bool houseWasFullyExplored() const;

  bool houseWasFullyCleaned() const;

  void decreaseDirt();

  void updateCurrent(direction dir);

  int getCurrentI() const;
  int getCurrentJ() const;
};
#endif