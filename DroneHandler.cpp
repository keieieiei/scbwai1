#include "DroneHandler.h"

DroneHandler::DroneHandler(BWAPI::Unit u)
  : UnitHandler(u)
  , state{ GatherState::DEFAULT }
  , baseManager(std::weak_ptr<BaseManager>())
{

}

DroneHandler::~DroneHandler()
{

}

void DroneHandler::update()
{
  switch (objective)
  {
  case Objective::GATHER_MINERALS:
    // if mineral target doesn't exist, request assignment again
    if (targetUnit == nullptr)
      state = GatherState::DEFAULT;
    else if (!targetUnit->exists())
      state = GatherState::DEFAULT;
    // if baseManager is gone (might want a method to reset everything)
    if (baseManager.expired())
    {
      state = GatherState::DEFAULT;
      objective = Objective::NONE;
      targetPos = { -1, -1 };
      targetUnit = nullptr;
    }

    switch (state)
    {
    case GatherState::MOVE_TO_MINERALS:
      // Check if worker should be pushed to the next state
      if (unit->getDistance(targetUnit->getPosition()) < 10) // test how far we should do this (probs 30-40)
        state = GatherState::WAIT_FOR_MINERALS;
    case GatherState::WAIT_FOR_MINERALS:
      // Check if worker is carrying something, in which case it moves on to the next state
      // TODO: what if worker is carrying something but not moving to return it?
      if ((BWAPI::Filter::IsCarryingSomething)(unit))
      {
        state = GatherState::RETURN_MINERALS;
        // update minerals initialGatherFrame and worker count
        baseManager.lock()->finishGathering(targetUnit);
      }
      // otherwise make sure worker is focused on the assigned mineral
      // Note: applies to MOVE_TO_MINERALS as well
      else if (unit->getOrderTarget() != targetUnit)
        unit->gather(targetUnit);
      break;
    case GatherState::RETURN_MINERALS:
      if ((BWAPI::Filter::IsCarryingSomething)(unit))
        break;
      // if the worker just delivered resources however...
    case GatherState::DEFAULT:
      targetUnit = baseManager.lock()->requestMineralAssignment(unit->getPosition());
      // make sure it's not nullptr
      if (targetUnit == nullptr)
        break;
      unit->gather(targetUnit);
      // hurgghh since basemanager is using this to reassign us...
      if (targetUnit->getType() == BWAPI::UnitTypes::Zerg_Extractor)
      {
        state = GatherState::GAS;
        objective = Objective::GATHER_GAS;
      }
      else
        state = GatherState::MOVE_TO_MINERALS;
      break;
    }
    break;
  case Objective::GATHER_GAS:
    if (!targetUnit->exists() || targetUnit->getResources() == 0)
    {
      state = GatherState::DEFAULT;
      resetObjective();
    }
    if (unit->isIdle())
      unit->gather(targetUnit);
    break;
  }

  /* TODO: implement these after we verify the swap to DroneHandler works
  BWAPI::Orders::MoveToMinerals;
  BWAPI::Orders::WaitForMinerals;
  BWAPI::Orders::MiningMinerals;
  BWAPI::Orders::ReturnMinerals;
  */

  /* TODO: what are these?
  BWAPI::Orders::Harvest1;
  BWAPI::Orders::Harvest2;
  BWAPI::Orders::Harvest3;
  BWAPI::Orders::Harvest4;
  */
}

bool DroneHandler::setObjective(Objective o, BWAPI::Unit u)
{
  // gathering objectives
  if (o == Objective::GATHER_GAS)
    state = GatherState::GAS;

  if (o >= Objective::GATHER_MINERALS && o <= Objective::GATHER_GAS)
    return UnitHandler::setObjective(o, u);

  return false;
}

GatherState DroneHandler::getGatherState()
{
  return state;
}

void DroneHandler::giveBaseManager(std::shared_ptr<BaseManager> bm)
{
  baseManager = bm;
}

BWAPI::Unit DroneHandler::getTargetUnit()
{
  return targetUnit;
}