#include "Drone.h"

Drone::Drone(const BWAPI::Unit &u)
  : unit(u)
  , isReturning(false)
  , isMovingToPatch(false)
  , isAtMinerals(false)
  , initialReturnFrame(0)
  , mineral(nullptr)
{

}

Drone::~Drone()
{

}