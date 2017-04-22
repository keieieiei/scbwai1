#include "Mineral.h"

Mineral::Mineral(const BWAPI::Unit &u)
  : unit(u)
  , gatherFrames(80) // TODO: is there a better way to get a value for this?
  , initialGatherFrame(0)
  , initialReturnFrame(0)
  , averageReturnFrames(0)
  , numReturns(0)
  , returningWorker(nullptr)
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
  if (workers.size() > 0)
  {
    // if we've finished following the last drone that left, check to see if there's another
    if (returningWorker == nullptr) {
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

    // clear all drones that are leaving
    // Note: i thought i wanted to check if we cleared a drone but i can't remember why ...
    // maybe to notify and sync up with BaseManager recognizing that it needs to retake control
    // maybe can have BaseManager check for drones that stop carrying resources... hmm but that's nontrivial
    workers.erase(std::remove_if(workers.begin(), workers.end(),
      [](const Drone& d) { return d.unit->isCarryingMinerals(); }));

    // check if we're following a worker and it has reached the hatchery
    // update our tracking things if so and stop following it
    if (returningWorker != nullptr && !returningWorker->unit->isCarryingMinerals()) // Note: do we have to check for nullptr?
    {
      int returnFrames = BWAPI::Broodwar->getFrameCount() - initialReturnFrame;
      ++numReturns;
      averageReturnFrames = (averageReturnFrames * (numReturns - 1) + returnFrames) / numReturns;
      returningWorker = nullptr;
    }
  }


  // TODO: has to be a better way to manage this?
  // Reset initial gather frame
  if (initialGatherFrame != 0 && !unit->isBeingGathered())
  {
    initialGatherFrame = 0;

    if (numGatherFrames > 0 && numGatherFrames < 100)
    {
      ++numTrips;
      averageTrip = (averageTrip * (numTrips - 1) + numGatherFrames) / numTrips;
    }
  }
  // Set initial gather frame
  if (initialGatherFrame == 0 && unit->isBeingGathered())
  {
    initialGatherFrame = BWAPI::Broodwar->getFrameCount();
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
  BWAPI::Position &pos = unit->getPosition();
  BWAPI::Broodwar->registerEvent([=](BWAPI::Game*){ BWAPI::Broodwar->drawTextMap(pos, "%.1lf | %d", averageReturnFrames, numReturns); },
    nullptr,
    BWAPI::Broodwar->getLatencyFrames());
}

void Mineral::addWorker(const Drone &d)
{
  workers.push_back(d);
}

int Mineral::remainingGatherFrames()
{
  int numWaitingWorkers = (workers.size() > 1) ? workers.size() - 1 : 0;
  int currentRemainingFrames = (initialGatherFrame == 0) ? 0 : gatherFrames - (BWAPI::Broodwar->getFrameCount() - initialGatherFrame); // Potential Issue:  this might be negative
  return currentRemainingFrames + numWaitingWorkers * gatherFrames;
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