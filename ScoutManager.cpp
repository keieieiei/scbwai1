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

}

void ScoutManager::addEnemyBase(BWAPI::Position pos)
{
  if (!enemyMain.isValid())
  {
    int minDistance = std::numeric_limits<int>::max();
    for (const auto &startingLocation : startingLocations)
    {
      if (pos.getDistance(startingLocation) < minDistance)
      {
        minDistance = pos.getApproxDistance(startingLocation);
        enemyMain = pos;
      }
    }
  }
}

void ScoutManager::addScout(BWAPI::Unit u)
{
  printf("addy addy\n");
  if (u->getType() == BWAPI::UnitTypes::Zerg_Drone)
  {
    drones.push_back(u);
    if (startingLocations.size() > 2)
      u->move(startingLocations[2]);
  }
  else if (u->getType() == BWAPI::UnitTypes::Zerg_Overlord)
  {
    printf("addy overlord\n");
    overlords.push_back(std::make_shared<OverlordHandler>(OverlordHandler(u)));
    overlords.back()->revealTile(startingLocations[1]);
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