#pragma once
#include <BWAPI.h>
#include "Drone.h"

class Mineral
{
private:
  // Between 75 and 90
  // Usually caps at 85  ... even seen a 94 ! wtf
  // average is around 79.3 -> 79.5
  const int gatherFrames;
  int initialGatherFrame;
  int initialReturnFrame;
  double averageReturnFrames;
  int numReturns;
  std::vector<Drone> workers;
  const Drone *returningWorker;

  // TESTING
  int numGatherFrames;
  int numGatherFramesMax;
  int numGatherFramesMin;
  int numTrips;
  double averageTrip;

  void releaseWorker();
  void calculateReturnFrames();

public:
  Mineral(const BWAPI::Unit &u);
  ~Mineral();

  BWAPI::Unit unit;

  void update();
  void addWorker(const Drone &d);
  int  remainingGatherFrames();
  int  getAverageReturnFrames();
  int  timeToBeginMining();
};