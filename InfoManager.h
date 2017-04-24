#pragma once
#include <BWAPI.h>
#include "UnitInfo.h"
#include <vector>

class InfoManager
{
private:
  int thisPlayerID;
  std::vector<BWAPI::UnitType> livingUnitTypes;

  std::vector<UnitInfo> enemyUnitsInfo; // don't want pointers here, the game blacks them out when no visible
  std::vector<BWAPI::UnitType> enemyUnitTypes;

  BWAPI::Unitset testEnemyUnits = {};

public:
  InfoManager();
  ~InfoManager();

  void init();
  void setPlayerID(int);
  int getPlayerID();
  bool ownedByPlayer(BWAPI::Unit u);
  std::vector<BWAPI::UnitType> getLivingUnitTypes();
  bool hasLivingUnitType(BWAPI::UnitType ut, std::vector<BWAPI::UnitType> &l = InfoManager::Instance().livingUnitTypes);
  void addLivingUnitType(BWAPI::UnitType ut, std::vector<BWAPI::UnitType> &l = InfoManager::Instance().livingUnitTypes);
  void removeLivingUnitType(BWAPI::UnitType ut, std::vector<BWAPI::UnitType> &l = InfoManager::Instance().livingUnitTypes);
  void cleanUpUnitTypeList(std::vector<BWAPI::UnitType> &l = InfoManager::Instance().livingUnitTypes);


  std::vector<BWAPI::UnitType> getEnemyUnitTypes();
  std::vector<UnitInfo> getEnemyUnitsInfo();
  void addEnemyUnit(BWAPI::Unit u);
  void removeEnemyUnit(BWAPI::Unit u);
  int numEnemyType(BWAPI::UnitType ut);
  void cleanUpEnemyTypes(); // fix cleanUpUnitTypeList to work with both later...?

  void debugEnemy();

  static InfoManager & Instance(){
    static InfoManager instance;
    return instance;
  }
};