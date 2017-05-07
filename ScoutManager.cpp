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

  //////////////////////////////////////
  // populate baseLocations with a list of base locations, as per BWTA, sorted by distance from the player's starting location
  
  // create a std::set copy to pass to getGroundDistances
  std::set<BWAPI::TilePosition> targets;

  // get the BaseLocation of the player's starting location/main base
  const BWTA::BaseLocation *playerMain = BWTA::getStartLocation(BWAPI::Broodwar->self());
  printf("main at %d, %d\n", playerMain->getPosition().x, playerMain->getPosition().y);

  // store baselocations into a vector (for sorting and using later) and a set (to pass to getGroundDistances)
  for (BWTA::BaseLocation *b : BWTA::getBaseLocations())
  {
    baseLocations.push_back(b);
    targets.insert(b->getTilePosition());
  }

  // get a map of distances from the main base to all bases
  std::map<BWAPI::TilePosition, double> blmap = BWTA::getGroundDistances(playerMain->getTilePosition(), targets);
  // sort the vector, passing the map (for now)
  std::sort(baseLocations.begin(), baseLocations.end(),
    [blmap](const BWTA::BaseLocation *a, const BWTA::BaseLocation *b){ return blmap.find(a->getTilePosition())->second < blmap.find(b->getTilePosition())->second; });

  // debug sorted base locations
  printf("Sorted base locations: \n");
  for (BWTA::BaseLocation *b : baseLocations)
    printf("dist from main: %f, pos x: %d, %d\n", BWTA::getGroundDistance(playerMain->getTilePosition(), b->getTilePosition()), b->getPosition().x, b->getPosition().y);

  ///////////////////////////////////////////////////////////////
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
  BWAPI::Position p;
  // we've found and destroyed main but the game hasn't ended; search; super super super hacky lazy way
  for (BWTA::BaseLocation *b : baseLocations)
  {
    if (!BWAPI::Broodwar->isExplored(b->getTilePosition()))
    {
      for (auto u : overlords)
      {
        //TODO: walk up to mineral not working as expected atm
        //overlords.back()->setObjective(Objective::REVEAL_TILE, b->getStaticMinerals().getPosition());
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

std::vector<BWTA::BaseLocation*> ScoutManager::getSortedBaseLocations()
{
  return baseLocations;
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