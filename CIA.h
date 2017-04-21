#pragma once
#include <BWAPI.h>
#include <vector>

class CIA
{
public:
  CIA();
  ~CIA(){};

  std::vector<BWAPI::UnitType> _livingUnitTypes;
  int _thisPlayer;
  void addLivingUnitType(BWAPI::UnitType);
  void removeLivingUnitType(BWAPI::UnitType);

  static CIA & Instance(){
    static CIA instance;
    return instance;
  };
};