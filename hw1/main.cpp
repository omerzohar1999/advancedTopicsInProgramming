#include "House.h"
#include <iostream>
#include <string.h>

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cerr << "usage: " << argv[0] << " <input_file>" << std::endl;
    exit(1);
  }
  std::string input = argv[1];
  House *house = new House(input);
  while (!(house->changeState())) {
  }
  std::string output = "output_";
  output.append(input);
  house->create_output(output);
  return 0;
}
