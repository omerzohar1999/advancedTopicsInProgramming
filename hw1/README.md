Contributors:
- Omer Zohar
- Amit Novick

Compilation:
```bash
make
```

Usage:
```bash
./myrobot <house_input_file>
```

Input file format:
```
max_steps: int,max_battery_size: int
docking_station_x: int,docking_station_y: int,is_wall_north: bool,is_wall_east: bool,is_wall_south: bool,is_wall_west: bool,dirt_level: int
x: int,y: int,is_wall_north: bool,is_wall_east: bool,is_wall_south: bool,is_wall_west: bool,dirt_level: int
x: int,y: int,is_wall_north: bool,is_wall_east: bool,is_wall_south: bool,is_wall_west: bool,dirt_level: int
x: int,y: int,is_wall_north: bool,is_wall_east: bool,is_wall_south: bool,is_wall_west: bool,dirt_level: int
.
.
.
```

Output file format:
```
Action: one of {North, East, South, West, Stay}
Action: one of {North, East, South, West, Stay}
Action: one of {North, East, South, West, Stay}
.
.
.
Number of steps: (int)
Dirt left: (int)
Vacuum cleaner is {alive / dead}
Mission ended {successfully / unsuccessfully}
```

Solution:
- Locations in the house are represented by square tiles
- Every location can have walls in each of 4 directions: N/E/S/W
- The vacuum cleaner first explores the house for adjacent (not wall-separated) locations
- Explored locations are saved as a mapping of the house known to the vacuum cleaner
- The vacuum cleaner will attempt to clean all visited locations
- The mission is successful iff there is 0 dirt left in the house, and the vacuum cleaner is back in the docking station.

Input files:
- `input_a.txt`: A 1x3 strip that should end with unsuccessful
- `input_b.txt`: A 3x3 maze that should end with successful
- `input_c.txt`: A 6x7 squarish house that should end with unsuccessful

(Optional) Configuration file:
- If a text file `project-config.txt` is present, setting `VISUALIZATION = true` in it will write an ASCII text visualization of the house to a file named `visualization.txt`
