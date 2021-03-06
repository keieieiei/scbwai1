#include "BaseManager.h"

BaseManager::BaseManager(const BWAPI::Unit unit)
  : main{ unit }
  , vespene{ nullptr }
{
  // Find and add surrounding resources
  // TODO: use BWTA ?
  for (const auto u : main->getUnitsInRadius(256, BWAPI::Filter::IsResourceContainer))
  {
    if (u->getType().isMineralField())
    {
      minerals.push_back(std::make_shared<Mineral>(Mineral{ u, 0, 0 }));
    }
    else if (u->getType() == BWAPI::UnitTypes::Resource_Vespene_Geyser)
    {
      vespene = u;
    }
    // TODO: what if u->getType().isRefinery()
  }
}

BaseManager::~BaseManager() {}

void BaseManager::update()
{
  // check if we have to refresh our vespene pointer
  // and mark it as run out if it has
  // nullptr means if vespene never existed or ran out.
  if (vespene != nullptr)
  {
    if (!vespene->exists())
    {
      for (const auto u : main->getUnitsInRadius(256, BWAPI::Filter::IsResourceContainer)) // TODO: check if this includes Refineries
        if (u->getType() == BWAPI::UnitTypes::Resource_Vespene_Geyser)
          vespene = u;
    }
    else // vespene->exists()
      if (vespene->getResources() == 0)
        vespene = nullptr;     
  }

  // Remove minerals that don't exist
  minerals.erase(std::remove_if(std::begin(minerals), std::end(minerals), [](const std::shared_ptr<Mineral> &m){ return !m->unit->exists(); }), std::end(minerals));

  // Remove worker pointers that expired
  drones.erase(std::remove_if(std::begin(drones), std::end(drones), [](const std::weak_ptr<DroneHandler> &d){ return d.expired(); }), std::end(drones));

  // check for workers that lost their minerals to the previous statement
  //  then check for workers that don't exist and update their mineral if they contribute to a mineral count
  // Note: this might be an invariant that says we should implement mineral as a class
  for (auto &drone : drones)
  {
    // Update worker if they're about to get removed and contribute to a mineral's count
    if (!drone.lock()->unit->exists() && (drone.lock()->getGatherState() == GatherState::MOVE_TO_MINERALS || drone.lock()->getGatherState() == GatherState::WAIT_FOR_MINERALS))
      for (auto &mineral : minerals)
        if (mineral->unit == drone.lock()->getTargetUnit())
        {
          if (mineral->numWorkers > 0)
            --mineral->numWorkers;

          continue;
        }
  }

  // Remove workers that don't exist
  drones.erase(std::remove_if(std::begin(drones), std::end(drones), [](const std::weak_ptr<DroneHandler> &d){ return !d.lock()->unit->exists(); }), std::end(drones));

  // Check if we've completely run out of minerals or don't have a hatchery (prevents some bad pointer calls)
  // TODO: fully implement this
  if (minerals.size() == 0 || !main->exists())
  {
    return;
  }

  // Update Mineral initialGatherFrame tracker if it's not locked by a gatherer
  for (const auto &mineral : minerals)
    if (!mineral->unit->isBeingGathered())
      mineral->initialGatherFrame = BWAPI::Broodwar->getFrameCount();


  // Manage Workers
  for (const auto &drone : drones)
  {
    // DEBUG
    BWAPI::Broodwar->drawTextMap(drone.lock()->unit->getPosition(), "%d", drone.lock()->getGatherState());

    /*
    switch (worker.state)
    {
    case Worker::MOVING_TO_GATHER:
      // TODO: check if workers are swapping targets before getting near their target mineral
      if (worker.unit->getDistance(worker.resource.lock()->unit->getPosition()) < 5) // test how far this needs to be >0
        worker.state = Worker::AT_GATHER_POINT;
    case Worker::AT_GATHER_POINT:
      // TODO: what if worker is carrying something but not moving to return it?
      // Update mineral info and change worker state when it's finished gathering
      //  should not trigger from previous case
      if ((BWAPI::Filter::IsCarryingSomething)(worker.unit))
      {
        worker.state = Worker::RETURNING_RESOURCE;
        worker.resource.lock()->initialGatherFrame = BWAPI::Broodwar->getFrameCount();
        --worker.resource.lock()->numWorkers;
      }
      // ensure worker is focused on assigned mineral
      else if (worker.unit->getOrderTarget() != worker.resource.lock()->unit)
        worker.unit->gather(worker.resource.lock()->unit);
      break;
    case Worker::RETURNING_RESOURCE:
      if ((BWAPI::Filter::IsCarryingSomething)(worker.unit))
        break;
      // if the worker just delivered its resources
      worker.resource = std::weak_ptr<Mineral>(); // just in case
    case Worker::DEFAULT:
      // find the most optimal mineral patch for that worker to go to
      int minFrames = std::numeric_limits<int>::max();
      for (const auto &mineral : minerals)
      {
        int totalFrames = 0;

        // lookup how many frames it will take us to get to the patch
        int distance = worker.unit->getDistance(mineral->unit->getPosition());
        if (distance <= moveDistanceMax)
          totalFrames += moveToMineralFrames.at(distance);
        else
          totalFrames += static_cast<int>(static_cast<double>(distance) / moveDistanceMax * moveToMineralFrames.at(moveDistanceMax));

        // if the number of frames until we can start gathering is longer than frame to get to the patch
        //  then we'll just use that instead
        if (mineral->numWorkers > 0)
        {
          int waitFrames = mineral->numWorkers * gatherFrames - (BWAPI::Broodwar->getFrameCount() - mineral->initialGatherFrame);
          totalFrames = (totalFrames > waitFrames) ? totalFrames : waitFrames;
        }

        // Re-use distance
        distance = main->getDistance(mineral->unit->getPosition());
        // add the number of frames it will take us to return to the hatchery with our goods
        if (distance >= returnDistanceMin && distance <= returnDistanceMax)
          totalFrames += returnToMainFrames.at(distance);
        else if (distance > returnDistanceMax)
          totalFrames += static_cast<int>(static_cast<double>(distance) / returnDistanceMax * returnToMainFrames.at(returnDistanceMax));
        else // shouldn't happen in normal maps
          totalFrames += returnToMainFrames.at(returnDistanceMin);

        // swap to this Mineral if its better
        if (totalFrames < minFrames)
        {
          minFrames = totalFrames;
          worker.resource = mineral;
        }
      }
      // Update chosen mineral's count and workers state
      worker.state = Worker::MOVING_TO_GATHER;
      ++worker.resource.lock()->numWorkers;
      break;

    }
    */
  }
}

int BaseManager::numWorkers()
{
  return drones.size();
}

bool BaseManager::containsWorker(BWAPI::Unit unit)
{
  for (const auto &drone : drones)
    if (drone.lock()->unit == unit)
      return true;

  return false;
}

void BaseManager::addWorker(std::shared_ptr<UnitHandler> dh, std::shared_ptr<BaseManager> bm)
{
  drones.push_back(std::dynamic_pointer_cast<DroneHandler>(dh));
  drones.back().lock()->giveBaseManager(bm);
  drones.back().lock()->setObjective(Objective::GATHER_MINERALS, nullptr);
}

BWAPI::Unit BaseManager::takeWorker()
{
  printf("takeworker worker size %d", drones.size());
  if (drones.size() < 1)
    return nullptr;

  // Sort in reverse order of GatherState value: GAS, RETURNING_MINERALS, WAIT_FOR_MINERALS, MOVE_TO_MINERALS, DEFAULT
  std::sort(std::begin(drones), std::end(drones), [](const std::weak_ptr<DroneHandler> &a, const std::weak_ptr<DroneHandler> &b) { return a.lock()->getGatherState() > b.lock()->getGatherState(); });

  // Return the last element (hopefully DEFAULT or MOVING_TO_GATHER)
  if (drones.back().lock()->getGatherState() == GatherState::MOVE_TO_MINERALS || drones.back().lock()->getGatherState() == GatherState::WAIT_FOR_MINERALS)
    for (auto &mineral : minerals)
      if (mineral->unit == drones.back().lock()->getTargetUnit())
      {
        if (mineral->numWorkers > 0)
          --mineral->numWorkers;

        continue;
      }

  BWAPI::Unit unit = drones.back().lock()->unit;
  drones.back().lock()->resetObjective();
  drones.pop_back();
  return unit;
}

void BaseManager::finishGathering(BWAPI::Unit u)
{
  for (auto &mineral : minerals)
    if (mineral->unit == u)
    {
      mineral->initialGatherFrame = BWAPI::Broodwar->getFrameCount();
      if (mineral->numWorkers > 0)
        --mineral->numWorkers;

      continue;
    }
}

BWAPI::Unit BaseManager::requestMineralAssignment(BWAPI::Position pos)
{
  // TODO:  more robust way to set / choose gas gathering
  // set target as vespene if we have a vespene up and 2 or less gatherers
  //  .... the dronehandler will change the objective... fuck... this is messy
  if (vespene != nullptr)
    if (vespene->exists() && vespene->isCompleted() && vespene->getType() == BWAPI::UnitTypes::Zerg_Extractor && vespene->getPlayer() == BWAPI::Broodwar->self())
    {
      int numGasGatherers = 0;
      for (const auto &drone : drones)
        if (!drone.expired())
          if (drone.lock()->getObjective() == Objective::GATHER_GAS)
            ++numGasGatherers;

      if (numGasGatherers < 3)
        return vespene;
    }

  int minFrames = std::numeric_limits<int>::max();
  BWAPI::Unit returnMineral = nullptr;
  for (const auto &mineral : minerals)
  {
    int totalFrames = 0;

    // lookup how many frames it will take us to get to the patch
    int distance = mineral->unit->getDistance(pos);
    if (distance <= moveDistanceMax)
      totalFrames += moveToMineralFrames.at(distance);
    else
      totalFrames += static_cast<int>(static_cast<double>(distance) / moveDistanceMax * moveToMineralFrames.at(moveDistanceMax));

    // if the number of frames until we can start gathering is longer than frame to get to the patch
    //  then we'll just use that instead
    if (mineral->numWorkers > 0)
    {
      int waitFrames = mineral->numWorkers * gatherFrames - (BWAPI::Broodwar->getFrameCount() - mineral->initialGatherFrame);
      totalFrames = (totalFrames > waitFrames) ? totalFrames : waitFrames;
    }

    // Re-use distance
    distance = main->getDistance(mineral->unit->getPosition());
    // add the number of frames it will take us to return to the hatchery with our goods
    if (distance >= returnDistanceMin && distance <= returnDistanceMax)
      totalFrames += returnToMainFrames.at(distance);
    else if (distance > returnDistanceMax)
      totalFrames += static_cast<int>(static_cast<double>(distance) / returnDistanceMax * returnToMainFrames.at(returnDistanceMax));
    else // shouldn't happen in normal maps
      totalFrames += returnToMainFrames.at(returnDistanceMin);

    // swap to this Mineral if its better
    if (totalFrames < minFrames)
    {
      minFrames = totalFrames;
      returnMineral = mineral->unit;
    }
  }

  for (auto &mineral : minerals)
    if (mineral->unit == returnMineral)
    {
      ++mineral->numWorkers;
      continue;
    }

  return returnMineral;
}

const int BaseManager::gatherFrames = 80;

// TODO properly initialize these two lookup tables
const int BaseManager::moveDistanceMax = 256; // ending key for move to mineral distance->frames lookup table
// distance->frames lookup table:  intialize keys from 0 to moveDistanceMax, inclusive
const std::unordered_map<int, int> BaseManager::moveToMineralFrames =
{
  { 0, 0 }, { 1, 0 }, { 2, 0 }, { 3, 0 }, { 4, 0 }, { 5, 1 }, { 6, 1 }, { 7, 1 }, { 8, 1 }, { 9, 1 },
  { 10, 2 }, { 11, 2 }, { 12, 2 }, { 13, 2 }, { 14, 2 }, { 15, 3 }, { 16, 3 }, { 17, 3 }, { 18, 3 }, { 19, 3 },
  { 20, 4 }, { 21, 4 }, { 22, 4 }, { 23, 4 }, { 24, 4 }, { 25, 5 }, { 26, 5 }, { 27, 5 }, { 28, 5 }, { 29, 5 },
  { 30, 6 }, { 31, 6 }, { 32, 6 }, { 33, 6 }, { 34, 6 }, { 35, 7 }, { 36, 7 }, { 37, 7 }, { 38, 7 }, { 39, 7 },
  { 40, 8 }, { 41, 8 }, { 42, 8 }, { 43, 8 }, { 44, 8 }, { 45, 9 }, { 46, 9 }, { 47, 9 }, { 48, 9 }, { 49, 9 },
  { 50, 10 }, { 51, 10 }, { 52, 10 }, { 53, 10 }, { 54, 10 }, { 55, 11 }, { 56, 11 }, { 57, 11 }, { 58, 11 }, { 59, 11 },
  { 60, 12 }, { 61, 12 }, { 62, 12 }, { 63, 12 }, { 64, 12 }, { 65, 13 }, { 66, 13 }, { 67, 13 }, { 68, 13 }, { 69, 13 },
  { 70, 14 }, { 71, 14 }, { 72, 14 }, { 73, 14 }, { 74, 14 }, { 75, 15 }, { 76, 15 }, { 77, 15 }, { 78, 15 }, { 79, 15 },
  { 80, 16 }, { 81, 16 }, { 82, 16 }, { 83, 16 }, { 84, 16 }, { 85, 17 }, { 86, 17 }, { 87, 17 }, { 88, 17 }, { 89, 17 },
  { 90, 18 }, { 91, 18 }, { 92, 18 }, { 93, 18 }, { 94, 18 }, { 95, 19 }, { 96, 19 }, { 97, 19 }, { 98, 19 }, { 99, 19 },
  { 100, 20 }, { 101, 20 }, { 102, 20 }, { 103, 20 }, { 104, 20 }, { 105, 21 }, { 106, 21 }, { 107, 21 }, { 108, 21 }, { 109, 21 },
  { 110, 22 }, { 111, 22 }, { 112, 22 }, { 113, 22 }, { 114, 22 }, { 115, 23 }, { 116, 23 }, { 117, 23 }, { 118, 23 }, { 119, 23 },
  { 120, 24 }, { 121, 24 }, { 122, 24 }, { 123, 24 }, { 124, 24 }, { 125, 25 }, { 126, 25 }, { 127, 25 }, { 128, 25 }, { 129, 25 },
  { 130, 26 }, { 131, 26 }, { 132, 26 }, { 133, 26 }, { 134, 26 }, { 135, 27 }, { 136, 27 }, { 137, 27 }, { 138, 27 }, { 139, 27 },
  { 140, 28 }, { 141, 28 }, { 142, 28 }, { 143, 28 }, { 144, 28 }, { 145, 29 }, { 146, 29 }, { 147, 29 }, { 148, 29 }, { 149, 29 },
  { 150, 30 }, { 151, 30 }, { 152, 30 }, { 153, 30 }, { 154, 30 }, { 155, 31 }, { 156, 31 }, { 157, 31 }, { 158, 31 }, { 159, 31 },
  { 160, 32 }, { 161, 32 }, { 162, 32 }, { 163, 32 }, { 164, 32 }, { 165, 33 }, { 166, 33 }, { 167, 33 }, { 168, 33 }, { 169, 33 },
  { 170, 34 }, { 171, 34 }, { 172, 34 }, { 173, 34 }, { 174, 34 }, { 175, 35 }, { 176, 35 }, { 177, 35 }, { 178, 35 }, { 179, 35 },
  { 180, 36 }, { 181, 36 }, { 182, 36 }, { 183, 36 }, { 184, 36 }, { 185, 37 }, { 186, 37 }, { 187, 37 }, { 188, 37 }, { 189, 37 },
  { 190, 38 }, { 191, 38 }, { 192, 38 }, { 193, 38 }, { 194, 38 }, { 195, 39 }, { 196, 39 }, { 197, 39 }, { 198, 39 }, { 199, 39 },
  { 200, 40 }, { 201, 40 }, { 202, 40 }, { 203, 40 }, { 204, 40 }, { 205, 41 }, { 206, 41 }, { 207, 41 }, { 208, 41 }, { 209, 41 },
  { 210, 42 }, { 211, 42 }, { 212, 42 }, { 213, 42 }, { 214, 42 }, { 215, 43 }, { 216, 43 }, { 217, 43 }, { 218, 43 }, { 219, 43 },
  { 220, 44 }, { 221, 44 }, { 222, 44 }, { 223, 44 }, { 224, 44 }, { 225, 45 }, { 226, 45 }, { 227, 45 }, { 228, 45 }, { 229, 45 },
  { 230, 46 }, { 231, 46 }, { 232, 46 }, { 233, 46 }, { 234, 46 }, { 235, 47 }, { 236, 47 }, { 237, 47 }, { 238, 47 }, { 239, 47 },
  { 240, 48 }, { 241, 48 }, { 242, 48 }, { 243, 48 }, { 244, 48 }, { 245, 49 }, { 246, 49 }, { 247, 49 }, { 248, 49 }, { 249, 49 },
  { 250, 50 }, { 251, 50 }, { 252, 50 }, { 253, 50 }, { 254, 50 }, { 255, 51 }, { 256, 51 }, { 257, 51 }, { 258, 51 }, { 259, 51 }
};

const int BaseManager::returnDistanceMin = 50;  // Starting key for return to main distance->frames lookup table
const int BaseManager::returnDistanceMax = 256; // Ending key for return to main distance->frames lookup table
// distance->frames lookup table: intialize keys from returnDistanceMin to returnDistanceMax, inclusive
const std::unordered_map<int, int> BaseManager::returnToMainFrames =
{
  { 0, 0 }, { 1, 0 }, { 2, 0 }, { 3, 0 }, { 4, 0 }, { 5, 1 }, { 6, 1 }, { 7, 1 }, { 8, 1 }, { 9, 1 },
  { 10, 2 }, { 11, 2 }, { 12, 2 }, { 13, 2 }, { 14, 2 }, { 15, 3 }, { 16, 3 }, { 17, 3 }, { 18, 3 }, { 19, 3 },
  { 20, 4 }, { 21, 4 }, { 22, 4 }, { 23, 4 }, { 24, 4 }, { 25, 5 }, { 26, 5 }, { 27, 5 }, { 28, 5 }, { 29, 5 },
  { 30, 6 }, { 31, 6 }, { 32, 6 }, { 33, 6 }, { 34, 6 }, { 35, 7 }, { 36, 7 }, { 37, 7 }, { 38, 7 }, { 39, 7 },
  { 40, 8 }, { 41, 8 }, { 42, 8 }, { 43, 8 }, { 44, 8 }, { 45, 9 }, { 46, 9 }, { 47, 9 }, { 48, 9 }, { 49, 9 },
  { 50, 10 }, { 51, 10 }, { 52, 10 }, { 53, 10 }, { 54, 10 }, { 55, 11 }, { 56, 11 }, { 57, 11 }, { 58, 11 }, { 59, 11 },
  { 60, 12 }, { 61, 12 }, { 62, 12 }, { 63, 12 }, { 64, 12 }, { 65, 13 }, { 66, 13 }, { 67, 13 }, { 68, 13 }, { 69, 13 },
  { 70, 14 }, { 71, 14 }, { 72, 14 }, { 73, 14 }, { 74, 14 }, { 75, 15 }, { 76, 15 }, { 77, 15 }, { 78, 15 }, { 79, 15 },
  { 80, 16 }, { 81, 16 }, { 82, 16 }, { 83, 16 }, { 84, 16 }, { 85, 17 }, { 86, 17 }, { 87, 17 }, { 88, 17 }, { 89, 17 },
  { 90, 18 }, { 91, 18 }, { 92, 18 }, { 93, 18 }, { 94, 18 }, { 95, 19 }, { 96, 19 }, { 97, 19 }, { 98, 19 }, { 99, 19 },
  { 100, 20 }, { 101, 20 }, { 102, 20 }, { 103, 20 }, { 104, 20 }, { 105, 21 }, { 106, 21 }, { 107, 21 }, { 108, 21 }, { 109, 21 },
  { 110, 22 }, { 111, 22 }, { 112, 22 }, { 113, 22 }, { 114, 22 }, { 115, 23 }, { 116, 23 }, { 117, 23 }, { 118, 23 }, { 119, 23 },
  { 120, 24 }, { 121, 24 }, { 122, 24 }, { 123, 24 }, { 124, 24 }, { 125, 25 }, { 126, 25 }, { 127, 25 }, { 128, 25 }, { 129, 25 },
  { 130, 26 }, { 131, 26 }, { 132, 26 }, { 133, 26 }, { 134, 26 }, { 135, 27 }, { 136, 27 }, { 137, 27 }, { 138, 27 }, { 139, 27 },
  { 140, 28 }, { 141, 28 }, { 142, 28 }, { 143, 28 }, { 144, 28 }, { 145, 29 }, { 146, 29 }, { 147, 29 }, { 148, 29 }, { 149, 29 },
  { 150, 30 }, { 151, 30 }, { 152, 30 }, { 153, 30 }, { 154, 30 }, { 155, 31 }, { 156, 31 }, { 157, 31 }, { 158, 31 }, { 159, 31 },
  { 160, 32 }, { 161, 32 }, { 162, 32 }, { 163, 32 }, { 164, 32 }, { 165, 33 }, { 166, 33 }, { 167, 33 }, { 168, 33 }, { 169, 33 },
  { 170, 34 }, { 171, 34 }, { 172, 34 }, { 173, 34 }, { 174, 34 }, { 175, 35 }, { 176, 35 }, { 177, 35 }, { 178, 35 }, { 179, 35 },
  { 180, 36 }, { 181, 36 }, { 182, 36 }, { 183, 36 }, { 184, 36 }, { 185, 37 }, { 186, 37 }, { 187, 37 }, { 188, 37 }, { 189, 37 },
  { 190, 38 }, { 191, 38 }, { 192, 38 }, { 193, 38 }, { 194, 38 }, { 195, 39 }, { 196, 39 }, { 197, 39 }, { 198, 39 }, { 199, 39 },
  { 200, 40 }, { 201, 40 }, { 202, 40 }, { 203, 40 }, { 204, 40 }, { 205, 41 }, { 206, 41 }, { 207, 41 }, { 208, 41 }, { 209, 41 },
  { 210, 42 }, { 211, 42 }, { 212, 42 }, { 213, 42 }, { 214, 42 }, { 215, 43 }, { 216, 43 }, { 217, 43 }, { 218, 43 }, { 219, 43 },
  { 220, 44 }, { 221, 44 }, { 222, 44 }, { 223, 44 }, { 224, 44 }, { 225, 45 }, { 226, 45 }, { 227, 45 }, { 228, 45 }, { 229, 45 },
  { 230, 46 }, { 231, 46 }, { 232, 46 }, { 233, 46 }, { 234, 46 }, { 235, 47 }, { 236, 47 }, { 237, 47 }, { 238, 47 }, { 239, 47 },
  { 240, 48 }, { 241, 48 }, { 242, 48 }, { 243, 48 }, { 244, 48 }, { 245, 49 }, { 246, 49 }, { 247, 49 }, { 248, 49 }, { 249, 49 },
  { 250, 50 }, { 251, 50 }, { 252, 50 }, { 253, 50 }, { 254, 50 }, { 255, 51 }, { 256, 51 }, { 257, 51 }, { 258, 51 }, { 259, 51 }
};