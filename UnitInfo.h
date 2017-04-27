#pragma once
#include <BWAPI.h>
#include "Debug.h"

class UnitInfo
{
private:
  int unitID;
  BWAPI::Unit unit;
  BWAPI::UnitType unitType;
  BWAPI::Position position;
  BWAPI::TilePosition startLocBase;

  static bool debug;
public:
  UnitInfo(BWAPI::Unit u);
  ~UnitInfo();

  int getID();
  BWAPI::Unit getUnit();
  BWAPI::UnitType getType();
  void setType (BWAPI::UnitType);
  BWAPI::Position getPosition();
  void setPosition(BWAPI::Position);
  BWAPI::TilePosition getStartLocBase();
  void updateStartLocBase();
  void updateUnitInfo(BWAPI::Unit);

  bool operator ==(BWAPI::Unit u)
  {
    return this->unitID == u->getID();
  }

  static void setDebug(bool b) { debug = b; }
};

