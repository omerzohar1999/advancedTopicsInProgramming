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
<Description of file>
MaxSteps = NNN
MaxBattery = NNN
Rows = NNN
Cols = NNN
<W/D/1-9/anything else>
<W/D/1-9/anything else>
<W/D/1-9/anything else>
.
.
.
```

Output file format:
```
NumSteps = NNN
DirtLeft = NNN
Status = <FINISHED/WORKING/DEAD>
Steps:
sequence of <N/E/S/W/s>, optionally F in the end
```

Solution:
- Locations in the house are represented by square tiles
- Every location is either empty space or a wall
- The vacuum cleaner first explores the house for adjacent (not wall-separated) locations
- Explored locations are saved as a mapping of the house known to the vacuum cleaner
- The vacuum cleaner will attempt to clean all visited locations
- The mission is successful iff there is 0 dirt left in the house, and the vacuum cleaner is back in the docking station.

Input files:
- `input_a.txt`: A 1x3 strip that should end with unsuccessful
- `input_b.txt`: A 3x5 maze that should end with successful
- `input_c.txt`: A 6x7 squarish house that should end with successful

(Optional) Configuration file:
- If a text file `project-config.txt` is present, setting `VISUALIZATION = true` in it will write an ASCII text visualization of the house to a file named `visualization.txt`
