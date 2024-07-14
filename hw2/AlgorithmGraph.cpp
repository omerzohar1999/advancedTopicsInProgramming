#include "AlgorithmGraph.h"
#include "abstract_algorithm.h"
#include "enums_utils.h"
#include <iostream>

VaccumGraphCell::VaccumGraphCell() {}
bool VaccumGraphCell::getIsDocking() const { return is_docking; }
bool VaccumGraphCell::getWasVisited() const { return was_visited; }
bool VaccumGraphCell::getIsDirty() const { return dirt != 0; }
void VaccumGraphCell::setDocking() { is_docking = true; }
void VaccumGraphCell::setVisited() { was_visited = true; }
u_int32_t VaccumGraphCell::getID() const { return id; }

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

bool AlgorithmGraph::cellExists(int loc_i, int loc_j) const {
  return cells.find(loc_i) != cells.end() &&
         cells.at(loc_i).find(loc_j) != cells.at(loc_i).end();
}

AlgorithmGraph::AlgorithmGraph() {
  addCell(0, 0);
  cells.at(0).at(0).setDocking();
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
    int neighbor_i = locIByDirection(curr_i, dir);
    int neighbor_j = locJByDirection(curr_j, dir);

    if (!isThereWall[i] && !cellExists(neighbor_i, neighbor_j)) {
      addCell(neighbor_i, neighbor_j);
      std::cout << "added cell " << cells.at(neighbor_i).at(neighbor_j).getID()
                << " in (" << neighbor_i << "," << neighbor_j << ")"
                << std::endl;
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
  std::cout << "we know about " << num_cells << " cells" << std::endl;
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
    int ptr_i = q.front().first;
    int ptr_j = q.front().second;
    int ptr_id = cells.at(ptr_i).at(ptr_j).getID();
    q.pop();
    if (condition(cells.at(ptr_i).at(ptr_j))) {
      ret.first = distances[cells.at(ptr_i).at(ptr_j).getID()];
      // backtrack to current
      while (curr_i != ptr_i || curr_j != ptr_j) {
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
    std::pair<int, int> north_coordinates =
        std::pair<int, int>(ptr_i - 1, ptr_j);
    if (cellExists(north_coordinates.first, north_coordinates.second)) {
      int north_cell_id = cells.at(north_coordinates.first)
                              .at(north_coordinates.second)
                              .getID();
      if (!visited[north_cell_id]) {
        visited[north_cell_id] = true;
        distances[north_cell_id] = distances[ptr_id] + 1;
        parent[north_cell_id] = Direction::South;
        q.push(north_coordinates);
      }
    }

    // handle East neighbor
    std::pair<int, int> east_coordinates =
        std::pair<int, int>(ptr_i, ptr_j + 1);
    if (cellExists(east_coordinates.first, east_coordinates.second)) {
      int east_cell_id =
          cells.at(east_coordinates.first).at(east_coordinates.second).getID();
      if (!visited[east_cell_id]) {
        visited[east_cell_id] = true;
        distances[east_cell_id] = distances[ptr_id] + 1;
        parent[east_cell_id] = Direction::West;
        q.push(east_coordinates);
      }
    }

    // handle South neighbor
    std::pair<int, int> south_coordinates =
        std::pair<int, int>(ptr_i + 1, ptr_j);
    if (cellExists(south_coordinates.first, south_coordinates.second)) {
      int south_cell_id = cells.at(south_coordinates.first)
                              .at(south_coordinates.second)
                              .getID();
      if (!visited[south_cell_id]) {
        visited[south_cell_id] = true;
        distances[south_cell_id] = distances[ptr_id] + 1;
        parent[south_cell_id] = Direction::North;
        q.push(south_coordinates);
      }
    }

    // handle West neighbor
    std::pair<int, int> west_coordinates =
        std::pair<int, int>(ptr_i, ptr_j - 1);
    if (cellExists(west_coordinates.first, west_coordinates.second)) {
      int west_cell_id =
          cells.at(west_coordinates.first).at(west_coordinates.second).getID();
      if (!visited[west_cell_id]) {
        visited[west_cell_id] = true;
        distances[west_cell_id] = distances[ptr_id] + 1;
        parent[west_cell_id] = Direction::East;
        q.push(west_coordinates);
      }
    }
  }

  return ret;
}

std::pair<int, Direction> AlgorithmGraph::dockingDir() {
  return bfs(cellIsDockingCondition);
}

std::pair<int, Direction> AlgorithmGraph::unvisitedDir() {
  std::pair<int, Direction> ret = bfs(cellWasntVisitedCondition);
  if (ret.first == -1) {
    finished_scanning = true;
    std::cout << "finished scanning house" << std::endl;
  }
  return ret;
}

std::pair<int, Direction> AlgorithmGraph::dirtyDir() {
  std::pair<int, Direction> ret = bfs(cellIsDirtyCondition);
  if (finished_scanning && ret.first == -1)
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
  if (!isStationaryStep(dir)) {
    curr_i = locIByDirection(curr_i, stepToDir(dir));
    curr_j = locJByDirection(curr_j, stepToDir(dir));
  } else if (dir == Step::Stay)
    decreaseDirt();
}
