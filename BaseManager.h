#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include <queue>
#include <vector>

#include "Mineral.h"
#include "Drone.h"

class BaseManager
{
private:
  std::vector<Mineral*> minerals; // TODO:  don't want this to be a pointer
  std::vector<Drone> drones;
  BWAPI::Unit vespene; // Potential Issue: case of more than one geyser in base?
  BWAPI::Unit main;
  BWTA::BaseLocation *location;
  std::vector<BWAPI::Unit> sortedMinerals;
  std::vector<BWAPI::Unit> freeMinerals;
  std::vector<BWAPI::Unit> mineralsQueue;

  // debugging
  std::vector<int> isGatherFrameCount;

  void detectResources();

public:
  BaseManager(const BWAPI::Unit &m, BWTA::BaseLocation *&l); // TODO:  figure out why BaseLocation can't be const
  ~BaseManager();

  void update();
  void addWorker(const BWAPI::Unit &u);
  BWAPI::Unit takeWorker();
  bool containsWorker(const Drone &d);
  bool hasMain();
  int  numWorkers();
  int  remainingMinerals();
  int  remainingVespene();
};