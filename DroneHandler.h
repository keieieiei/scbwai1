#pragma once
#include <BWAPI.h>
#include <memory>
#include "UnitHandler.h"
#include "BaseManager.h"

class BaseManager;

enum class GatherState
{
  DEFAULT,
  MOVE_TO_MINERALS,
  WAIT_FOR_MINERALS,
  MINING_MINERALS, // i guess this is unused
  RETURN_MINERALS,
};

class DroneHandler : public UnitHandler
{
private:
  // TODO: update to Orders later
  GatherState state;
  std::weak_ptr<BaseManager> baseManager;

public:
  DroneHandler(BWAPI::Unit u);
  virtual ~DroneHandler();

  virtual void update();
  virtual bool setObjective(Objective o, BWAPI::Unit u);
  
  GatherState getGatherState();
  void giveBaseManager(std::shared_ptr<BaseManager> bm);
  BWAPI::Unit getTargetUnit();
};