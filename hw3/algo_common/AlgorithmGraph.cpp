#include "AlgorithmGraph.h"
#include "common/AbstractAlgorithm.h"
#include "common/enums_utils.h"
#include <iostream>

bool VaccumGraphCell::getIsDocking() const { return is_docking; }

VaccumGraphCell::VaccumGraphCell() {}

void VaccumGraphCell::setID(int id) { this->id = id; }

bool VaccumGraphCell::getWasVisited() const { return was_visited; }

bool VaccumGraphCell::getIsDirty() const { return dirt != 0; }

void VaccumGraphCell::setDocking() { is_docking = true; }

void VaccumGraphCell::setVisited() { was_visited = true; }

int VaccumGraphCell::getID() const { return id; }

void VaccumGraphCell::decreaseDirt() {
  if (dirt > 0)
    dirt--;
}

void VaccumGraphCell::setDirt(int dirt) { this->dirt = dirt; }

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
  cells[loc_i][loc_j].setID(num_cells);
  num_cells++;
}

void AlgorithmGraph::visit(int dirt, bool wallInNorth, bool wallInEast,
                           bool wallInSouth, bool wallInWest) {
  Direction dir;

  cells[curr_i][curr_j].setDirt(dirt);
  if (cells[curr_i][curr_j].getWasVisited())
    return;

  cells[curr_i][curr_j].setVisited();

  bool isThereWall[4] = {wallInNorth, wallInEast, wallInSouth, wallInWest};

  // for each direction, if wall doesnt exist:
  //  if cell was previously discovered there, add as neighbor.
  //  else, create cell and add as neighbor.
  for (int i = 0; i < 4; i++) {

    dir = static_cast<Direction>(i);
    int neighbor_i = locIByDirection(curr_i, dir);
    int neighbor_j = locJByDirection(curr_j, dir);

    if (!isThereWall[i] && !cellExists(neighbor_i, neighbor_j)) {
      addCell(neighbor_i, neighbor_j);
    }
  }
}

void AlgorithmGraph::updateDistancesFromDocking() {
  bool found_unvisited = false;
  bool found_dirty = false;
  std::vector<bool> visited(num_cells);
  std::vector<int> distances(num_cells);
  std::vector<Direction> parent(num_cells);
  std::fill(visited.begin(), visited.end(), false);
  std::fill(distances.begin(), distances.end(), -1);
  std::fill(parent.begin(), parent.end(), Direction::North);

  visited[cells.at(0).at(0).getID()] = true;
  distances[cells.at(0).at(0).getID()] = 0;
  cells.at(0).at(0).setDistanceFromDocking(0);
  std::queue<std::pair<int, int>> q;
  q.emplace(0, 0); // docking cell
  while (!(q.empty())) {

    int ptr_i = q.front().first;
    int ptr_j = q.front().second;
    int ptr_id = cells.at(ptr_i).at(ptr_j).getID();

    std::cout << "updateDistancesFromDocking: visiting cell " << ptr_i << ","
              << ptr_j << std::endl;

    found_dirty |= cellIsDirtyCondition(cells.at(ptr_i).at(ptr_j));
    found_unvisited |= cellWasntVisitedCondition(cells.at(ptr_i).at(ptr_j));

    q.pop();

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

        cells.at(north_coordinates.first)
            .at(north_coordinates.second)
            .setDistanceFromDocking(distances[north_cell_id]);
        cells.at(north_coordinates.first)
            .at(north_coordinates.second)
            .setDirectionToDocking(Direction::South);

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

      std::cout << "updateDistancesFromDocking: visiting eastern cell "
                << east_coordinates.first << "," << east_coordinates.second
                << std::endl;
      if (!visited[east_cell_id]) {
        std::cout << "updateDistancesFromDocking: eastern cell not visited"
                  << std::endl;
        visited[east_cell_id] = true;
        std::cout << "updating distance from docking to be "
                  << distances[ptr_id] << " + 1 for cell no. " << east_cell_id
                  << " which is in (" << east_coordinates.first << ","
                  << east_coordinates.second << ")" << std::endl;
        distances[east_cell_id] = distances[ptr_id] + 1;

        cells.at(east_coordinates.first)
            .at(east_coordinates.second)
            .setDistanceFromDocking(distances[east_cell_id]);

        cells.at(east_coordinates.first)
            .at(east_coordinates.second)
            .setDirectionToDocking(Direction::West);

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

        cells.at(south_coordinates.first)
            .at(south_coordinates.second)
            .setDistanceFromDocking(distances[south_cell_id]);
        cells.at(south_coordinates.first)
            .at(south_coordinates.second)
            .setDirectionToDocking(Direction::North);

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

        cells.at(west_coordinates.first)
            .at(west_coordinates.second)
            .setDistanceFromDocking(distances[west_cell_id]);
        cells.at(west_coordinates.first)
            .at(west_coordinates.second)
            .setDirectionToDocking(Direction::East);

        parent[west_cell_id] = Direction::East;
        q.push(west_coordinates);
      }
    }
  }
  finished_scanning |= !found_unvisited;
  finished_cleaning |= finished_scanning && !found_dirty;
}

bool AlgorithmGraph::finishedScanning() const { return finished_scanning; }

bool AlgorithmGraph::finishedCleaning() const { return finished_cleaning; }

bool AlgorithmGraph::finishedJob() const {
  return finishedCleaning() && finishedScanning();
}

std::pair<std::pair<int, Direction>, int> AlgorithmGraph::bfs(
    const std::function<bool(const VaccumGraphCell &)> &condition) const {
  std::pair<int, Direction> ret;
  ret.first = -1;
  ret.second = Direction::North;
  std::cout << "cell (" << curr_i << "," << curr_j
            << ") is dirty: " << cells.at(curr_i).at(curr_j).getIsDirty()
            << " is visited " << cells.at(curr_i).at(curr_j).getWasVisited()
            << std::endl;
  if (condition((cells.at(curr_i).at(curr_j)))) {
    std::cout << "bfs: found cell " << curr_i << "," << curr_j << std::endl;
    ret.first = 0;
    return std::pair<std::pair<int, Direction>, int>(
        ret, cells.at(curr_i).at(curr_j).getDistanceFromDocking());
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
    int ptr_i = q.front().first;
    int ptr_j = q.front().second;
    int ptr_id = cells.at(ptr_i).at(ptr_j).getID();
    std::cout << "bfs: visiting cell " << ptr_i << "," << ptr_j << std::endl;
    q.pop();

    std::cout << "cell (" << ptr_i << "," << ptr_j
              << ") is dirty: " << cells.at(ptr_i).at(ptr_j).getIsDirty()
              << " is visited " << cells.at(ptr_i).at(ptr_j).getWasVisited()
              << std::endl;
    if (condition(cells.at(ptr_i).at(ptr_j))) {

      std::cout << "cell (" << ptr_i << "," << ptr_j << ") passed condition"
                << std::endl;
      ret.first = distances[cells.at(ptr_i).at(ptr_j).getID()];
      int distance_ret_to_docking =
          cells.at(ptr_i).at(ptr_j).getDistanceFromDocking();
      // backtrack to current
      while (curr_i != ptr_i || curr_j != ptr_j) {
        Direction dir_to_parent = parent[cells.at(ptr_i).at(ptr_j).getID()];
        std::cout << "backtracking to (" << ptr_i << "," << ptr_j << ") from ()"
                  << locIByDirection(ptr_i, dir_to_parent) << ","
                  << locJByDirection(ptr_j, dir_to_parent) << "), direction is "
                  << directionString(dir_to_parent) << std::endl;
        int new_i = locIByDirection(ptr_i, dir_to_parent);
        int new_j = locJByDirection(ptr_j, dir_to_parent);
        if (new_i == curr_i && new_j == curr_j) {
          ret.second = oppositeDirection(dir_to_parent);
          std::cout << "found direction to cell: "
                    << directionString(ret.second) << std::endl;
          break;
        }
        ptr_i = new_i;
        ptr_j = new_j;
      }
      std::cout << "returning ((distance, direction), distance to docking): (("
                << ret.first << "," << directionString(ret.second) << "), "
                << distance_ret_to_docking << ")" << std::endl;
      return std::pair<std::pair<int, Direction>, int>(ret,
                                                       distance_ret_to_docking);
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

  return std::pair<std::pair<int, Direction>, int>(ret, -1);
}

std::pair<int, Direction> AlgorithmGraph::dockingDistAndDir() {
  return std::pair<int, Direction>(
      cells.at(curr_i).at(curr_j).getDistanceFromDocking(),
      cells.at(curr_i).at(curr_j).getDirectionToDocking());
}

std::pair<std::pair<int, Direction>, int> AlgorithmGraph::unvisitedBfs() {
  std::cout << "unvisitedBfs" << std::endl;
  std::pair<std::pair<int, Direction>, int> ret =
      bfs(cellWasntVisitedCondition);
  if (ret.first.first == -1) {
    finished_scanning = true;
  }
  return ret;
}

std::pair<std::pair<int, Direction>, int> AlgorithmGraph::dirtyBfs() {
  std::cout << "dirtyBfs" << std::endl;
  std::pair<std::pair<int, Direction>, int> ret = bfs(cellIsDirtyCondition);
  if (finished_scanning && ret.first.first == -1)
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

void VaccumGraphCell::setDistanceFromDocking(int distance) {
  distance_from_docking = distance;
}

int VaccumGraphCell::getDistanceFromDocking() const {
  return distance_from_docking;
}

void VaccumGraphCell::setDirectionToDocking(Direction dir) {
  direction_to_docking = dir;
}

Direction VaccumGraphCell::getDirectionToDocking() const {
  return direction_to_docking;
}
