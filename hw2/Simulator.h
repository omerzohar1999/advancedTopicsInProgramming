#ifndef SIM_H
#define SIM_H

#include "AbstractAlgorithm.h"
#include "Direction.h"

class Simulator {
public:
  virtual ~Simulator() {};
  virtual bool isThereWall(Direction dir) const = 0;
  virtual int howMuchDirtHere() const = 0;
  virtual int getBatteryLeft() const = 0;
  virtual void setAlgorithm(AbstractAlgorithm &algo) = 0;
  virtual bool run() = 0;
};
#endif