#include "BuildExecutor.h"

BuildExecutor::BuildExecutor(BuildOrder bo)
  : buildOrder(builds.at(bo))
  , mineralReserve{0}
  , droneBetterBuildShitOrItsAFuckingMotherfucker{nullptr}
{

}

BuildExecutor::~BuildExecutor()
{

}

void BuildExecutor::update()
{
  if (buildOrder.empty())
    return;

  if (droneBetterBuildShitOrItsAFuckingMotherfucker != nullptr)
    if (droneBetterBuildShitOrItsAFuckingMotherfucker->getType() != BWAPI::UnitTypes::Zerg_Drone)
    {
      droneBetterBuildShitOrItsAFuckingMotherfucker = nullptr;
      mineralReserve = 0;
    }

  // get rid of bunk not larvae
  larvae.erase(std::remove_if(std::begin(larvae), std::end(larvae), [](const BWAPI::Unit &u){ return (!u->exists() || u->getType() != BWAPI::UnitTypes::Zerg_Larva); }), std::end(larvae));

  // TODO if we can suddenly build multiple things in one frame it'll only do one
  // doesnt do a lot of shitt zzz
  if (buildOrder.front().mineralPrice() + mineralReserve <= BWAPI::Broodwar->self()->minerals())
  {
    // if it building do building stuff (no failure check so RIP if we can't find placement location)
    if (buildOrder.front().isBuilding() && mainManager.lock()->numWorkers() > 0)
    {
      printf("try to build fekkin building %d\n", buildOrder.size());
      const BWAPI::Unit &u = mainManager.lock()->takeWorker();
      BWAPI::TilePosition buildPosition = BWAPI::Broodwar->getBuildLocation(buildOrder.front(), u->getTilePosition());
      if (u->build(buildOrder.front(), buildPosition))
      {
        mineralReserve = buildOrder.front().mineralPrice();
        buildOrder.pop();
        printf("build fekkin building %d\n", buildOrder.size());
        droneBetterBuildShitOrItsAFuckingMotherfucker = u;
      }
    }
    // if it not building do unitsy stuff i think maybe there's another condition if so RIP
    else if (larvae.size() > 0)
    {
      if (larvae.back()->morph(buildOrder.front()))
      {
        // build last thing infinitely
        if (buildOrder.size() != 1)
          buildOrder.pop();
        larvae.pop_back();
      }
    }
  }
}

bool BuildExecutor::containsLarva(BWAPI::Unit u)
{
  for (const auto &larva : larvae)
    if (larva == u)
      return true;
  return false;
}

void BuildExecutor::addLarva(BWAPI::Unit u)
{
  larvae.push_back(u);
}

void BuildExecutor::giveMainManager(const std::shared_ptr<BaseManager> &mm)
{
  mainManager = mm;
}

const std::unordered_map<BuildOrder, std::queue<BWAPI::UnitType>> BuildExecutor::builds =
{
  { 
    BuildOrder::FOURPOOL,
    std::queue<BWAPI::UnitType>
    ({
      BWAPI::UnitTypes::Zerg_Spawning_Pool, // 4 Pool
      BWAPI::UnitTypes::Zerg_Drone,         // 3 Drone
      BWAPI::UnitTypes::Zerg_Zergling       // 4 Zerglings -> infinity
    })
  },

  {
    BuildOrder::FIVEPOOL,
    std::queue<BWAPI::UnitType>
    ({
      BWAPI::UnitTypes::Zerg_Drone,         // 4 Drone
      BWAPI::UnitTypes::Zerg_Spawning_Pool, // 5 Pool
      BWAPI::UnitTypes::Zerg_Drone,         // 4 Drone
      BWAPI::UnitTypes::Zerg_Drone,         // 5 Drone
      BWAPI::UnitTypes::Zerg_Zergling,      // 6 Lings -> infinity + an overlord
      BWAPI::UnitTypes::Zerg_Zergling,
      BWAPI::UnitTypes::Zerg_Zergling,
      BWAPI::UnitTypes::Zerg_Overlord,
      BWAPI::UnitTypes::Zerg_Zergling
    })
  },

  {
    BuildOrder::NINEPOOL,
    std::queue<BWAPI::UnitType>
    ({
      BWAPI::UnitTypes::Zerg_Drone,         // 4 Drone
      BWAPI::UnitTypes::Zerg_Drone,         // 5 Drone
      BWAPI::UnitTypes::Zerg_Drone,         // 6 Drone
      BWAPI::UnitTypes::Zerg_Drone,         // 7 Drone
      BWAPI::UnitTypes::Zerg_Drone,         // 8 Drone
      BWAPI::UnitTypes::Zerg_Spawning_Pool, // 9 Pool
      BWAPI::UnitTypes::Zerg_Drone,         // 8 Drone
      BWAPI::UnitTypes::Zerg_Overlord,      // 9 Overlord
      BWAPI::UnitTypes::Zerg_Drone,         // 9 Drone
      BWAPI::UnitTypes::Zerg_Zergling       // 10 Lings -> infinity
    })
  }
};


