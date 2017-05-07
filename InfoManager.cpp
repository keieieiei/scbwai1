#include "InfoManager.h"

InfoManager::InfoManager()
{
  thisPlayerID = 0;
  playerUnitTypes = {};
  playerUnitsInfo = {};
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

////////////////////////// UnitType ////////////////////////////////////
std::vector<BWAPI::UnitType> InfoManager::getPlayerUnitTypes()
{
  return playerUnitTypes;
}

std::vector<BWAPI::UnitType> InfoManager::getEnemyUnitTypes()
{
  return enemyUnitTypes;
}

bool InfoManager::hasUnitType(BWAPI::UnitType ut, std::vector<BWAPI::UnitType> &v)
{
  auto ui = std::find(v.begin(), v.end(), ut);
  return ui != v.end();
}

void InfoManager::addUnitType(BWAPI::UnitType ut, std::vector<BWAPI::UnitType> &v)
{
  if (!hasUnitType(ut, v)) v.push_back(ut);
}

void InfoManager::removeUnitType(BWAPI::UnitType ut, std::vector<BWAPI::UnitType> &v)
{
  auto ui = std::find(v.begin(), v.end(), ut);
  if (ui != v.end()) v.erase(ui);
}

int InfoManager::numUnitType(BWAPI::UnitType ut, std::vector<UnitInfo> v)
{
  int num = 0;
  for (auto u : v)
  {
    if (u.getType() == ut) num++;
  }
  return num;
}

////////////////////////// UnitInfo ////////////////////////////////////
std::vector<UnitInfo> InfoManager::getPlayerUnitsInfo()
{
  return playerUnitsInfo;
}

std::vector<UnitInfo> InfoManager::getEnemyUnitsInfo()
{
  return enemyUnitsInfo;
}

void InfoManager::addUnitInfo(BWAPI::Unit u)
{
  setCurrentVar(u);

  if (debug) printf("Received request to add %s %d to %s units\n", u->getType().c_str(), u->getID(), currPlayer);

  auto ui = std::find(currUnits->begin(), currUnits->end(), u);

  if (ui == currUnits->end())
  {
    // unit does not exist in the list; create UnitInfo and add
    UnitInfo newU(u);
    currUnits->push_back(newU);
    if (debug) printf("Added %s unit %s id %d\n", currPlayer, u->getType().c_str(), u->getID());
  }
  else
  {
    // TODO: maybe? if something morphs while it is still visible, discover is not triggered so this won't happen for 
    //       enemies atm
    // TODO: check if currUnits->unit->type is the same type as currUnits->type
    // unit already exists; update the type if necessary (morphs, etc)
    if (ui->getType() != u->getType()) {
      BWAPI::UnitType prevType = ui->getType();

      if (debug) printf("IM: Updated %s unit from %s to %s id %d\n", currPlayer, prevType.c_str(), u->getType().c_str(), u->getID());

      ui->updateUnitInfo(u);

      if (numUnitType(prevType, *currUnits) == 0)
      {
        removeUnitType(prevType, *currTypes);
      }
    }
  }

  // update unit type list
  addUnitType(u->getType(), *currTypes);
}

void InfoManager::updateUnitInfo(BWAPI::Unit u) {
  setCurrentVar(u);
  auto ui = std::find(currUnits->begin(), currUnits->end(), u);
  if (ui != currUnits->end())
  {
    ui->updateUnitInfo(u);
  }
}

void InfoManager::removeUnitInfo(BWAPI::Unit u)
{
  setCurrentVar(u);

  if (debug) printf("Received request to remove %s %d from %s units\n", u->getType().c_str(), u->getID(), currPlayer);

  // remove unit
  auto ui = std::find(currUnits->begin(), currUnits->end(), u);
  if (ui != currUnits->end()) currUnits->erase(ui);
  if (debug) printf("Removed %s unit %s id %d\n", currPlayer, u->getType().c_str(), u->getID());

  // update unit type list
  if (numUnitType(u->getType(), *currUnits) == 0)
  {
    removeUnitType(u->getType(), *currTypes);
  }
}

bool InfoManager::hasUnitInfo(BWAPI::Unit u)
{
  std::vector<UnitInfo> v = ownedByPlayer(u) ? playerUnitsInfo : enemyUnitsInfo;
  auto ui = std::find(v.begin(), v.end(), u);
  return ui != v.end();
}

////////////////////////// EnemyInfo ////////////////////////////////////
// excludes resources
std::vector<UnitInfo> InfoManager::getEnemyBuildings()
{
  std::vector<UnitInfo> res;
  for (auto ui : enemyUnitsInfo)
  {
    if (ui.getType().isBuilding() && !ui.getType().isNeutral())
    {
      res.push_back(ui);
    }
  }
  return res;
}

int InfoManager::getNumEnemyFlyers()
{
  int num = 0;
  for (auto ui : enemyUnitsInfo)
  {
    if (ui.getType().isFlyer())
    {
      num++;
    }
  }
  return num;
}

////////////////////////// Util ////////////////////////////////////
void InfoManager::setCurrentVar(BWAPI::Unit u)
{
  if (ownedByPlayer(u))
  {
    setCurrentVar("player");
  }
  else
  {
    setCurrentVar("enemy");
  }
}

void InfoManager::setCurrentVar(const char* s)
{
  currPlayer = s;
  currUnits = (s == "player") ? &playerUnitsInfo : &enemyUnitsInfo;
  currTypes = (s == "player") ? &playerUnitTypes : &enemyUnitTypes;
}

////////////////////////// Debug ////////////////////////////////////
void InfoManager::debugUnits(const char* s){
  setCurrentVar(s);

  printf("%s units logged:\n", s);
  for (auto &u : *currUnits) {
    printf("%s %d @ %d, %d; base @ %d, %d\n", u.getType().c_str(), u.getID(), u.getPosition().x, u.getPosition().y, u.getStartLocBase().x, u.getStartLocBase().y);
  }
  printf("\n\n# of each %s type:\n", s);
  for (auto ut : *currTypes) {
    printf("%s: %d\n", ut.c_str(), numUnitType(ut, *currUnits));
  }
  printf("\n\n# of %s types: %d\n", s, currTypes->size());
  printf("last known %s units: %d\n\n", s, currUnits->size());
}

void InfoManager::debugEnemyBuildings(){
  printf("Enemy Buildings:\n");
  for (auto &u : getEnemyBuildings())
  {
    printf("%s %d at %d, %d\n", u.getType().c_str(),u.getID(), u.getPosition().x, u.getPosition().y);
  }

}