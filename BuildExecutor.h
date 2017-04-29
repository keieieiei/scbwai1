#pragma once
#include <BWAPI.h>
#include <queue>
#include <unordered_map>
#include <memory>
#include "BaseManager.h"

/*
  Takes a static build order as input and executes that production through the game

  Information Required:
    Ordered List of BWAPI::UnitTypes representing the build order
    Build Positions for any of the buildings (currently using the built-in building placement finder)
    Knowledge of mineral cost
    Knowledge of the success of the previous build step ?

  Resources Required:
    Access to production facilities (hatchery) for units
    Access to a Drone unit (&command over it for now but should swap to giving the drone a "build" order at a certain position and having the drone execute it)
    Access to units that can morph (Hydralisk->Lurker; Mutalisk->Guardian)
      or buildings that can morph (Spire->Greater Spire; Hatchery->Lair->Hive; Creep Colony->Sunken Colony/Spore Colony)

    Note: maybe the above could be generalized since all are essentially just morphable units

  Future Functionality:
    Support for inserting, appending, or shifting production reactively when prompted
    Ability to build swap when prompted by ?strategy manager?
    Ability to store a forking build order and choose the path as prompted (may be same as above)
    Ability to continue production with no input based on rules:
      eg: "build drones if we don't have 24; build zerglings and hydralisks at a 2:1 ratio if resources allow, otherwise hydras until gas is gone and zerglings with the rest"
  
    Note: perhaps some of this would be best handled by a BuildManager and this class would focus purely as an interface for execution 1 by 1?

  Note: perhaps this class would better serve as a BuildManager and let the individual agents calculate the execution details
  Note: use of potential field to clear the build area?
*/

enum class BuildOrder
{
  FOURPOOL,
  FIVEPOOL,
  NINEPOOL,
};

class BuildExecutor
{
private:
  std::queue<BWAPI::UnitType> buildOrder;
  std::vector<BWAPI::Unit> larvae;
  const static std::unordered_map<BuildOrder, std::queue<BWAPI::UnitType>> builds;

  // TODO: Temporary access to mainManager in order to grab a worker
  std::weak_ptr<BaseManager> mainManager;
  // stupid fucking shitty way to make sure it builds the fucking thing we want it to before we spend minerals
  // also only works for one fucking building b/c fuck this
  int mineralReserve;
  BWAPI::Unit droneBetterBuildShitOrItsAFuckingMotherfucker;

  // temp add; should probably restructure a bit for buildorders if we keep the dynamic scout condition function method
  BuildOrder activeBuild;
public:
  BuildExecutor(BuildOrder bo);
  ~BuildExecutor();

  void update();
  bool containsLarva(BWAPI::Unit u);
  void addLarva(BWAPI::Unit u);
  // temp add; should probably restructure a bit for buildorders if we keep the dynamic scout condition function method
  BuildOrder getBuildOrder();

  // TODO: Temporary way to set mainManager pointer
  void giveMainManager(const std::shared_ptr<BaseManager> &mm);
};