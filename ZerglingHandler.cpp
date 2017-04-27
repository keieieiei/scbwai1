#include "ZerglingHandler.h"

ZerglingHandler::ZerglingHandler(BWAPI::Unit u)
  : unit{ u }
  , target()
  , objective{ Objective::NONE }
{

}

ZerglingHandler::~ZerglingHandler()
{

}

void ZerglingHandler::update()
{
  BWAPI::Broodwar->drawTextMap(unit->getPosition(), "%d", objective);

  if (!(BWAPI::Broodwar->getFrameCount() % 5 == 0))
    return;

  // shitty attack & scouting subroutine
  switch (objective)
  {
  case Objective::ASSAULT:
    if (!unit->attack(unit->getClosestUnit(BWAPI::Filter::IsEnemy)))
      unit->attack(target);
    break;
  case Objective::DEFEND:
    if (!unit->attack(unit->getClosestUnit(BWAPI::Filter::IsEnemy)))
      unit->move(target);
    break;
  case Objective::NONE:
    break;
  }
}

void ZerglingHandler::attack(BWAPI::Position pos)
{
  target = pos;
  objective = Objective::ASSAULT;
}

void ZerglingHandler::defend(BWAPI::Position pos)
{
  target = pos;
  objective = Objective::DEFEND;
}