#include "VaccumGraph.h"
#include <iostream>

VaccumGraphCell::VaccumGraphCell(int loc_i, int loc_j)
    : loc_i(loc_i), loc_j(loc_j) {
  for (int i = 0; i < 4; i++)
    neighbors[i] = nullptr;
}
bool VaccumGraphCell::getIsDocking() const { return is_docking; }
bool VaccumGraphCell::getWasVisited() const { return was_visited; }
bool VaccumGraphCell::getIsDirty() const { return dirt != 0; }
void VaccumGraphCell::setDocking() { is_docking = true; }
void VaccumGraphCell::setVisited() { was_visited = true; }
void VaccumGraphCell::addNeighbor(Direction dir, VaccumGraphCell *neighbor) {
  neighbors[dir] = neighbor;
  if (neighbor != nullptr)
    neighbor->neighbors[oppositeDirection(dir)] = this;
}
int VaccumGraphCell::getLocI() const { return loc_i; }
int VaccumGraphCell::getLocJ() const { return loc_j; }
u_int32_t VaccumGraphCell::getID() const { return id; }
VaccumGraphCell *VaccumGraphCell::getNeighborInDirection(Direction dir) {
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

void VaccumGraph::visit(const Sensor *sensor) {
  current->setDirt(sensor->howMuchDirtHere());
  if (current->getWasVisited())
    return;
  int locI = current->getLocI();
  int locJ = current->getLocJ();

  // for each direction, if wall doesnt exist:
  //  if cell was previously discovered there, add as neighbor.
  //  else, create cell and add as neighbor.
  for (int i = 0; i < 4; i++) {

    Direction dir = (Direction)i;
    VaccumGraphCell *cellInDirection = nullptr;

    if (!sensor->isThereWall(dir)) {
      u_int32_t locIInDir = locIByDirection(locI, dir);
      u_int32_t locJInDir = locJByDirection(locJ, dir);

      cellInDirection = getCellInCoordinates(locIInDir, locJInDir);

      if (cellInDirection == nullptr) {
        cellInDirection = new VaccumGraphCell(locIInDir, locJInDir);
        addCell(cellInDirection);
      }
    }
    current->addNeighbor(dir, cellInDirection);
  }

  current->setVisited();
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
  std::vector<Direction> parent(num_cells);

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

Direction VaccumGraph::bfsDirection(
    const std::function<bool(const VaccumGraphCell *)> &condition) {
  if (condition(current)) {
    return STAY;
  }
  std::vector<bool> visited(num_cells);
  std::vector<int> distances(num_cells);
  std::vector<Direction> parent(num_cells);

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

Direction VaccumGraph::dirForDocking() {
  return bfsDirection(cellIsDockingCondition);
}

Direction VaccumGraph::dirForUnvisited() {
  Direction ret = bfsDirection(cellWasntVisitedCondition);

  if (ret == NOT_EXISTS)
    finished_scanning = true;
  return ret;
}

Direction VaccumGraph::dirForDirty() {
  Direction ret = bfsDirection(cellIsDirtyCondition);
  if (ret == NOT_EXISTS)
    finished_cleaning = true;
  return ret;
}

bool VaccumGraph::isInDocking() { return current->getIsDocking(); }

bool VaccumGraph::houseWasFullyExplored() const { return finished_scanning; }

bool VaccumGraph::houseWasFullyCleaned() const { return finished_cleaning; }

void VaccumGraph::decreaseDirt() { current->decreaseDirt(); }

void VaccumGraph::updateCurrent(Direction dir) {
  if (dir < 4)
    this->current = this->current->getNeighborInDirection(dir);
  if (dir == STAY)
    decreaseDirt();
}
