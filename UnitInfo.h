#pragma once
#include <BWAPI.h>

class UnitInfo
{
private:
  int unitID;
  BWAPI::UnitType unitType;
public:
  UnitInfo(int uid, BWAPI::UnitType ut);
  ~UnitInfo();

  int getID();
  BWAPI::UnitType getType();
  void setType(BWAPI::UnitType);

  bool operator ==(BWAPI::Unit u)
  {
    return this->unitID == u->getID();
  }
};

