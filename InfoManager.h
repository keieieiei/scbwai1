#pragma once
#include <BWAPI.h>
#include "UnitInfo.h"
#include <vector>

class InfoManager
{
private:
  int thisPlayerID;
  std::vector<UnitInfo> playerUnitsInfo;
  std::vector<BWAPI::UnitType> playerUnitTypes;

  std::vector<UnitInfo> enemyUnitsInfo;
  std::vector<BWAPI::UnitType> enemyUnitTypes;

  const char* currPlayer;
  std::vector<UnitInfo> *currUnits;
  std::vector<BWAPI::UnitType> *currTypes;

  void setCurrentVar(const char* s);
  void setCurrentVar(BWAPI::Unit u);

public:
  InfoManager();
  ~InfoManager();

  void setPlayerID(int);
  int getPlayerID();

  bool ownedByPlayer(BWAPI::Unit u);

  std::vector<BWAPI::UnitType> getPlayerUnitTypes();

  bool hasUnitType(BWAPI::UnitType ut, std::vector<BWAPI::UnitType> &v = InfoManager::Instance().playerUnitTypes);
  void addUnitType(BWAPI::UnitType ut, std::vector<BWAPI::UnitType> &v = InfoManager::Instance().playerUnitTypes);
  void removeUnitType(BWAPI::UnitType ut, std::vector<BWAPI::UnitType> &v = InfoManager::Instance().playerUnitTypes);
  int numUnitType(BWAPI::UnitType, std::vector<UnitInfo> v);

  void addUnitInfo(BWAPI::Unit u);
  void removeUnitInfo(BWAPI::Unit u);
  bool hasUnitInfo(BWAPI::Unit u);

  std::vector<BWAPI::UnitType> getEnemyUnitTypes();
  std::vector<UnitInfo> getEnemyUnitsInfo();

  void debug(const char* s);

  static InfoManager & Instance(){
    static InfoManager instance;
    return instance;
  }
};