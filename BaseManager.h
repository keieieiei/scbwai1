#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include <vector>

class BaseManager
{
private:
  BWAPI::Unitset workers;
  BWAPI::Unitset minerals;
  BWAPI::Unit vespene; // Potential Issue: case of more than one geyser in base?
  BWAPI::Unit main;
  BWTA::BaseLocation *location;

  void detectResources();

public:
  BaseManager(BWAPI::Unit &m, BWTA::BaseLocation *&l);
  ~BaseManager();

  void update();
  void addWorker(BWAPI::Unit u);
  void removeWorker(BWAPI::Unit u);
  bool containsWorker(BWAPI::Unit u);
  bool hasMain();
  int  numWorkers();
  int  remainingMinerals();
  int  remainingVespene();
};