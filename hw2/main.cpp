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
    std::shared_ptr house = std::make_shared<MySimulator>();
    bool read_error = house->readHouseFile(input);
    if (read_error)
        return 1;
    MyAlgorithm algo;
    house->setAlgorithm(algo);
    bool run_error = house->run();
    bool output_error = house->createOutput(input);
    return (run_error || output_error);
}