#include "CIA.h"

CIA::CIA()
{
}

void CIA::addLivingUnitType(BWAPI::UnitType unitType)
{
  auto ut = std::find(_livingUnitTypes.begin(), _livingUnitTypes.end(), unitType);
  if (ut == _livingUnitTypes.end())
  {
    _livingUnitTypes.push_back(unitType);
  }
}

void CIA::removeLivingUnitType(BWAPI::UnitType unitType)
{
  auto ut = std::find(_livingUnitTypes.begin(), _livingUnitTypes.end(), unitType);
  if (ut != _livingUnitTypes.end())
  {
    _livingUnitTypes.erase(ut);
  }
}

/*CIA & CIA::Instance()
{
static CIA instance;
return instance;
}*/