#include "ScoutManager.h"

ScoutManager::ScoutManager()
  : enemyMain{ -1, -1 }
{
  for (BWAPI::TilePosition tp : BWAPI::Broodwar->getStartLocations())
  {
    startingLocations.push_back(BWAPI::Position(tp));
  }
  BWAPI::Position pos = BWAPI::Position(BWAPI::Broodwar->self()->getStartLocation());
  std::sort(std::begin(startingLocations), std::end(startingLocations),
    [&pos](BWAPI::Position &a, BWAPI::Position &b){ return pos.getDistance(a) < pos.getDistance(b); });

  // hackityhack
  mainExplored = false;
  printf("%d start locations found\n", BWTA::getStartLocations().size());
  printf("%d base locations found\n", BWTA::getBaseLocations().size());
}

ScoutManager::~ScoutManager()
{

}

void ScoutManager::update()
{
  for (auto &overlord : overlords)
    overlord->update();

  if (!enemyMain.isValid())
  {
    if (startingLocations.size() == 4)
    {
      if (BWAPI::Broodwar->isExplored(BWAPI::TilePosition(startingLocations[1])) && BWAPI::Broodwar->isExplored(BWAPI::TilePosition(startingLocations[2])))
        enemyMain = startingLocations[3];
    }
    else if (startingLocations.size() == 3)
    {
      if (BWAPI::Broodwar->isExplored(BWAPI::TilePosition(startingLocations[1])))
        enemyMain = startingLocations[2];
    }
    else if (startingLocations.size() == 2)
    {
      enemyMain = startingLocations[1];
    }
  }
}

void ScoutManager::findEnemyBase()
{
  // is this working? i have no idea. the case is so rare i'm not seeing happen r.i.p.
  // update: seen it once; but the entire map was discovered but no enemy to be found....  wtf?
  // we've found and destroyed main but the game hasn't ended; search; super super super hacky lazy way
  for (const BWTA::BaseLocation *b : BWTA::getBaseLocations())
  {
    if (!BWAPI::Broodwar->isExplored(b->getTilePosition()))
    {
      for (auto u : overlords)
      {
        overlords.back()->setObjective(Objective::REVEAL_TILE, b->getPosition());
      }
      continue;
    }
  }
}

void ScoutManager::addEnemyBase(BWAPI::Position pos)
{
  if (!enemyMain.isValid())
  {
    int minDistance = std::numeric_limits<int>::max();
    for (const auto &startingLocation : startingLocations)
    {
      if (pos.getApproxDistance(startingLocation) < minDistance)
      {
        minDistance = pos.getApproxDistance(startingLocation);
        enemyMain = pos;
      }
    }
  }
  //printf("added main base at %d, %d\n", pos.x, pos.y);
  mainExplored = true;
}

void ScoutManager::addScout(BWAPI::Unit u)
{
  printf("addy addy\n");
  if (u->getType() == BWAPI::UnitTypes::Zerg_Drone)
  {
    printf("addy drone\n");
    drones.push_back(u);
    if (startingLocations.size() > 2)
      u->move(startingLocations[2]);
  }
  else if (u->getType() == BWAPI::UnitTypes::Zerg_Overlord)
  {
    printf("addy overlord\n");
    overlords.push_back(std::make_shared<OverlordHandler>(OverlordHandler(u)));
    overlords.back()->setObjective(Objective::REVEAL_TILE, startingLocations[1]);
  }
}

bool ScoutManager::containsUnit(BWAPI::Unit u)
{
  for (const auto &overlord : overlords)
    if (overlord->unit == u)
      return true;
  return false;
}

// hacky hack hack hack
BWAPI::Position ScoutManager::getFurthestStartingLocation()
{
  return startingLocations.back();
}

// hacky
bool ScoutManager::isMainExplored()
{
  return mainExplored;
}

bool ScoutManager::isMainFound()
{
  return enemyMain.isValid();
}

int ScoutManager::numDrones()
{
  return drones.size();
}

/* dynamic scout condition functions - obsolete
scdef ScoutManager::getScoutConditionFunction(BuildOrder bo)
{
  switch (bo)
  {
  case BuildOrder::FOURPOOL:
    return &fourpoolscdef;
  case BuildOrder::FIVEPOOL:
      return &fivepoolscdef;
  case BuildOrder::NINEPOOL:
    return &ninepoolscdef;
  default:
    return &defaultscdef;
  }
}

bool defaultscdef(BWAPI::Unit u, std::shared_ptr<BaseManager> bm)
{
  return false;
}

bool fourpoolscdef(BWAPI::Unit u, std::shared_ptr<BaseManager> bm)
{
  if (u->getType() == BWAPI::UnitTypes::Zerg_Spawning_Pool)
  {
    if (!u->isCompleted() && u->getRemainingBuildTime() < 400 && bm->numWorkers() > 3)
      return true;
  }
  return false;
}

bool fivepoolscdef(BWAPI::Unit u, std::shared_ptr<BaseManager> bm)
{
  if (u->getType() == BWAPI::UnitTypes::Zerg_Spawning_Pool)
  {
    if (!u->isCompleted() && u->getRemainingBuildTime() < 400 && bm->numWorkers() > 5)
      return true;
  }
  return false;
}

bool ninepoolscdef(BWAPI::Unit u, std::shared_ptr<BaseManager> bm)
{
  if (u->getType() == BWAPI::UnitTypes::Zerg_Spawning_Pool)
  {
    if (!u->isCompleted() && u->getRemainingBuildTime() < 400 && bm->numWorkers() > 9)
      return true;
  }
  return false;
}
*/