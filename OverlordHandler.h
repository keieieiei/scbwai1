#include <BWAPI.h>

/*
  Used as a scout, a transport, a detector, and maybe even as a meatshield.
*/

enum class ScoutObjective
{
  REVEAL_TILE,
  WATCH_TILE,
  SPREAD_VISION,
  EXPLORE_BASE,
  TRACK_UNIT,
  NONE,
};

class OverlordHandler
{
private:
  BWAPI::Position targetPosition;
  ScoutObjective objective;

public:
  OverlordHandler(BWAPI::Unit u);
  ~OverlordHandler();

  BWAPI::Unit unit;

  void update();
  void revealTile(BWAPI::Position tp);
};