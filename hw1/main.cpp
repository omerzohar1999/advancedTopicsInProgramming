#include "House.h"
#include <iostream>
#include <string.h>

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cerr << "usage: " << argv[0] << " <input_file>" << std::endl;
    return 1;
  }
  std::string input = argv[1];
  House *house = new House(input);
  if (!house->clean()) {
    return 1;
  }
  if (house->createOutput("output_" + input))
    return 1;
  return 0;
}
