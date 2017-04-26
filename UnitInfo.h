#pragma once
#include <BWAPI.h>

class UnitInfo
{
private:
  int unitID;
  BWAPI::Unit unit;
  BWAPI::UnitType unitType;
  BWAPI::Position position;
  BWAPI::TilePosition startLocBase;
public:
  UnitInfo(BWAPI::Unit u);
  ~UnitInfo();

  int getID();
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
};

