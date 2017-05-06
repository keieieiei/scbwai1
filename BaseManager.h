#pragma once
#include <BWAPI.h>
#include <memory>
#include "DroneHandler.h"

class DroneHandler;

class BaseManager
{
private:
  static const int gatherFrames;
  static const int moveDistanceMax;
  static const std::unordered_map<int, int> moveToMineralFrames;
  static const int returnDistanceMin;
  static const int returnDistanceMax;
  static const std::unordered_map<int, int> returnToMainFrames;

  struct Mineral
  {
    BWAPI::Unit unit;
    int numWorkers;
    int initialGatherFrame;
  };

  /* Deprecated
  struct Worker
  {
    enum WorkerState
    {
      DEFAULT,
      MOVING_TO_GATHER,
      AT_GATHER_POINT,
      RETURNING_RESOURCE,
    };

    BWAPI::Unit unit;
    std::weak_ptr<Mineral> resource;
    WorkerState state;
  };
  */

  std::vector<std::weak_ptr<DroneHandler>> drones;
  std::vector<std::shared_ptr<Mineral>> minerals;
  // TODO should be a vector since some bases have doublegas...
  BWAPI::Unit vespene;

public:
  BaseManager(const BWAPI::Unit unit);
  ~BaseManager();

  BWAPI::Unit main;

  void update();
  int  numWorkers();
  bool containsWorker(BWAPI::Unit unit);
  void addWorker(std::shared_ptr<UnitHandler> dh, std::shared_ptr<BaseManager> bm);
  BWAPI::Unit takeWorker();

  void finishGathering(BWAPI::Unit u);
  BWAPI::Unit requestMineralAssignment(BWAPI::Position pos);
};