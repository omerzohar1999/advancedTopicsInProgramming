#include "VaccumGraph.h"
#include <iostream>

VaccumGraphCell::VaccumGraphCell(int loc_i, int loc_j)
    : loc_i(loc_i), loc_j(loc_j) {
  for (int i = 0; i < 4; i++)
    neighbors[i] = nullptr;
}
bool VaccumGraphCell::getIsDocking() const { return isDocking; }
bool VaccumGraphCell::getWasVisited() const { return wasVisited; }
bool VaccumGraphCell::getIsDirty() const { return dirt != 0; }
void VaccumGraphCell::setDocking() { isDocking = true; }
void VaccumGraphCell::setVisited(u_int8_t dirt) {
  wasVisited = true;
  this->dirt = dirt;
}
void VaccumGraphCell::addNeighbor(direction dir, VaccumGraphCell *neighbor) {
  neighbors[dir] = neighbor;
}
int VaccumGraphCell::getLocI() const { return loc_i; }
int VaccumGraphCell::getLocJ() const { return loc_j; }
u_int32_t VaccumGraphCell::getID() const { return id; }
VaccumGraphCell *VaccumGraphCell::getNeighborInDirection(direction dir) {
  if (dir < 4) {
    return neighbors[dir];
  }
  return nullptr;
}

void VaccumGraphCell::decreaseDirt() {
  if (dirt > 0)
    dirt--;
}

void VaccumGraphCell::setDirt(u_int8_t dirt) { this->dirt = dirt; }

auto cellIsDockingCondition = [](const VaccumGraphCell *cell) {
  return cell->getIsDocking();
};
auto cellWasntVisitedCondition = [](const VaccumGraphCell *cell) {
  return !(cell->getWasVisited());
};
auto cellIsDirtyCondition = [](const VaccumGraphCell *cell) {
  return cell->getIsDirty();
};

VaccumGraph::VaccumGraph() {
  current = new VaccumGraphCell(0, 0);
  current->setDocking();
  addCell(current);
}

void VaccumGraph::addCell(VaccumGraphCell *cell) {
  cells[cell->getLocI()][cell->getLocJ()] = cell;
  num_cells++;
}

void VaccumGraph::visit(u_int8_t dirt, bool wallNorth, bool wallEast,
                        bool wallSouth, bool wallWest) {
  current->setDirt(dirt);
  if (current->getWasVisited())
    return;
  int locI = current->getLocI();
  int locJ = current->getLocJ();

  if (!wallNorth) {
    VaccumGraphCell *cellInNorth = getCellInCoordinates(locI - 1, locJ);
    if (cellInNorth == nullptr) {
      cellInNorth = new VaccumGraphCell(locI - 1, locJ);
      addCell(cellInNorth);
    }
    current->addNeighbor(NORTH, cellInNorth);
    cellInNorth->addNeighbor(SOUTH, current);
  } else {
    current->addNeighbor(NORTH, nullptr);
  }

  if (!wallEast) {
    VaccumGraphCell *cellInEast = getCellInCoordinates(locI, locJ + 1);
    if (cellInEast == nullptr) {
      cellInEast = new VaccumGraphCell(locI, locJ + 1);
      addCell(cellInEast);
    }
    current->addNeighbor(EAST, cellInEast);
    cellInEast->addNeighbor(WEST, current);
  } else {
    current->addNeighbor(EAST, nullptr);
  }

  if (!wallSouth) {
    VaccumGraphCell *cellInSouth = getCellInCoordinates(locI + 1, locJ);
    if (cellInSouth == nullptr) {
      cellInSouth = new VaccumGraphCell(locI + 1, locJ);
      addCell(cellInSouth);
    }
    current->addNeighbor(SOUTH, cellInSouth);
    cellInSouth->addNeighbor(NORTH, current);
  } else {
    current->addNeighbor(SOUTH, nullptr);
  }

  if (!wallWest) {
    VaccumGraphCell *cellInWest = getCellInCoordinates(locI, locJ - 1);
    if (cellInWest == nullptr) {
      cellInWest = new VaccumGraphCell(locI, locJ - 1);
      addCell(cellInWest);
    }
    current->addNeighbor(WEST, cellInWest);
    cellInWest->addNeighbor(EAST, current);
  } else {
    current->addNeighbor(WEST, nullptr);
  }

  current->setVisited(dirt);
}

VaccumGraphCell *VaccumGraph::getCellInCoordinates(int loc_i, int loc_j) {
  return cells[loc_i][loc_j];
}

int VaccumGraph::bfsDistance(
    const std::function<bool(const VaccumGraphCell *)> &condition) {
  if (condition(current)) {
    return 0;
  }

  std::vector<bool> visited(num_cells);
  std::vector<int> distances(num_cells);
  std::vector<direction> parent(num_cells);

  std::fill(visited.begin(), visited.end(), false);
  std::fill(distances.begin(), distances.end(), -1);
  std::fill(parent.begin(), parent.end(), NOT_EXISTS);

  visited[current->getID()] = true;
  distances[current->getID()] = 0;
  parent[current->getID()] = STAY;

  std::queue<VaccumGraphCell *> q;
  q.push(current);

  VaccumGraphCell *v;
  VaccumGraphCell *u;
  while (!(q.empty())) {
    v = q.front();
    q.pop();
    if (condition(v)) {
      // return v!
      return distances[v->getID()];
    }

    // handle NORTH neighbor
    u = v->getNeighborInDirection(NORTH);
    if (u != nullptr && !visited[u->getID()]) {
      visited[u->getID()] = true;
      distances[u->getID()] = distances[v->getID()] + 1;
      parent[u->getID()] = SOUTH;
      q.push(u);
    }

    // handle EAST neighbor
    u = v->getNeighborInDirection(EAST);
    if (u != nullptr && !visited[u->getID()]) {
      visited[u->getID()] = true;
      distances[u->getID()] = distances[v->getID()] + 1;
      parent[u->getID()] = WEST;
      q.push(u);
    }

    // handle SOUTH neighbor
    u = v->getNeighborInDirection(SOUTH);
    if (u != nullptr && !visited[u->getID()]) {
      visited[u->getID()] = true;
      distances[u->getID()] = distances[v->getID()] + 1;
      parent[u->getID()] = NORTH;
      q.push(u);
    }

    // handle WEST neighbor
    u = v->getNeighborInDirection(WEST);
    if (u != nullptr && !visited[u->getID()]) {
      visited[u->getID()] = true;
      distances[u->getID()] = distances[v->getID()] + 1;
      parent[u->getID()] = EAST;
      q.push(u);
    }
  }

  return -1;
}

direction VaccumGraph::bfsDirection(
    const std::function<bool(const VaccumGraphCell *)> &condition) {
  if (condition(current)) {
    return STAY;
  }
  std::vector<bool> visited(num_cells);
  std::vector<int> distances(num_cells);
  std::vector<direction> parent(num_cells);

  std::fill(visited.begin(), visited.end(), false);
  std::fill(distances.begin(), distances.end(), -1);
  std::fill(parent.begin(), parent.end(), NOT_EXISTS);

  visited[current->getID()] = true;
  distances[current->getID()] = 0;
  parent[current->getID()] = STAY;

  std::queue<VaccumGraphCell *> q;
  q.push(current);

  VaccumGraphCell *v;
  VaccumGraphCell *u;
  while (!(q.empty())) {
    v = q.front();
    q.pop();
    if (condition(v)) {
      // backtrack to current
      VaccumGraphCell *p = v->getNeighborInDirection(parent[v->getID()]);
      while (p != current) {
        v = p;
        p = v->getNeighborInDirection(parent[v->getID()]);
      }
      return oppositeDirection(parent[v->getID()]);
    }

    // handle NORTH neighbor
    u = v->getNeighborInDirection(NORTH);
    if (u != nullptr) {
    }
    if (u != nullptr && !visited[u->getID()]) {
      visited[u->getID()] = true;
      distances[u->getID()] = distances[v->getID()] + 1;
      parent[u->getID()] = SOUTH;
      q.push(u);
    }

    // handle EAST neighbor
    u = v->getNeighborInDirection(EAST);
    if (u != nullptr && !visited[u->getID()]) {
      visited[u->getID()] = true;
      distances[u->getID()] = distances[v->getID()] + 1;
      parent[u->getID()] = WEST;
      q.push(u);
    }

    // handle SOUTH neighbor
    u = v->getNeighborInDirection(SOUTH);
    if (u != nullptr && !visited[u->getID()]) {
      visited[u->getID()] = true;
      distances[u->getID()] = distances[v->getID()] + 1;
      parent[u->getID()] = NORTH;
      q.push(u);
    }

    // handle WEST neighbor
    u = v->getNeighborInDirection(WEST);
    if (u != nullptr && !visited[u->getID()]) {
      visited[u->getID()] = true;
      distances[u->getID()] = distances[v->getID()] + 1;
      parent[u->getID()] = EAST;
      q.push(u);
    }
  }

  return NOT_EXISTS;
}

int VaccumGraph::distanceFromDocking() {
  return bfsDistance(cellIsDockingCondition);
}

direction VaccumGraph::dirForDocking() {
  return bfsDirection(cellIsDockingCondition);
}

int VaccumGraph::distanceFromUnvisited() {
  int ret = bfsDistance(cellWasntVisitedCondition);
  if (ret == -1)
    finishedScanning = true;
  return ret;
}

direction VaccumGraph::dirForUnvisited() {
  direction ret = bfsDirection(cellWasntVisitedCondition);

  if (ret == NOT_EXISTS)
    finishedScanning = true;
  return ret;
}

int VaccumGraph::distanceFromDirty() {
  int ret = bfsDistance(cellIsDirtyCondition);
  if (ret == -1)
    finishedCleaning = true;
  return ret;
}

direction VaccumGraph::dirForDirty() {
  direction ret = bfsDirection(cellIsDirtyCondition);
  if (ret == NOT_EXISTS)
    finishedCleaning = true;
  return ret;
}

bool VaccumGraph::isInDocking() { return current->getIsDocking(); }

bool VaccumGraph::houseWasFullyExplored() const { return finishedScanning; }

bool VaccumGraph::houseWasFullyCleaned() const { return finishedCleaning; }

void VaccumGraph::decreaseDirt() { current->decreaseDirt(); }

void VaccumGraph::updateCurrent(direction dir) {
  if (dir < 4)
    this->current = this->current->getNeighborInDirection(dir);
}

int VaccumGraph::getCurrentI() const { return current->getLocI(); }
int VaccumGraph::getCurrentJ() const { return current->getLocJ(); }
