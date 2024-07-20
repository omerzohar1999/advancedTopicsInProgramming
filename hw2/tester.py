import os
import subprocess
import filecmp

def run_test(input_file, expected_return):
    # Run the executable with the input file
    result = subprocess.run(['./myrobot', f'test_files/{input_file}'], capture_output=True)

    # Check the return value
    if result.returncode == expected_return:
        print(f"OK returned value for {input_file}")
    else:
        print(f"Expected returned value: {expected_return}, Received: {result.returncode} for {input_file}")

    # Compare output files
    output_file = f"test_files/output_{input_file}"
    expected_output = f"test_files/output_{input_file[6:]}"  # Remove "input_" prefix

    if filecmp.cmp(output_file, expected_output):
        print(f"OK output file for {input_file}")
    else:
        print(f"Output file different from expected for {input_file}")

    return result.returncode == expected_return and filecmp.cmp(output_file, expected_output)

def main():
    test_cases = [
        ("input_a.txt", 1),
        ("input_b.txt", 0),
        ("input_c.txt", 0)
    ]

    all_passed = True

    for input_file, expected_return in test_cases:
        if not run_test(input_file, expected_return):
            all_passed = False
        print()  # Empty line for readability

    if all_passed:
        print("All tests passed")
    else:
        print("Some test failed")

if __name__ == "__main__":
    main()