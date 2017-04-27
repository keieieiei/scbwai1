#include "UnitInfo.h"

bool UnitInfo::debug = false;

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

BWAPI::Unit UnitInfo::getUnit()
{
  return this->unit;
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
// this method may not be necessary
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
  const char *s = u->getPlayer()->getID() == BWAPI::Broodwar->self()->getID() ? "player" : "enemy";
  if (debug) printf("UI: Updated %s %s %d @ %d, %d to %s @ %d, %d\n", s, unitType.c_str(), unitID, position.x, position.y, u->getType().c_str(), u->getPosition().x, u->getPosition().y);

  setType(u->getType());
  setPosition(u->getPosition());
  updateStartLocBase();
}

