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

  // variables to hold the objects of the player we are currently working on
  const char* currPlayer;
  std::vector<UnitInfo> *currUnits;
  std::vector<BWAPI::UnitType> *currTypes;

  // methods to set the objects of the player we are currently working on 
  void setCurrentVar(const char* s);
  void setCurrentVar(BWAPI::Unit u);

  // AKG:
  BWAPI::Position enemyStartingPosition;

  bool debug = false;
public:
  InfoManager();
  ~InfoManager();

  void setPlayerID(int);
  int getPlayerID();

  bool ownedByPlayer(BWAPI::Unit u);

  std::vector<BWAPI::UnitType> getPlayerUnitTypes();
  std::vector<BWAPI::UnitType> getEnemyUnitTypes();

  bool hasUnitType(BWAPI::UnitType ut, std::vector<BWAPI::UnitType> &v = InfoManager::Instance().playerUnitTypes);
  void addUnitType(BWAPI::UnitType ut, std::vector<BWAPI::UnitType> &v = InfoManager::Instance().playerUnitTypes);
  void removeUnitType(BWAPI::UnitType ut, std::vector<BWAPI::UnitType> &v = InfoManager::Instance().playerUnitTypes);
  int numUnitType(BWAPI::UnitType, std::vector<UnitInfo> v);

  std::vector<UnitInfo> getPlayerUnitsInfo();
  std::vector<UnitInfo> getEnemyUnitsInfo();

  void addUnitInfo(BWAPI::Unit u);
  void removeUnitInfo(BWAPI::Unit u);
  bool hasUnitInfo(BWAPI::Unit u);
  void updateUnitInfo(BWAPI::Unit u);

  std::vector<UnitInfo> getEnemyBuildings();
  // return number for now, if necessary can create a separate list of flying enemies only with more specific info
  int getNumEnemyFlyers();

  static InfoManager & Instance(){
    static InfoManager instance;
    return instance;
  }

  void debugUnits(const char* s);
  void debugEnemyBuildings();
  void setDebug(bool b) { debug = b; };
};