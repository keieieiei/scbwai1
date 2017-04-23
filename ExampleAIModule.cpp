#include "ExampleAIModule.h"
#include <iostream>

using namespace BWAPI;
using namespace Filter;

// BWTA2 Analysis Trackers
// bool analyzed;
// bool analysis_just_finished;

// temporary SpawningPool flag
int buildingPool;
int buildingHatch;
bool hasPool;
int buildingOverlord;
// bug  need to fix it when buildings are cancelled
int droneCount;
int zerglingCount;
// debug
int morphCount;
// not fully implemented, need to figure out when to decrease it properly
int mineralBuffer;
// holds the row position to avoid overlap
int rowPos;
BaseManager *mainManager;
// reserve a unit
int reserve;

void ExampleAIModule::onStart()
{
  // get player id
  InfoManager::Instance().setPlayerID(Broodwar->self()->getID());

  // Hello World!
  // Broodwar->sendText("Hello world!");

  // Print the map name.
  // BWAPI returns std::string when retrieving a string, don't forget to add .c_str() when printing!
  Broodwar << "The map is " << Broodwar->mapName() << "!" << std::endl;

  // Enable the UserInput flag, which allows us to control the bot and type messages.
  Broodwar->enableFlag(Flag::UserInput);

  // Uncomment the following line and the bot will know about everything through the fog of war (cheat).
  //Broodwar->enableFlag(Flag::CompleteMapInformation);

  // Set the command optimization level so that common commands can be grouped
  // and reduce the bot's APM (Actions Per Minute).
  Broodwar->setCommandOptimizationLevel(2);

  // Check if this is a replay
  if (Broodwar->isReplay())
  {
    // Announce the players in the replay
    Broodwar << "The following players are in this replay:" << std::endl;

    // Iterate all the players in the game using a std:: iterator
    Playerset players = Broodwar->getPlayers();
    for (auto p : players)
    {
      // Only print the player if they are not an observer
      if (!p->isObserver())
        Broodwar << p->getName() << ", playing as " << p->getRace() << std::endl;
    }
  }
  else // if this is not a replay
  {
    // Retrieve you and your enemy's races. enemy() will just return the first enemy.
    // If you wish to deal with multiple enemies then you must use enemies().
    if (Broodwar->enemy()) // First make sure there is an enemy
      Broodwar << "The matchup is " << Broodwar->self()->getRace() << " vs " << Broodwar->enemy()->getRace() << std::endl;
  }

  // Read map information into BWTA
  BWTA::readMap();
  // analyzed = false;
  // analysis_just_finished = false;

  BWTA::analyze();
  // analyzed = true;
  // analysis_just_finished = true;

  // temporary
  hasPool = false;
  buildingHatch = 0;
  buildingPool = 0;
  buildingOverlord = 0;
  // initialize to 1 to offset the starting hatchery
  droneCount = 1;
  zerglingCount = 0;
  morphCount = 0;
  mineralBuffer = 0;
  BWTA::BaseLocation *l = BWTA::getStartLocation(Broodwar->self());
  BWAPI::Unit m;
  for (auto &u : Broodwar->self()->getUnits())
  {
    if (u->getType().isResourceDepot())
    {
      m = u;
    }
  }
  mainManager = new BaseManager(m, l);

  FILE *df;
  AllocConsole();
  freopen_s(&df, "conin$", "r", stdin);
  freopen_s(&df, "conout$", "w", stdout);
  freopen_s(&df, "conout$", "w", stderr);
  printf("Debugging Window:\n");
}

void ExampleAIModule::onEnd(bool isWinner)
{
  // Called when the game ends
  if (isWinner)
  {
    // Log your win here!
  }

  delete mainManager;
  mainManager = nullptr;

  printf("end");
  FreeConsole();
}

void ExampleAIModule::onFrame()
{
  // Called once every game frame

  // Display the game frame rate as text in the upper left area of the screen
  Broodwar->drawTextScreen(200, 0, "FPS: %d", Broodwar->getFPS());
  Broodwar->drawTextScreen(200, 20, "Average FPS: %f", Broodwar->getAverageFPS()); 
  Broodwar->drawTextScreen(200, 10, "Latency Frames : %d", Broodwar->getLatencyFrames());

  // Return if the game is a replay or is paused
  if (Broodwar->isReplay() || Broodwar->isPaused() || !Broodwar->self())
    return;

  // BWTA draw
  drawTerrainData();
  /*if (analyzed)
  drawTerrainData();
  if (analysis_just_finished)
  {
  Broodwar << "Finished analyzing map." << std::endl;
  analysis_just_finished = false;
  }*/

  // update hatch/pool building times if they are building
  if (buildingHatch > 0)
  {
    --buildingHatch;
  }
  if (buildingPool > 0)
  {
    --buildingPool;
  }
  if (buildingOverlord > 0)
  {
    --buildingOverlord;
  }

  // debug
  Broodwar->drawTextScreen(200, 40, "buildingHatch value: %d", buildingHatch);
  Broodwar->drawTextScreen(200, 60, "buildingPool value: %d", buildingPool);
  Broodwar->drawTextScreen(200, 80, "hasPool value: %d", hasPool);
  Broodwar->drawTextScreen(200, 100, "droneCount value: %d", droneCount);
  Broodwar->drawTextScreen(200, 120, "zerglingCount value: %d", zerglingCount);
  Broodwar->drawTextScreen(50, 0, "mineralBuffer: %d", mineralBuffer);
  Broodwar->drawTextScreen(50, 20, "Supply: %d/%d", Broodwar->self()->supplyUsed(), Broodwar->self()->supplyTotal());

  // Basemanager Debug
  Broodwar->drawTextScreen(500, 20, "hasMain(): %d", mainManager->hasMain());
  Broodwar->drawTextScreen(500, 40, "numWorkers(): %d", mainManager->numWorkers());
  Broodwar->drawTextScreen(500, 60, "remainingMinerals(): %d", mainManager->remainingMinerals());
  Broodwar->drawTextScreen(500, 80, "remainingVespene(): %d", mainManager->remainingVespene());

  // Player Unit Debug
  rowPos = 20;
  for (BWAPI::UnitType ut : InfoManager::Instance().getPlayerUnitTypes()){
    Broodwar->drawTextScreen(350, rowPos, "%s: %d", ut.c_str(), Broodwar->self()->allUnitCount(ut));
    rowPos += 20;
  }

  // Enemy Unit Debug
  rowPos = 120;
  for (BWAPI::UnitType ut : InfoManager::Instance().getEnemyUnitTypes()){
    Broodwar->drawTextScreen(500, rowPos, "%s: %d", ut.c_str(), InfoManager::Instance().numUnitType(ut, InfoManager::Instance().getEnemyUnitsInfo()));
    rowPos += 20;
  }
  Broodwar->drawTextScreen(500, rowPos, "# Known Enemy Units: %d", InfoManager::Instance().getEnemyUnitsInfo().size());

  // TESTING:  move back to end of onFrame()
  mainManager->update();

  // Prevent spamming by only running our onFrame once every number of latency frames.
  // Latency frames are the number of frames before commands are processed.
  if (Broodwar->getFrameCount() % Broodwar->getLatencyFrames() != 0)
    return;

  // hack way of stopping anything else from being produced
  // supply is 2x actual supply; we're going for 9pool
  if (Broodwar->self()->supplyUsed() >= 18 && !hasPool && buildingPool < 200)
  {
    mineralBuffer = 200;
  }
  else
  {
    mineralBuffer = 0;
  }

  // Pool Building Code; frame delay if it fails to build
  // TESTING : remove the if true and make 2nd else if an if to undo
  if (!hasPool && buildingPool == 0 && Broodwar->self()->minerals() >= UnitTypes::Zerg_Spawning_Pool.mineralPrice())
  {
    //mineralBuffer += UnitTypes::Zerg_Spawning_Pool.mineralPrice();
    if (mainManager->numWorkers() > 0)
    {
      const Unit &u = mainManager->takeWorker();
      TilePosition buildPosition = Broodwar->getBuildLocation(UnitTypes::Zerg_Spawning_Pool, u->getTilePosition());
      u->build(UnitTypes::Zerg_Spawning_Pool, buildPosition);
      buildingPool = 180;
    }
  }
  else if (buildingHatch == 0 && Broodwar->self()->minerals() >= UnitTypes::Zerg_Hatchery.mineralPrice() + mineralBuffer)
  {
    //mineralBuffer += UnitTypes::Zerg_Hatchery.mineralPrice();
    // getBuildLocation() might not find a suitable location for UnitTypes::Zerg_Hatchery
    if (mainManager->numWorkers() > 0)
    {
      // TODO: often can't find a placement position for hatchery, trying the same invalid location over and over
      const Unit &u = mainManager->takeWorker();
      TilePosition buildPosition = Broodwar->getBuildLocation(UnitTypes::Zerg_Spawning_Pool, u->getTilePosition());
      u->build(UnitTypes::Zerg_Hatchery, buildPosition);
      buildingHatch = 240;
    }
  }

  // Iterate through all the units that we own
  for (auto &u : Broodwar->self()->getUnits())
  {
    // Ignore the unit if it no longer exists
    // Make sure to include this block when handling any Unit pointer!
    if (!u->exists())
      continue;

    // Ignore the unit if it has one of the following status ailments
    if (u->isLockedDown() || u->isMaelstrommed() || u->isStasised())
      continue;

    // Ignore the unit if it is in one of the following states
    if (u->isLoaded() || !u->isPowered() || u->isStuck())
      continue;

    // Ignore the unit if it is incomplete or busy constructing
    //if ( !u->isCompleted() || u->isConstructing() )
    //  continue;

    // reserve a unit
    if (reserve == 0 && u->getType().isWorker())
    {
      reserve = u->getID();
    }

    // Finally make the unit do some stuff!

    if (u->isConstructing())
    {
      if (u->getBuildType() == UnitTypes::Zerg_Hatchery)
      {
        //buildingHatch = u->getRemainingBuildTime(); no need to build hatches sequentially
        Broodwar->registerEvent([u](Game*){ Broodwar->drawTextScreen(200, 140, "%s morphing in %d frames", u->getType().c_str(), u->getRemainingBuildTime()); },
          nullptr,
          Broodwar->getLatencyFrames());
      }
      if (u->getBuildType() == UnitTypes::Zerg_Spawning_Pool)
      {
        buildingPool = u->getRemainingBuildTime() + 48; // close the gap between buildingPool and hasPool
        Broodwar->registerEvent([u](Game*){ Broodwar->drawTextScreen(200, 160, "%s morphing in %d frames", u->getType().c_str(), u->getRemainingBuildTime()); },
          nullptr,
          Broodwar->getLatencyFrames());
      }
      if (u->getBuildType() == UnitTypes::Zerg_Overlord)
      {
        buildingOverlord = u->getRemainingBuildTime() + 48;
        Broodwar->registerEvent([u](Game*){ Broodwar->drawTextScreen(200, 120, "%s morphing in %d frames", u->getType().c_str(), u->getRemainingBuildTime()); },
          nullptr,
          Broodwar->getLatencyFrames());
      }

      /*if (u->getBuildType() == UnitTypes::Zerg_Hatchery || u->getBuildType() == UnitTypes::Zerg_Spawning_Pool)
      // this some weird ass shit the [u] is any variables that are used in the arguments of the function call (Game*){ ... } idfk but probably to help with the &
      // and then nullptr is a buffer argument ('cos we need to get to 3rd args to put how many frames it lasts for)
      Broodwar->registerEvent([u](Game*){ Broodwar->drawTextScreen(200, 120, "%s morphing in %d frames", u->getType().c_str(), u->getRemainingBuildTime()); },
      nullptr,
      Broodwar->getLatencyFrames());*/
      //Broodwar->sendText("%s building in %d frames", u->getType().c_str(), u->getRemainingBuildTime());
    }

    if (u->getType() == UnitTypes::Zerg_Zergling)
    {
      // shitty attack & scouting subroutine
      if (!u->attack(u->getClosestUnit(Filter::IsEnemy)))
      {
        for (BWTA::BaseLocation * startLocation : BWTA::getStartLocations())
        {
          if (!BWAPI::Broodwar->isExplored(startLocation->getTilePosition()))
          {
            u->attack(startLocation->getPosition());
            continue;
          }
        }
      }
    }
    // If the unit is a worker unit **but not the reserve!
    else if (u->getType().isWorker() && u->getID() != reserve)
    {

      // if our worker is gathering or idle but doesn't belong to mainManager
      if (!mainManager->containsWorker(u) && (u->isIdle() || u->isGatheringMinerals() || u->isGatheringGas()))
      {
        mainManager->addWorker(u);
        // Order workers carrying a resource to return them to the center,
        // otherwise find a mineral patch to harvest.
        /* base manager should handle this now
        if (u->isCarryingGas() || u->isCarryingMinerals())
        {
        u->returnCargo();
        }
        else if (!u->getPowerUp())  // The worker cannot harvest anything if it
        {                             // is carrying a powerup such as a flag
        // Harvest from the nearest mineral patch or gas refinery
        if (!u->gather(u->getClosestUnit(IsMineralField || IsRefinery)))
        {
        // If the call fails, then print the last error message
        Broodwar << Broodwar->getLastError() << std::endl;
        }
        } // closure: has no powerup */
      } // closure: if idle
    }
    else if (u->getType().isResourceDepot()) // A resource depot is a Command Center, Nexus, or Hatchery
    {
      /*if (Broodwar->self()->supplyUsed() - Broodwar->self()->supplyTotal() <= 2 &&
      Broodwar->self()->minerals() >= UnitTypes::Zerg_Overlord.mineralPrice() + mineralBuffer &&
      Broodwar->self()->incompleteUnitCount(UnitTypes::Zerg_Overlord) == 0)
      {
      u->train(UnitTypes::Zerg_Overlord);
      }
      else*/
      // TESTING make sure to add lings back in >.>
      //if (hasPool && Broodwar->self()->minerals() >= UnitTypes::Zerg_Zergling.mineralPrice() + mineralBuffer)
      // {
      //  u->train(UnitTypes::Zerg_Zergling);
      //}
      /*else*/ if (droneCount < 18 && Broodwar->self()->minerals() >= UnitTypes::Zerg_Drone.mineralPrice() + mineralBuffer)
      {
        u->train(u->getType().getRace().getWorker());
      }
      // Order the depot to construct more workers! But only when it is idle.
      if (u->isIdle() && !u->isTraining())
      {
        // If that fails, draw the error at the location so that you can visibly see what went wrong!
        // However, drawing the error once will only appear for a single frame
        // so create an event that keeps it on the screen for some frames
        Position pos = u->getPosition();
        Error lastErr = Broodwar->getLastError();
        Broodwar->registerEvent([pos, lastErr](Game*){ Broodwar->drawTextMap(pos, "%c%s", Text::White, lastErr.c_str()); },   // action
          nullptr,    // condition
          Broodwar->getLatencyFrames());  // frames to run

        if (lastErr == Errors::Insufficient_Supply && buildingOverlord == 0)
        {
          u->train(UnitTypes::Zerg_Overlord);
          buildingOverlord = 48;
        }
        /*
        // Retrieve the supply provider type in the case that we have run out of supplies
        UnitType supplyProviderType = u->getType().getRace().getSupplyProvider();
        static int lastChecked = 0;
        // If we are supply blocked and haven't tried constructing more recently
        if (  lastErr == Errors::Insufficient_Supply &&
        lastChecked + 400 < Broodwar->getFrameCount() &&
        Broodwar->self()->incompleteUnitCount(supplyProviderType) == 0 )
        {
        lastChecked = Broodwar->getFrameCount();
        // Retrieve a unit that is capable of constructing the supply needed
        Unit supplyBuilder = u->getClosestUnit(  GetType == supplyProviderType.whatBuilds().first &&
        (IsIdle || IsGatheringMinerals) &&
        IsOwned);
        // If a unit was found
        if ( supplyBuilder )
        {
        if ( supplyProviderType.isBuilding() )
        {
        TilePosition targetBuildLocation = Broodwar->getBuildLocation(supplyProviderType, supplyBuilder->getTilePosition());
        if ( targetBuildLocation )
        {
        // Register an event that draws the target build location
        Broodwar->registerEvent([targetBuildLocation,supplyProviderType](Game*)
        {
        Broodwar->drawBoxMap( Position(targetBuildLocation),
        Position(targetBuildLocation + supplyProviderType.tileSize()),
        Colors::Blue);
        },
        nullptr,  // condition
        supplyProviderType.buildTime() + 100 );  // frames to run
        // Order the builder to construct the supply structure
        supplyBuilder->build( supplyProviderType, targetBuildLocation );
        }
        }
        else
        {
        // Train the supply provider (Overlord) if the provider is not a structure
        supplyBuilder->train( supplyProviderType );
        }
        } // closure: supplyBuilder is valid
        } // closure: insufficient supply */
      } // closure: failed to train idle unit
    }
  } // closure: unit iterator

  // clean up enemy list (should prolly move this later)
  //InfoManager::Instance().cleanUpEnemyTypes();
}

void ExampleAIModule::onSendText(std::string text)
{
  // BWTA2 read /analyze to begin map analysis
  /*if (text == "/analyze")
  {
  if (analyzed == false)
  {
  Broodwar << "Analyzing map... this may take a minute" << std::endl;
  //BWTA::analyze();
  //analyzed = true;
  //analysis_just_finished = true;
  //thread seemed to create problems
  //CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)AnalyzeThread, NULL, 0, NULL);
  }
  else
  {
  // Send the text to the game if it is not being processed.
  Broodwar->sendText("%s", text.c_str());
  }
  }*/
  Broodwar->sendText("%s", text.c_str());

  // Make sure to use %s and pass the text as a parameter,
  // otherwise you may run into problems when you use the %(percent) character!

  //debug trigger
  if (text == "print enemy") {
    InfoManager::Instance().debug("enemy");
  } 
  else if (text == "print player")
  {
    InfoManager::Instance().debug("player");
  }
}

void ExampleAIModule::onReceiveText(BWAPI::Player player, std::string text)
{
  // Parse the received text
  Broodwar << player->getName() << " said \"" << text << "\"" << std::endl;
}

void ExampleAIModule::onPlayerLeft(BWAPI::Player player)
{
  // Interact verbally with the other players in the game by
  // announcing that the other player has left.
  Broodwar->sendText("GG, %s!", player->getName().c_str());
}

void ExampleAIModule::onNukeDetect(BWAPI::Position target)
{
  // Check if the target is a valid position
  if (target)
  {
    // if so, print the location of the nuclear strike target
    Broodwar << "Nuclear Launch Detected at " << target << std::endl;
  }
  else
  {
    // Otherwise, ask other players where the nuke is!
    Broodwar->sendText("Where's the nuke?");
  }

  // You can also retrieve all the nuclear missile targets using Broodwar->getNukeDots()!
}

void ExampleAIModule::onUnitDiscover(BWAPI::Unit unit)
{
  // notes: triggers after units are created, but not when morphed
  // printf("%s has been discovered.\n", unit->getType().c_str());

  if (!InfoManager::Instance().ownedByPlayer(unit) && !unit->getPlayer()->isNeutral())
  {
    InfoManager::Instance().addUnitInfo(unit);
  }
}

void ExampleAIModule::onUnitEvade(BWAPI::Unit unit)
{
}

void ExampleAIModule::onUnitShow(BWAPI::Unit unit)
{
}

void ExampleAIModule::onUnitHide(BWAPI::Unit unit)
{
}

void ExampleAIModule::onUnitCreate(BWAPI::Unit unit)
{
  // triggers when something is created, but not when morphed

  if (Broodwar->isReplay())
  {
    // if we are in a replay, then we will print out the build order of the structures
    if (unit->getType().isBuilding() && !unit->getPlayer()->isNeutral())
    {
      int seconds = Broodwar->getFrameCount() / 24;
      int minutes = seconds / 60;
      seconds %= 60;
      Broodwar->sendText("%.2d:%.2d: %s creates a %s", minutes, seconds, unit->getPlayer()->getName().c_str(), unit->getType().c_str());
    }
  }

  // update unit info
  if (!unit->getPlayer()->isNeutral()) InfoManager::Instance().addUnitInfo(unit);
}

void ExampleAIModule::onUnitDestroy(BWAPI::Unit unit)
{
  if (unit->getType() == UnitTypes::Zerg_Spawning_Pool)
  {
    hasPool = false;
  }
  if (unit->getType() == UnitTypes::Zerg_Drone)
  {
    --droneCount;
  }

  // print death report
  if (InfoManager::Instance().ownedByPlayer(unit))
  {
    Broodwar->sendText("Player %s has perished.", unit->getType().c_str());
  }
  else
  {
    Broodwar->sendText("Enemy %s has perished.", unit->getType().c_str());
  }

  // remove unit type from  unit list if population hits 0
  InfoManager::Instance().removeUnitInfo(unit);

  // reset reserve if it's killed
  if (unit->getID() == reserve) 
  {
    reserve = 0;
  }
}

void ExampleAIModule::onUnitMorph(BWAPI::Unit unit)
{
  if (Broodwar->isReplay())
  {
    // if we are in a replay, then we will print out the build order of the structures
    if (unit->getType().isBuilding() && !unit->getPlayer()->isNeutral())
    {
      int seconds = Broodwar->getFrameCount() / 24;
      int minutes = seconds / 60;
      seconds %= 60;
      Broodwar->sendText("%.2d:%.2d: %s morphs a %s", minutes, seconds, unit->getPlayer()->getName().c_str(), unit->getType().c_str());
    }
  }
  if (unit->getType() == UnitTypes::Zerg_Drone)
  {
    ++morphCount;
  }

  // print all morph args
  Broodwar->sendText("A %s has completed its morph.\n", unit->getType().c_str());
  
  // update unit info
  InfoManager::Instance().addUnitInfo(unit);
}

void ExampleAIModule::onUnitRenegade(BWAPI::Unit unit)
{
}

void ExampleAIModule::onSaveGame(std::string gameName)
{
  Broodwar << "The game was saved to \"" << gameName << "\"" << std::endl;
}

void ExampleAIModule::onUnitComplete(BWAPI::Unit unit)
{
  // spawning pool and hatchery flags
  if (unit->getType() == UnitTypes::Zerg_Hatchery)
  {
    //buildingHatch = 0;
    --droneCount;
  }
  if (unit->getType() == UnitTypes::Zerg_Spawning_Pool)
  {
    buildingPool = 0;
    hasPool = true;
    --droneCount;
  }
  if (unit->getType() == UnitTypes::Zerg_Overlord)
  {
    buildingOverlord = 0;
  }
  if (unit->getType() == UnitTypes::Zerg_Drone)
    ++droneCount;

  // update unit info
  if (!unit->getPlayer()->isNeutral()) InfoManager::Instance().addUnitInfo(unit);
}

// BWTA functions for map analysis
/* removed for crash
DWORD WINAPI AnalyzeThread()
{
//BWTA::analyze();
analyzed = true;
analysis_just_finished = true;
return 0;
} */

void ExampleAIModule::drawTerrainData()
{
  //we will iterate through all the base locations, and draw their outlines.
  for (const auto& baseLocation : BWTA::getBaseLocations()) {
    TilePosition p = baseLocation->getTilePosition();

    //draw outline of center location
    Position leftTop(p.x * TILE_SIZE, p.y * TILE_SIZE);
    Position rightBottom(leftTop.x + 4 * TILE_SIZE, leftTop.y + 3 * TILE_SIZE);
    Broodwar->drawBoxMap(leftTop, rightBottom, Colors::Blue);

    //draw a circle at each mineral patch
    for (const auto& mineral : baseLocation->getStaticMinerals()) {
      Broodwar->drawCircleMap(mineral->getInitialPosition(), 30, Colors::Cyan);
    }

    //draw the outlines of Vespene geysers
    for (const auto& geyser : baseLocation->getGeysers()) {
      TilePosition p1 = geyser->getInitialTilePosition();
      Position leftTop1(p1.x * TILE_SIZE, p1.y * TILE_SIZE);
      Position rightBottom1(leftTop1.x + 4 * TILE_SIZE, leftTop1.y + 2 * TILE_SIZE);
      Broodwar->drawBoxMap(leftTop1, rightBottom1, Colors::Orange);
    }

    //if this is an island expansion, draw a yellow circle around the base location
    if (baseLocation->isIsland()) {
      Broodwar->drawCircleMap(baseLocation->getPosition(), 80, Colors::Yellow);
    }
  }

  //we will iterate through all the regions and ...
  for (const auto& region : BWTA::getRegions()) {
    // draw the polygon outline of it in green
    BWTA::Polygon p = region->getPolygon();
    for (size_t j = 0; j < p.size(); ++j) {
      Position point1 = p[j];
      Position point2 = p[(j + 1) % p.size()];
      Broodwar->drawLineMap(point1, point2, Colors::Green);
    }
    // visualize the chokepoints with red lines
    for (auto const& chokepoint : region->getChokepoints()) {
      Position point1 = chokepoint->getSides().first;
      Position point2 = chokepoint->getSides().second;
      Broodwar->drawLineMap(point1, point2, Colors::Red);
    }
  }
}