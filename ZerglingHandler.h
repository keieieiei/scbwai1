#pragma once
#include <BWAPI.h>
#include <BWTA.h>

enum class Objective
{
  ASSAULT,
  DEFEND,
  NONE,
};

class ZerglingHandler
{
private:
  BWAPI::Position target;
  Objective objective;

public:
  ZerglingHandler(BWAPI::Unit u);
  ~ZerglingHandler();

  BWAPI::Unit unit;
  void update();
  void attack(BWAPI::Position pos);
  void defend(BWAPI::Position pos);

};