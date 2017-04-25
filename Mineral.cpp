#include "Mineral.h"

Mineral::Mineral(const BWAPI::Unit &u)
  : unit(u)
  , gatherFrames(80) // TODO: is there a better way to get a value for this?
  , initialGatherFrame(0)
  , averageReturnFrames(0)
  , numReturns(0)
  , numGatherFrames(0)
  , numGatherFramesMax(0)
  , numGatherFramesMin(100)
  , numTrips(0)
  , averageTrip(0)
{

}

Mineral::~Mineral()
{

}

void Mineral::update()
{
  BWAPI::Broodwar->drawTextScreen(250, 170, "Mineral Update: %d", workers.size());
  if (BWAPI::Broodwar->getFrameCount() % 100 == 0)
  {
    printf("Mineral Update: %d\n", workers.size());
  }
  if (workers.size() > 0)
  {
    /* pretty sure we won't need this with drone tracking its own shit, BaseManager should handle the rest
    // if we've finished following the last drone that left, check to see if there's another
    if (returningWorker == nullptr)
    {
    for (const Drone& drone : workers)
    {
    if (drone.unit->isCarryingMinerals())
    {
    returningWorker = &drone;
    initialReturnFrame = BWAPI::Broodwar->getFrameCount();
    break;
    }
    }
    }
    */

    // flag all drones that are leaving
    // TODO: maybe move this logic to basemanager
    /*
    for (Drone &drone : workers)
    {
    if (drone.unit->isCarryingMinerals())
    {
    drone.isAtMinerals = false;
    drone.isReturning = true;
    drone.initialReturnFrame = BWAPI::Broodwar->getFrameCount();
    printf("drone leaving: %d\n", workers.size());
    // Note: Could set the d.mineral pointer to nullptr here
    //   but we want to associate the return frames with this Mineral
    }
    }
    */
    for (Drone *drone : workers)
    {
      int pos_x = unit->getPosition().x;
      int pos_y = unit->getPosition().y - 10;
      BWAPI::Broodwar->drawTextMap(pos_x, pos_y, "%d : %d : %d", drone->isMovingToPatch, drone->isAtMinerals, drone->isReturning);

      BWAPI::Broodwar->drawTextScreen(250, 180, "Mineral Worker Loop: %d : %d : %d", drone->isMovingToPatch, drone->isAtMinerals, drone->isReturning);
      if (BWAPI::Broodwar->getFrameCount() % 100 == 0)
      {
        printf("Mineral Worker Loop: %d : %d : %d\n", drone->isMovingToPatch, drone->isAtMinerals, drone->isReturning);
      }
    }

    // clear all drones BaseManager has set to leaving
    workers.erase(std::remove_if(workers.begin(), workers.end(), [](Drone *d) { return d->isReturning; }), workers.end());


    /* same as above, shouldn't need if drone & basemanager handle it
    // check if we're following a worker and it has reached the hatchery
    // update our tracking things if so and stop following it
    if (returningWorker != nullptr && !returningWorker->unit->isCarryingMinerals()) // Note: do we have to check for nullptr?
    {
    int returnFrames = BWAPI::Broodwar->getFrameCount() - initialReturnFrame;
    ++numReturns;
    averageReturnFrames = (averageReturnFrames * (numReturns - 1) + returnFrames) / numReturns;
    returningWorker = nullptr;
    }
    */

    // Make sure all Drones queued up to mine here keep their focus
    // TODO: may have to change to a Event thing if this doesn't update every frame
    // TODO: this isn't enough to force them to stay on the patch 
    for (Drone *drone : workers)
    {
      //drone.unit->getLastCommandFrame();
      //drone.unit->isInterruptible();
      // if drone is trying to go somewhere else
      if (drone->unit->getOrderTarget() != drone->mineral->unit)
      {
        //force it to gather here again 
        if (!drone->unit->gather(drone->mineral->unit))
        {
          // If the call fails, then print the last error message
          BWAPI::Broodwar << BWAPI::Broodwar->getLastError() << std::endl;
        }
      }
    }

  }




  // TODO: has to be a better way to manage this?
  // Reset initial gather frame
  if (!unit->isBeingGathered())
  {
    initialGatherFrame = BWAPI::Broodwar->getFrameCount();

    if (numGatherFrames > 0 && numGatherFrames < 100)
    {
      ++numTrips;
      averageTrip = (averageTrip * (numTrips - 1) + numGatherFrames) / numTrips;
    }
  }

  // TESTING
  // count for gather frames
  if (unit->isBeingGathered())
  {
    numGatherFrames = BWAPI::Broodwar->getFrameCount() - initialGatherFrame;
  }
  // update max/min when numGatherFrames is not changing
  else
  {
    if (numGatherFrames < 100)
    {
      numGatherFramesMax = (numGatherFrames > numGatherFramesMax) ? numGatherFrames : numGatherFramesMax;
    }
    if (numGatherFrames != 0)
    {
      numGatherFramesMin = (numGatherFrames < numGatherFramesMin) ? numGatherFrames : numGatherFramesMin;
    }
  }
  // print results
  if (BWAPI::Broodwar->getFrameCount() % 100 == 0)
  {
    printf("Minerals Drawing Thing: %.1lf : %d | %d\n", averageReturnFrames, numReturns, workers.size());
  }
  BWAPI::Position pos = unit->getPosition();
  BWAPI::Broodwar->drawTextMap(pos, "%.1lf : %d | %d", averageReturnFrames, numReturns, workers.size());
}

void Mineral::addWorker(Drone *d)
{
  workers.push_back(d);
  // DEBUG getting added in every frame
  BWAPI::Broodwar->drawTextScreen(250, 190, "Mineral Drone Add Argument: %d : %d : %d | %d", d->isMovingToPatch, d->isAtMinerals, d->isReturning, workers.size());
  if (BWAPI::Broodwar->getFrameCount() % 100 == 0)
  {
    printf("Mineral Drone Add Argument: %d : %d : %d | %d | %p\n", d->isMovingToPatch, d->isAtMinerals, d->isReturning, workers.size(), d);
    printf("Mineral Drone Add In Vector: %d : %d : %d | %d| %p\n", workers.back()->isMovingToPatch, workers.back()->isAtMinerals, workers.back()->isReturning, workers.size(), workers.back());
  }
}

int Mineral::remainingGatherFrames()
{
  // Note: Assume if we're not being gathered (getFrameCount() == initialGatherFrame()) then whatever workers we haved queued up will soon be beginning
  return gatherFrames * workers.size() - (BWAPI::Broodwar->getFrameCount() - initialGatherFrame);
}

int Mineral::getAverageReturnFrames()
{
  return averageReturnFrames;
}

int Mineral::timeToBeginMining()
{
  // TODO: implement drone->mineral patch movement time calculation
  //   currently substituting mineral patch -> hatch time instead
  //   should add a BWAPI::Position parameter when we implement  
  return std::max(static_cast<int>(averageReturnFrames), remainingGatherFrames()) + averageReturnFrames;
}

void Mineral::calculateReturnFrames(int frames)
{
  // DEBUG
  printf("calculateReturnFrames(%d) | %d : %.1lf\n", frames, numReturns, averageReturnFrames);
  ++numReturns;
  averageReturnFrames = (averageReturnFrames * (numReturns - 1) + frames) / numReturns;
}