#include "OverlordHandler.h"

OverlordHandler::OverlordHandler(BWAPI::Unit u)
  : UnitHandler(u)
{

}

OverlordHandler::~OverlordHandler()
{

}

void OverlordHandler::update()
{
  switch (objective)
  {
  case Objective::REVEAL_TILE:
    if (unit->getOrderTargetPosition() != targetPos)
      unit->move(targetPos);
    break;
  }
}

bool OverlordHandler::setObjective(Objective o, BWAPI::Position p)
{
  // only handles scouting objectives for now
  if (o >= Objective::REVEAL_TILE && o <= Objective::TRACK_UNIT)
    return UnitHandler::setObjective(o, p);
  
  return false;
}
