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

  // shitty attack & scouting subroutine
  switch (objective)
  {
  case Objective::ASSAULT:
  {
    BWAPI::Unit bestTarget = unit->getClosestUnit(BWAPI::Filter::IsEnemy && BWAPI::Filter::CanAttack && BWAPI::Filter::IsDetected);
    BWAPI::Unit secondTarget = unit->getClosestUnit(BWAPI::Filter::IsEnemy && BWAPI::Filter::IsDetected && !(BWAPI::Filter::Armor > 5));
    if (bestTarget != nullptr)
    {
      if (unit->getTarget() != bestTarget)
        unit->attack(bestTarget);
    }
    else if (secondTarget != nullptr)
    {
      if (unit->getTarget() != secondTarget)
        unit->attack(secondTarget);
    }
    else if (unit->getOrderTargetPosition() != target)
      unit->attack(target); // this does not work well when spammed
    break;
  }
  case Objective::DEFEND:
  {
    BWAPI::Unit bestTarget = unit->getClosestUnit(BWAPI::Filter::IsEnemy && BWAPI::Filter::CanAttack && BWAPI::Filter::IsDetected, 500);
    BWAPI::Unit secondTarget = unit->getClosestUnit(BWAPI::Filter::IsEnemy && BWAPI::Filter::IsDetected && !(BWAPI::Filter::Armor > 5), 500);
    if (bestTarget != nullptr)
    {
      if (unit->getTarget() != bestTarget)
        unit->attack(bestTarget);
    }
    else if (secondTarget != nullptr)
    {
      if (unit->getTarget() != secondTarget)
        unit->attack(secondTarget);
    }
    else if (unit->getOrderTargetPosition() != target)
      unit->move(target); // this does not work well when spammed
    break;
  }
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