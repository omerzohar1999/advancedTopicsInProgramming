#include "House.h"
#include <iostream>

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cerr << "usage: " << argv[0] << " <input_file>" << std::endl;
    return 1;
  }
  std::string input = argv[1];
  MySimulator house = MySimulator(input);

  return !(house.run() && house.createOutput(input));
}