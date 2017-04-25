#pragma once
#include <BWAPI.h>

class UnitInfo
{
private:
  int unitID;
  BWAPI::Unit unit;
  BWAPI::UnitType unitType;
public:
  UnitInfo(BWAPI::Unit u);
  ~UnitInfo();

  int getID();
  BWAPI::UnitType getType();
  void setType(BWAPI::UnitType);

  bool operator ==(BWAPI::Unit u)
  {
    return this->unitID == u->getID();
  }
};

