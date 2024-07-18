#include "MyAlgorithm.h"
#include "MySimulator.h"
#include <iostream>
#include <memory>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "usage: " << argv[0] << " <input_file>" << std::endl;
        return 1;
    }
    std::string input = argv[1];
    auto house = std::make_shared<MySimulator>(input);
    MyAlgorithm robot = MyAlgorithm();
    house->setAlgorithm(robot);
    return !(house->run() && house->createOutput(input));
}