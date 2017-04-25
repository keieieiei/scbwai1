#pragma once
#include <BWAPI.h>
// #include "Mineral.h"

class Mineral; // resolve circular dependency

class Drone
{
public:
  Drone(const BWAPI::Unit &u);
  ~Drone();
  // QUESTION: is this the right place to put this? is there a better way to organize?
  //   might be better to have a struct with Drone and this data in BaseManager
  bool isReturning;
  bool isMovingToPatch;
  bool isAtMinerals;

  int initialReturnFrame;
  Mineral *mineral; // QUESTION: is this the best way to do it?  pointer to const

  BWAPI::Unit unit;

  bool operator ==(const Drone &d) const
  {
    return this->unit == d.unit;
  }
};