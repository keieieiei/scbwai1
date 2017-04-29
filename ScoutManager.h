#include <BWAPI.h>
#include <BWTA.h>
#include "BaseManager.h"
#include <memory>
#include "OverlordHandler.h"
#include "BuildExecutor.h"

/*
  Explores the enemy base and forces

  Information Required:
    Starting Base Locations (provided by BWTA)
    All Base Locations (base locations near minerals provided by BWTA)
    Locations or Buildings/Tech that infomanager or strategymanager might desire
    Areas that we should provide vision in case something passes through (might be able to determine this here) (eg: dropship paths or expansion points)
    recon of enemy forces locations

  Resources Required:
    Access to overlords/zerglings/drones & other units that can be used to scout (scourge? muta?)

  Future Functionality:
    Base Location prediction upon seeing enemy scouts/units
    Improved AI to scout around the enemy Base
    Ability to create "vision lines" around the enemy base and through the map to track enemy unit movements
      or just in general to spread vision among the map safely
    Ability to predict where units spotted with the vision lines might go
    Ability to trail and track high value units
*/

// these really should be somewhere else more generic, i think, maybe
typedef bool(*scdef) (BWAPI::Unit, std::shared_ptr<BaseManager>);
bool fourpoolscdef(BWAPI::Unit u, std::shared_ptr<BaseManager> bm);
bool fivepoolscdef(BWAPI::Unit u, std::shared_ptr<BaseManager> bm);
bool ninepoolscdef(BWAPI::Unit u, std::shared_ptr<BaseManager> bm);
bool defaultscdef(BWAPI::Unit u, std::shared_ptr<BaseManager> bm);


class ScoutManager
{
private:
  struct Task
  {
    ScoutObjective objective;
    BWAPI::Position position;
    std::weak_ptr<OverlordHandler> agent; // should be ScoutHandler
  };

  // should be weak ptr in the future but for now easier to do it like this
  std::vector<std::shared_ptr<OverlordHandler>> overlords;
  std::vector<Task> tasks; // maybe should be priority queue if we implement a priority system
  // TODO update to agent drone
  std::vector<BWAPI::Unit> drones;
  BWAPI::Position enemyMain;
  std::vector<BWAPI::Position> startingLocations;
  //hackityhack
  bool mainExplored = false;
public:
  ScoutManager();
  ~ScoutManager();

  void update();
  void findEnemyBase();
  void addEnemyBase(BWAPI::Position pos);
  void addScout(BWAPI::Unit u);
  bool containsUnit(BWAPI::Unit u);
  // hacky hack hack
  BWAPI::Position getFurthestStartingLocation();
  // hacky
  bool isMainExplored();
  scdef getScoutConditionFunction(BuildOrder bo);
};