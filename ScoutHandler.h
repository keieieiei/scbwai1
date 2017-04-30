#pragma once
#include <BWAPI.h>
#include "UnitHandler.h"

class ScoutHandler: public UnitHandler
{
  ScoutHandler(BWAPI::Unit u);
  virtual ~ScoutHandler();
};