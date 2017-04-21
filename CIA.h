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
  void addLivingUnitType(BWAPI::UnitType u);
  void removeLivingUnitType(BWAPI::UnitType u);

  static CIA & Instance(){
    static CIA instance;
    return instance;
  };
};
