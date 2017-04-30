#include <BWAPI.h>
#include "UnitHandler.h"

/*
  Used as a scout, a transport, a detector, and maybe even as a meatshield.
*/

class OverlordHandler: public UnitHandler
{
public:
  OverlordHandler(BWAPI::Unit u);
  virtual ~OverlordHandler();

  virtual void update();
  virtual bool setObjective(Objective o, BWAPI::Position p);
};