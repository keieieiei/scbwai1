#include "InfoManager.h"

InfoManager::InfoManager()
{
  thisPlayerID = 0;
  livingUnitTypes = { };
  enemyUnitsInfo = {};
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

std::vector<UnitInfo> InfoManager::getEnemyUnitsInfo()
{
  return enemyUnitsInfo;
}

void InfoManager::addEnemyUnit(BWAPI::Unit u)
{
  printf("Received request to add enemy %s id %d\n", u->getType().c_str(), u->getID());
  // insert can't tell if it's a dup for morphed things?, so check via ID.
  //bool has = false;
  /*for (auto &ux : enemyUnitsInfo) {
    if (ux.getID() == u->getID()){
      has = true;
      // update the type if necessary (morphs, etc)
      if (ux.getType() != u->getType()) {
        ux.setType(u->getType());
      }
      break;
    }
  }*/

  auto ui = std::find(enemyUnitsInfo.begin(), enemyUnitsInfo.end(), u);
  if (ui == enemyUnitsInfo.end())
  {
    UnitInfo newU(u->getID(), u->getType());
    enemyUnitsInfo.push_back(newU);
    printf("Added enemy unit %s id %d\n", u->getType().c_str(), u->getID());
  } 
  else
  {
    // update the type if necessary (morphs, etc)
    if ((*ui).getType() != u->getType()) {
      printf("Updated enemy unit from %s to %s id %d\n", (*ui).getType().c_str(), u->getType().c_str(), u->getID());
      (*ui).setType(u->getType());
    }
  }

  /*if (!has){
    UnitInfo newU(u->getID(), u->getType());
    enemyUnitsInfo.push_back(newU);
    printf("Added enemy unit %s id %d\n", u->getType().c_str(), u->getID());
  }*/
  addLivingUnitType(u->getType(), enemyUnitTypes);
}

void InfoManager::removeEnemyUnit(BWAPI::Unit u)
{
  auto ui = std::find(enemyUnitsInfo.begin(), enemyUnitsInfo.end(), u);
  if (ui != enemyUnitsInfo.end()) enemyUnitsInfo.erase(ui);

  removeLivingUnitType(u->getType(), enemyUnitTypes);
}

int InfoManager::numEnemyType(BWAPI::UnitType ut)
{
  int num = 0;
  for (auto u : enemyUnitsInfo)
  {
    if (u.getType() == ut) num++;
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

void InfoManager::debugEnemy()
{
  printf("last known enemy units: %d\n", enemyUnitsInfo.size());
  printf("# of types: %d\n", enemyUnitTypes.size());
  for (auto &u : enemyUnitsInfo) {
    printf("%s %d\n", u.getType().c_str(), u.getID());
  }

}
