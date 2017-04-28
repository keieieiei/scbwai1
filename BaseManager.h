#pragma once
#include <BWAPI.h>
#include <memory>

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

  std::vector<Worker> workers;
  std::vector<std::shared_ptr<Mineral>> minerals;

public:
  BaseManager(const BWAPI::Unit unit);
  ~BaseManager();

  BWAPI::Unit main;

  void update();
  int  numWorkers();
  bool containsWorker(BWAPI::Unit unit);
  void addWorker(BWAPI::Unit unit);
  BWAPI::Unit takeWorker();
};