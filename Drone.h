#pragma once
#include <BWAPI.h>

class Drone
{
public:
  Drone(const BWAPI::Unit &u);
  ~Drone();

  BWAPI::Unit unit;

  bool operator ==(const Drone &d) const
  {
    return this->unit == d.unit;
  }
};