#include "AbstractAlgorithm.h"
#include <iostream>

VaccumGraphCell::VaccumGraphCell() {}
bool VaccumGraphCell::getIsDocking() const { return is_docking; }
bool VaccumGraphCell::getWasVisited() const { return was_visited; }
bool VaccumGraphCell::getIsDirty() const { return dirt != 0; }
void VaccumGraphCell::setDocking() { is_docking = true; }
void VaccumGraphCell::setVisited() { was_visited = true; }
void VaccumGraphCell::addNeighbor(Direction dir, VaccumGraphCell *neighbor) {
  neighbors->at(dir) = neighbor;
  if (neighbor != nullptr)
    neighbor->neighbors->at(oppositeDirection(dir)) = this;
}
int VaccumGraphCell::getLocI() const { return loc_i; }
int VaccumGraphCell::getLocJ() const { return loc_j; }
u_int32_t VaccumGraphCell::getID() const { return id; }
VaccumGraphCell *VaccumGraphCell::getNeighborInDirection(Direction dir) {
  return neighbors->at(dir);
}

void VaccumGraphCell::decreaseDirt() {
  if (dirt > 0)
    dirt--;
}

void VaccumGraphCell::setDirt(size_t dirt) { this->dirt = dirt; }

auto cellIsDockingCondition = [](const VaccumGraphCell cell) {
  return cell.getIsDocking();
};
auto cellWasntVisitedCondition = [](const VaccumGraphCell cell) {
  return !(cell.getWasVisited());
};
auto cellIsDirtyCondition = [](const VaccumGraphCell cell) {
  return cell.getIsDirty();
};

AlgorithmGraph::AlgorithmGraph() {
  cells[0][0];
  cells[0][0].setDocking();
}

void AlgorithmGraph::addCell(int loc_i, int loc_j) {
  cells[loc_i][loc_j];
  num_cells++;
}

void AlgorithmGraph::visit(size_t dirt, bool wallInNorth, bool wallInEast,
                           bool wallInSouth, bool wallInWest) {
  cells[curr_i][curr_j].setDirt(dirt);
  if (cells[curr_i][curr_j].getWasVisited())
    return;

  bool isThereWall[4] = {wallInNorth, wallInEast, wallInSouth, wallInWest};

  // for each direction, if wall doesnt exist:
  //  if cell was previously discovered there, add as neighbor.
  //  else, create cell and add as neighbor.
  for (int i = 0; i < 4; i++) {

    Direction dir = static_cast<Direction>(i);
    VaccumGraphCell *cellInDirection = nullptr;

    if (!isThereWall[i]) {
      cells[locIByDirection(curr_i, dir)][locJByDirection(curr_j, dir)];
    }
  }

  cells[curr_i][curr_j].setVisited();
}

std::pair<int, Direction> AlgorithmGraph::bfs(
    const std::function<bool(const VaccumGraphCell &)> &condition) const {
  std::pair<int, Direction> ret;
  ret.first = -1;
  ret.second = Direction::North;
  if (condition((cells.at(curr_i).at(curr_j)))) {
    ret.first = 0;
    return ret;
  }
  std::vector<bool> visited(num_cells);
  std::vector<int> distances(num_cells);
  std::vector<Direction> parent(num_cells);

  std::fill(visited.begin(), visited.end(), false);
  std::fill(distances.begin(), distances.end(), -1);
  std::fill(parent.begin(), parent.end(), Direction::North);

  visited[cells.at(curr_i).at(curr_j).getID()] = true;
  distances[cells.at(curr_i).at(curr_j).getID()] = 0;

  std::queue<std::pair<int, int>> q;
  q.emplace(curr_i, curr_j);

  while (!(q.empty())) {
    std::pair<int, int> curr_coordinates = q.front();
    int ptr_i = q.front().first;
    int ptr_j = q.front().second;
    int ptr_id = cells.at(ptr_i).at(ptr_j).getID();
    q.pop();
    if (condition(cells.at(ptr_i).at(ptr_j))) {
      ret.first = distances[cells.at(ptr_i).at(ptr_j).getID()];
      // backtrack to current
      while (curr_i != ptr_i && curr_j != ptr_j) {
        Direction dir_to_parent = parent[cells.at(ptr_i).at(ptr_j).getID()];
        int new_i = locIByDirection(ptr_i, dir_to_parent);
        int new_j = locJByDirection(ptr_j, dir_to_parent);
        if (new_i == curr_i && new_j == curr_j) {
          ret.second = oppositeDirection(dir_to_parent);
          break;
        }
        ptr_i = new_i;
        ptr_j = new_j;
      }
      return ret;
    }

    // handle NORTH neighbor
    if (cells.find(ptr_i - 1) != cells.end() &&
        cells.at(ptr_i - 1).find(ptr_j) != cells.at(ptr_i - 1).end()) {
      std::pair<int, int> north_coordinates =
          std::pair<int, int>(ptr_i - 1, ptr_j);
      int north_cell_id = cells.at(north_coordinates.first)
                              .at(north_coordinates.second)
                              .getID();
      visited[north_cell_id] = true;
      distances[north_cell_id] = distances[ptr_id] + 1;
      parent[north_cell_id] = Direction::South;
      q.push(north_coordinates);
    }

    // handle East neighbor
    if (cells.find(ptr_i) != cells.end() &&
        cells.at(ptr_i).find(ptr_j + 1) != cells.at(ptr_i).end()) {
      std::pair<int, int> east_coordinates =
          std::pair<int, int>(ptr_i, ptr_j + 1);
      int east_cell_id =
          cells.at(east_coordinates.first).at(east_coordinates.second).getID();
      visited[east_cell_id] = true;
      distances[east_cell_id] = distances[ptr_id] + 1;
      parent[east_cell_id] = Direction::West;
      q.push(east_coordinates);
    }

    // handle South neighbor
    if (cells.find(ptr_i + 1) != cells.end() &&
        cells.at(ptr_i + 1).find(ptr_j) != cells.at(ptr_i + 1).end()) {
      std::pair<int, int> south_coordinates =
          std::pair<int, int>(ptr_i + 1, ptr_j);
      int south_cell_id = cells.at(south_coordinates.first)
                              .at(south_coordinates.second)
                              .getID();
      visited[south_cell_id] = true;
      distances[south_cell_id] = distances[ptr_id] + 1;
      parent[south_cell_id] = Direction::North;
      q.push(south_coordinates);
    }

    // handle West neighbor
    if (cells.find(ptr_i) != cells.end() &&
        cells.at(ptr_i).find(ptr_j - 1) != cells.at(ptr_i).end()) {
      std::pair<int, int> west_coordinates =
          std::pair<int, int>(ptr_i, ptr_j - 1);
      int west_cell_id =
          cells.at(west_coordinates.first).at(west_coordinates.second).getID();
      visited[west_cell_id] = true;
      distances[west_cell_id] = distances[ptr_id] + 1;
      parent[west_cell_id] = Direction::East;
      q.push(west_coordinates);
    }
  }

  return ret;
}

std::pair<int, Direction> AlgorithmGraph::dockingDir() {
  return bfs(cellIsDockingCondition);
}

std::pair<int, Direction> AlgorithmGraph::unvisitedDir() {
  std::pair<int, Direction> ret = bfs(cellWasntVisitedCondition);
  if (ret.first == -1)
    finished_scanning = true;
  return ret;
}

std::pair<int, Direction> AlgorithmGraph::dirtyDir() {
  std::pair<int, Direction> ret = bfs(cellIsDirtyCondition);
  if (ret.first == -1)
    finished_cleaning = true;
  return ret;
}

bool AlgorithmGraph::isInDocking() const {
  return cells.at(curr_i).at(curr_j).getIsDocking();
}

bool AlgorithmGraph::houseWasFullyExplored() const { return finished_scanning; }

bool AlgorithmGraph::houseWasFullyCleaned() const { return finished_cleaning; }

void AlgorithmGraph::decreaseDirt() {
  cells.at(curr_i).at(curr_j).decreaseDirt();
}

void AlgorithmGraph::updateCurrent(Step dir) {
  if (dir != Step::Stay && dir != Step::Finish) {
    curr_i = locIByDirection(curr_i, stepToDir(dir));
    curr_j = locJByDirection(curr_j, stepToDir(dir));
  } else if (dir == Step::Stay)
    decreaseDirt();
}
