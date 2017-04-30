#pragma once
#include <BWAPI.h>
#include "UnitHandler.h"

class ZerglingHandler: public UnitHandler
{
public:
  ZerglingHandler(BWAPI::Unit u);
  virtual ~ZerglingHandler();

  virtual void update();
  virtual bool setObjective(Objective o, BWAPI::Position p);
  // virtual bool setObjective(Objective o, BWAPI::Unit u); don't need this yet ... will we ever?
};