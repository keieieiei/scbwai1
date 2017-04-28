#include "OverlordHandler.h"

OverlordHandler::OverlordHandler(BWAPI::Unit u)
  : unit{ u }
  , objective{ ScoutObjective::NONE }
{

}

OverlordHandler::~OverlordHandler()
{

}

void OverlordHandler::update()
{
  switch (objective)
  {
  case ScoutObjective::REVEAL_TILE:
    if (unit->getOrderTargetPosition() != targetPosition)
      unit->move(targetPosition);
    break;
  }
}

void OverlordHandler::revealTile(BWAPI::Position tp)
{
  targetPosition = tp;
  objective = ScoutObjective::REVEAL_TILE;
}
