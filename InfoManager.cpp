#include "InfoManager.h"

InfoManager::InfoManager()
{
  thisPlayerID = 0;
  livingUnitTypes = { };
  enemyUnits = {};
}


InfoManager::~InfoManager()
{
}

void InfoManager::setPlayerID(int id)
{
  thisPlayerID = id;
}

int InfoManager::getPlayerID()
{
  return thisPlayerID;
}

bool InfoManager::ownedByPlayer(BWAPI::Unit u)
{
  return u->getPlayer()->getID() == thisPlayerID;
}

std::vector<BWAPI::UnitType> InfoManager::getLivingUnitTypes()
{
  return livingUnitTypes;
}

bool InfoManager::hasLivingUnitType(BWAPI::UnitType ut, std::vector<BWAPI::UnitType> &l)
{
  auto ui = std::find(l.begin(), l.end(), ut);
  return ui != l.end();
}

void InfoManager::addLivingUnitType(BWAPI::UnitType ut, std::vector<BWAPI::UnitType> &l)
{
  if (!hasLivingUnitType(ut, l)) l.push_back(ut);
}

void InfoManager::removeLivingUnitType(BWAPI::UnitType ut, std::vector<BWAPI::UnitType> &l)
{
  auto ui = std::find(l.begin(), l.end(), ut);
  if (ui != l.end()) l.erase(ui);
}

// currently only cleans up the player list
void InfoManager::cleanUpUnitTypeList(std::vector<BWAPI::UnitType> &l){
  for (auto ut : l){
    if (BWAPI::Broodwar->self()->allUnitCount(ut) == 0)
    {
      removeLivingUnitType(ut, l);
    }
  }
}

std::vector<BWAPI::UnitType> InfoManager::getEnemyUnitTypes()
{
  return enemyUnitTypes;
}

BWAPI::Unitset InfoManager::getEnemyUnits()
{
  return enemyUnits;
}

void InfoManager::addEnemyUnit(BWAPI::Unit u)
{
  // insert can't tell if it's a dup, so check.
  bool has = false;
  for (auto ux : enemyUnits) {
    if (ux->getID() == u->getID()){
      has = true;    
      break;
    }
  }

  if (!has) enemyUnits.insert(u);
  addLivingUnitType(u->getType(), enemyUnitTypes);
}

void InfoManager::removeEnemyUnit(BWAPI::Unit u)
{
  enemyUnits.erase(u);
  removeLivingUnitType(u->getType(), enemyUnitTypes);
}

int InfoManager::numEnemyType(BWAPI::UnitType ut)
{
  int num = 0;
  for (auto &u : enemyUnits)
  {
    if (u->getType() == ut) num++;
  }
  return num;
}
void InfoManager::cleanUpEnemyTypes()
{
  for (auto ut : enemyUnitTypes){
    if (numEnemyType(ut) == 0)
    {
      removeLivingUnitType(ut, enemyUnitTypes);
    }
  }
}
