#include "UnitInfo.h"


UnitInfo::UnitInfo(int uid, BWAPI::UnitType ut)
  : unitID(uid)
  , unitType(ut)
{
}


UnitInfo::~UnitInfo()
{
}

int UnitInfo::getID()
{
  return this->unitID;
}

BWAPI::UnitType UnitInfo::getType()
{
  return this->unitType;
}

void UnitInfo::setType(BWAPI::UnitType ut)
{
  this->unitType = ut;
}