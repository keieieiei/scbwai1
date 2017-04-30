#pragma once
#include <BWAPI.h>

enum class Objective
{
  // Attack 0 -> 1 inclusive
  ASSAULT,
  DEFEND,

  // Scout 2 -> 6 inclusive
  REVEAL_TILE,
  WATCH_TILE,
  SPREAD_VISION,
  EXPLORE_BASE,
  TRACK_UNIT,

  // Default 7
  NONE,
};

class UnitHandler
{
protected:
  BWAPI::Unit targetUnit;
  BWAPI::Position targetPos;
  Objective objective;

public:
  UnitHandler(BWAPI::Unit u);
  virtual ~UnitHandler();

  virtual void update() = 0;
  virtual bool setObjective(Objective o, BWAPI::Position p);
  virtual bool setObjective(Objective o, BWAPI::Unit u);

  const BWAPI::Unit unit;
};