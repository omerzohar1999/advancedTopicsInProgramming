import sys

def parse_alternative_format(file_content):
    lines = file_content.strip().split('\n')
    max_steps, max_battery = map(int, lines[0].split(','))

    house = {}
    docking_station = None
    max_x = max_y = 0

    for line in lines[1:]:
        x, y, wall_n, wall_e, wall_s, wall_w, dirt = line.split(',')
        x, y = int(x), int(y)
        max_x = max(max_x, x)
        max_y = max(max_y, y)

        walls = [wall_n == 'true', wall_e == 'true', wall_s == 'true', wall_w == 'true']
        dirt = int(dirt)

        house[(x, y)] = {'walls': walls, 'dirt': dirt}

        if docking_station is None:
            docking_station = (x, y)

    return max_steps, max_battery, house, docking_station, max_x + 1, max_y + 1

def generate_assignment_format(max_steps, max_battery, house, docking_station, rows, cols):
    output = [
        "House Name",
        f"MaxSteps = {max_steps}",
        f"MaxBattery = {max_battery}",
        f"Rows = {rows}",
        f"Cols = {cols}"
    ]

    for y in range(rows):
        row = ""
        for x in range(cols):
            if (x, y) == docking_station:
                row += 'D'
            elif (x, y) in house:
                cell = house[(x, y)]
                if any(cell['walls']):
                    row += 'W'
                elif cell['dirt'] > 0:
                    row += str(cell['dirt'])
                else:
                    row += ' '
            else:
                row += ' '
        output.append(row)

    return '\n'.join(output)

def convert_file(input_file, output_file):
    with open(input_file, 'r') as f:
        content = f.read()

    max_steps, max_battery, house, docking_station, rows, cols = parse_alternative_format(content)
    result = generate_assignment_format(max_steps, max_battery, house, docking_station, rows, cols)

    with open(output_file, 'w') as f:
        f.write(result)

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python script.py <input_file> <output_file>")
        sys.exit(1)

    input_file, output_file = sys.argv[1], sys.argv[2]
    convert_file(input_file, output_file)
    print(f"Conversion completed. Output written to {output_file}")