#include "BaseManager.h"

BaseManager::BaseManager(BWAPI::Unit &m, BWTA::BaseLocation *&l)
  : main(m)
  , location(l)
{
  workers = {};
  detectResources();
}

BaseManager::~BaseManager()
{
}

void BaseManager::update()
{
  for (const BWAPI::Unit &worker : workers)
  {
    if (worker->isIdle()) // workers freeze up without this?
    {
      // Order workers carrying a resource to return them to the center,
      // otherwise find a mineral patch to harvest.
      if (worker->isCarryingGas() || worker->isCarryingMinerals())
      {
        worker->returnCargo();
      }
      else if (!worker->getPowerUp()) // The worker cannot harvest anything if it
      {                          // is carrying a powerup such as a flag
        // Harvest from the nearest mineral patch or gas refinery
        if (!worker->gather(worker->getClosestUnit(BWAPI::Filter::IsMineralField || BWAPI::Filter::IsRefinery)))
        {
          // If the call fails, then print the last error message
          BWAPI::Broodwar << BWAPI::Broodwar->getLastError() << std::endl;
        }
      }
    }
  }

  //debug drawings
  for (auto& mineral : minerals) // red circles around basemanager minerals
  {
    BWAPI::Broodwar->registerEvent([mineral](BWAPI::Game*){ BWAPI::Broodwar->drawCircleMap(mineral->getInitialPosition(), 32, BWAPI::Colors::Red); },
      nullptr,
      BWAPI::Broodwar->getLatencyFrames());
  }
  BWAPI::Position &pos = main->getPosition(); // small red circle around hatch centerpoint
  BWAPI::Broodwar->registerEvent([pos](BWAPI::Game*){ BWAPI::Broodwar->drawCircleMap(pos, 15, BWAPI::Colors::Red); },
    nullptr,
    BWAPI::Broodwar->getLatencyFrames());
}
void BaseManager::addWorker(BWAPI::Unit u)
{
  workers.insert(u);
}
void BaseManager::removeWorker(BWAPI::Unit u)
{
  workers.erase(u);
}

bool BaseManager::containsWorker(BWAPI::Unit u)
{
  return (workers.count(u) == 1) ? true : false;
}
bool BaseManager::hasMain()
{
  return main->exists();
}
int BaseManager::numWorkers()
{
  return workers.size();
}
int BaseManager::remainingMinerals()
{
  int remainingMinerals = 0;

  for (const BWAPI::Unit &mineral : minerals)
  {
    remainingMinerals += mineral->getResources();
  }

  return remainingMinerals;
}
int BaseManager::remainingVespene()
{
  return vespene->getResources();
}

void BaseManager::detectResources()
{
  minerals = main->getUnitsInRadius(256, BWAPI::Filter::IsMineralField);

  for (const BWAPI::Unit &unit : main->getUnitsInRadius(256))
  {
    if (unit->getType() == BWAPI::UnitTypes::Resource_Vespene_Geyser)
    {
      vespene = unit;
      break;
    }
  }
}