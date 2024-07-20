#ifndef SIM_H
#define SIM_H

#include "abstract_algorithm.h"
#include "enums.h"

class Simulator {
public:
    virtual ~Simulator() {};

    virtual bool isThereWall(Direction dir) const = 0;

    virtual int howMuchDirtHere() const = 0;

    virtual float getBatteryLeft() const = 0;

    virtual void setAlgorithm(AbstractAlgorithm &algo) = 0;

    virtual bool run() = 0;
};

#endif