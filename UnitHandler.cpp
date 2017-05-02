#include "UnitHandler.h"

UnitHandler::UnitHandler(BWAPI::Unit u)
  : unit{ u }
  , targetUnit{ nullptr }
  , targetPos{ -1, -1 }
  , objective{ Objective::NONE }
{

}

UnitHandler::~UnitHandler()
{

}

bool UnitHandler::setObjective(Objective o, BWAPI::Position p)
{
  targetUnit = nullptr;
  targetPos = p;
  objective = o;

  return true;
}

bool UnitHandler::setObjective(Objective o, BWAPI::Unit u)
{
  targetUnit = u;
  targetPos = { -1, -1 };
  objective = o;

  return true;
}

void UnitHandler::resetObjective()
{
  targetUnit = nullptr;
  targetPos = { -1, -1 };
  objective = Objective::NONE;
}