#include "BaseManager.h"

BaseManager::BaseManager(const BWAPI::Unit &m, BWTA::BaseLocation *&l)
  : main(m)
  , location(l)
{
  drones.reserve(5);
  detectResources();
  sortedMinerals.reserve(minerals.size());
  freeMinerals.reserve(minerals.size());
  mineralsQueue.reserve(minerals.size());
  isGatherFrameCount.resize(minerals.size(), 0);
}

BaseManager::~BaseManager()
{
  for (Mineral *mineral : minerals)
  {
    delete mineral;
    mineral = nullptr;
  }
}

void BaseManager::update()
{
  // TODO:  check for drones with units that no longer exist
  for (const Drone &drone : drones)
  {
    if (drone.unit->isIdle()) // workers freeze up without this?
    {
      // Order workers carrying a resource to return them to the center,
      // otherwise find a mineral patch to harvest.
      if (drone.unit->isCarryingGas() || drone.unit->isCarryingMinerals())
      {
        drone.unit->returnCargo();
      }
      else if (!drone.unit->getPowerUp()) // The worker cannot harvest anything if it
      {                          // is carrying a powerup such as a flag
        // Harvest from the nearest mineral patch or gas refinery
        if (!drone.unit->gather(drone.unit->getClosestUnit(BWAPI::Filter::IsMineralField || BWAPI::Filter::IsRefinery)))
        {
          // If the call fails, then print the last error message
          BWAPI::Broodwar << BWAPI::Broodwar->getLastError() << std::endl;
        }
      }
    }

    // Note: passing references into the lambda capture for drawing causes crashes sometimes ???
    BWAPI::Position &pos = drone.unit->getPosition(); //main->getPosition();
    BWAPI::Position &pos_b = drone.unit->getTargetPosition();//  minerals.front()->unit->getPosition();
    // worker target line  ?? apparently this causes a crash at the same time every time
    if (true)//drone.unit->getPosition() != NULL && drone.unit->getTarget() != nullptr)
    {
      /*BWAPI::Broodwar->registerEvent([pos, pos_b](BWAPI::Game*){ */BWAPI::Broodwar->drawLineMap( pos, pos_b /*drone.unit->getTargetPosition()*/, BWAPI::Colors::Green);/* },
        nullptr,
        BWAPI::Broodwar->getLatencyFrames()); */
    }
    
    // worker command info
    //BWAPI::Position &pos = worker->getPosition();
    //int since = BWAPI::Broodwar->getFrameCount() - worker->getLastCommandFrame(); // Note: lambda flicker draws one worker if you capture &pos reference to pos??
    //BWAPI::Broodwar->registerEvent([&worker, pos, since](BWAPI::Game*){ BWAPI::Broodwar->drawTextMap(pos, "%s since %d", worker->getLastCommand().type.getName().c_str(), since); }, // Note: .c_str() alone doesn't work, has to be in "%s" for some reason
    //  nullptr,
    //  BWAPI::Broodwar->getLatencyFrames());
  }
  //debug drawings
  for (Mineral *&mineral : minerals) // red circles around basemanager minerals
  {
    mineral->update();
    BWAPI::Broodwar->registerEvent([mineral](BWAPI::Game*){ BWAPI::Broodwar->drawCircleMap(mineral->unit->getInitialPosition(), 32, BWAPI::Colors::Red); },
      nullptr,
      BWAPI::Broodwar->getLatencyFrames());
  }
  BWAPI::Position &pos = main->getPosition(); // small red circle around hatch centerpoint
  BWAPI::Broodwar->registerEvent([pos](BWAPI::Game*){ BWAPI::Broodwar->drawCircleMap(pos, 15, BWAPI::Colors::Red); },
    nullptr,
    BWAPI::Broodwar->getLatencyFrames());
}
void BaseManager::addWorker(const BWAPI::Unit &u)
{
  drones.push_back(Drone(u));
}
BWAPI::Unit BaseManager::takeWorker()
{
  // TODO: use a different data structure for Drones?
  // TODO: better unit taking logic
  // TODO: Potential Issue: How to handle when we have no workers?
  if (drones.size() == 0)
  {
    return nullptr;
  }
  Drone &d = drones.back();
  BWAPI::Unit &u = d.unit;
  drones.pop_back();
  return u; 
}

bool BaseManager::containsWorker(const Drone &d)
{
  return std::find(drones.begin(), drones.end(), d) != drones.end();
}
bool BaseManager::hasMain()
{
  return main->exists();
}
int BaseManager::numWorkers()
{
  return drones.size();
}
int BaseManager::remainingMinerals()
{
  int remainingMinerals = 0;

  for (Mineral *&mineral : minerals)
  {
    remainingMinerals += mineral->unit->getResources();
  }

  return remainingMinerals;
}
int BaseManager::remainingVespene()
{
  return vespene->getResources();
}

void BaseManager::detectResources()
{
  // TODO: probably can use BWTA for this
  for (const BWAPI::Unit &u : main->getUnitsInRadius(256, BWAPI::Filter::IsMineralField))
  {
    minerals.push_back(new Mineral(u));
  }

  for (const BWAPI::Unit &unit : main->getUnitsInRadius(256))
  {
    if (unit->getType() == BWAPI::UnitTypes::Resource_Vespene_Geyser)
    {
      vespene = unit;
      break;
    }
  }
}