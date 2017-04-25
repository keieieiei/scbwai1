#include "UnitInfo.h"


UnitInfo::UnitInfo(const BWAPI::Unit u)
  : unit(u)
{
  this->unitID = u->getID();
  this->unitType = u->getType();
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