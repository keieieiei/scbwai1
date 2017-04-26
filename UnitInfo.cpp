#include "UnitInfo.h"


UnitInfo::UnitInfo(const BWAPI::Unit u)
  : unit(u)
{
  this->unitID = u->getID();
  this->unitType = u->getType();
  this->position = u->getPosition();
  updateStartLocBase();
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

BWAPI::Position UnitInfo::getPosition()
{
  return this->position;
}

void UnitInfo::setPosition(BWAPI::Position tp)
{
  this->position = tp;
}

BWAPI::TilePosition UnitInfo::getStartLocBase()
{
  return this->startLocBase;
}

// TODO: currently assumes bases will be near start locations;
// replace with a smarter algorithm for finding bases?
void UnitInfo::updateStartLocBase()
{
  int minDist = 999999;
  int dist;
  BWAPI::Point<int, 32> p(position);
  for (auto tp : BWAPI::Broodwar->getStartLocations())
  {
    dist = tp.getApproxDistance(p);
    if (dist < minDist) {
      minDist = dist;
      startLocBase = tp;
    }
  }
}

void UnitInfo::updateUnitInfo(BWAPI::Unit u)
{
  setType(u->getType());
  setPosition(u->getPosition());
  updateStartLocBase();
}

