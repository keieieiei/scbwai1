#pragma once
#include <BWAPI.h>
#include "Drone.h"
#include <vector>

class Mineral
{
  //private:
public:
  // Between 75 and 90
  // Usually caps at 85  ... even seen a 94 ! wtf
  // average is around 79.3 -> 79.5
  const int gatherFrames;
  int initialGatherFrame;
  std::vector<Drone*> workers;

  // TODO: these might be obsolete, have to remove in class body too.
  double averageReturnFrames;
  int numReturns;

  // TESTING  maybe keep these for real-time analysis
  int numGatherFrames;
  int numGatherFramesMax;
  int numGatherFramesMin;
  int numTrips;
  double averageTrip;

  void releaseWorker();


  //public:
  Mineral(const BWAPI::Unit &u);
  ~Mineral();

  BWAPI::Unit unit;

  void update();
  void addWorker(Drone *d);
  int  remainingGatherFrames();
  int  getAverageReturnFrames();
  int  timeToBeginMining();
  void calculateReturnFrames(int frames);
};