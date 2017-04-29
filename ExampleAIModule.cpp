#include "ExampleAIModule.h"
#include <iostream>

using namespace BWAPI;
using namespace Filter;

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
std::shared_ptr<BaseManager> mainManager;
std::shared_ptr<BuildExecutor> buildExecutor;
std::shared_ptr<ScoutManager> scoutManager;
// reserve a unit - set to some unlikely non-ID value to turn off
int reserve = 9000;

// TODO:  should be std::shared_ptr<UnitHandler> in the future but i'm too lazy to implement that inheritance yet
static std::unordered_map<Unit, std::shared_ptr<ZerglingHandler>> unitLookup;

void ExampleAIModule::onStart()
{
  // open a console for printing debug msgs
  FILE *df;
  AllocConsole();
  freopen_s(&df, "conin$", "r", stdin);
  freopen_s(&df, "conout$", "w", stdout);
  freopen_s(&df, "conout$", "w", stderr);
  printf("Debugging Window:\n");

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

  // make sure unitLookup doesn't have to rehash; may need more if we include buildings in it or build over 100 overlords e.e
  unitLookup.reserve(500);

  // Read & analyze map information with BWTA
  BWTA::readMap();
  BWTA::analyze();

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
  BWAPI::Unit m = nullptr; // was causing build issues without initialization
  for (auto &u : Broodwar->self()->getUnits())
  {
    if (u->getType().isResourceDepot())
    {
      m = u;
    }
  }
  // causing build issues for some reason w/o if != nullptr
  if (m != nullptr) mainManager = std::make_shared<BaseManager>(BaseManager(m));

  char bo[9];
  printf("Enter build order: ");
  if (scanf_s("%8s", bo) == 1)
    puts(bo);

  if (strcmp(bo, "fivepool"))
  {
    printf("setting build order to %s\n", bo);
    buildExecutor = std::make_shared<BuildExecutor>(BuildExecutor(BuildOrder::FIVEPOOL));
  }
  else if (strcmp(bo, "fourpool"))
  {
    printf("setting build order to %s\n", bo);
    buildExecutor = std::make_shared<BuildExecutor>(BuildExecutor(BuildOrder::FOURPOOL));
  }
  else if (strcmp(bo, "ninepool"))
  {
    printf("setting build order to %s\n", bo);
    buildExecutor = std::make_shared<BuildExecutor>(BuildExecutor(BuildOrder::NINEPOOL));
  }
  else
  {
    printf("setting build order to default fivepool, cause wtf did u write????\n");
    buildExecutor = std::make_shared<BuildExecutor>(BuildExecutor(BuildOrder::FIVEPOOL));
  }
  //buildExecutor = std::make_shared<BuildExecutor>(BuildExecutor(BuildOrder::FIVEPOOL));
  buildExecutor->giveMainManager(mainManager);

  scoutManager = std::make_shared<ScoutManager>(ScoutManager());

  // for some rng chance stuff
  srand(time(NULL));
}

void ExampleAIModule::onEnd(bool isWinner)
{
  // Called when the game ends
  if (isWinner)
  {
    // Log your win here!
  }

  InfoManager::Instance().debugUnits("player");
  InfoManager::Instance().debugUnits("enemy");

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

  /*
  // Basemanager Debug
  Broodwar->drawTextScreen(500, 20, "hasMain(): %d", mainManager->hasMain());
  Broodwar->drawTextScreen(500, 40, "numWorkers(): %d", mainManager->numWorkers());
  Broodwar->drawTextScreen(500, 60, "remainingMinerals(): %d", mainManager->remainingMinerals());
  Broodwar->drawTextScreen(500, 80, "remainingVespene(): %d", mainManager->remainingVespene());
  */

  // Player Unit Debug
  rowPos = 20;
  for (BWAPI::UnitType ut : InfoManager::Instance().getPlayerUnitTypes()){
    Broodwar->drawTextScreen(350, rowPos, "%s: %d", ut.c_str(), Broodwar->self()->allUnitCount(ut));
    rowPos += 20;
  }

  // Enemy Unit Debug
  rowPos = 40;
  for (BWAPI::UnitType ut : InfoManager::Instance().getEnemyUnitTypes()){
    Broodwar->drawTextScreen(500, rowPos, "%s: %d", ut.c_str(), InfoManager::Instance().numUnitType(ut, InfoManager::Instance().getEnemyUnitsInfo()));
    rowPos += 20;
  }
  Broodwar->drawTextScreen(500, rowPos, "# Known Enemy Units: %d", InfoManager::Instance().getEnemyUnitsInfo().size());

  // TESTING:  move back to end of onFrame()
  mainManager->update();
  buildExecutor->update();
  scoutManager->update();

  // if we've already discovered the main base and obliterated it, find more bases
  if (scoutManager->isMainExplored() && InfoManager::Instance().getEnemyBuildings().empty())
  {
    scoutManager->findEnemyBase();
    Broodwar->drawTextScreen(500, 20, "find moaaarrrr");
  }

  // do Zergling stuff through unitLookup (should later be all unit stuff in here)
  for (auto &unit : unitLookup)
  {
    // Ignore the unit if it no longer exists
    // Make sure to include this block when handling any Unit pointer!
    if (!unit.second->unit->exists())
      continue;

    // Ignore the unit if it has one of the following status ailments
    if (unit.second->unit->isLockedDown() || unit.second->unit->isMaelstrommed() || unit.second->unit->isStasised())
      continue;

    // Ignore the unit if it is in one of the following states
    if (unit.second->unit->isLoaded() || !unit.second->unit->isPowered() || unit.second->unit->isStuck())
      continue;

    // spoof implementation of a squad
    if (Broodwar->self()->allUnitCount(UnitTypes::Zerg_Zergling) > 1)
    {
        // added ad hoc way to have the units keep attacking (there's some sort of weird bug where they stop attacking even tho
        // there are still discovered enemy buildings at a base)
        if (!InfoManager::Instance().getEnemyBuildings().empty())
        {
          //Broodwar->sendText("attack %s @ %d, %d", InfoManager::Instance().getEnemyBuildings().at(0).getType().c_str(), InfoManager::Instance().getEnemyBuildings().at(0).getPosition().x, InfoManager::Instance().getEnemyBuildings().at(0).getPosition().y);
          unit.second->attack(InfoManager::Instance().getEnemyBuildings().at(0).getPosition());
        }
        else if (!BWAPI::Broodwar->isExplored(BWAPI::TilePosition(scoutManager->getFurthestStartingLocation())))
        {
          unit.second->attack(scoutManager->getFurthestStartingLocation());
        }

    }
    else
      unit.second->defend(mainManager->main->getPosition());

    // update units
    unit.second->update();
  }

  // Prevent spamming by only running our onFrame once every number of latency frames.
  // Latency frames are the number of frames before commands are processed.
  if (Broodwar->getFrameCount() % Broodwar->getLatencyFrames() != 0)
    return;

  // hack way of stopping anything else from being produced until we get a 9 pool
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
  /*
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
  */

  // Iterate through all the units that we own
  for (auto &u : Broodwar->self()->getUnits())
  {
    // Ignore the unit if it no longer exists
    // Make sure to include this block when handling any Unit pointer!
    if (!u->exists())
      continue;

    // update unit info
    InfoManager::Instance().updateUnitInfo(u);

    // Ignore the unit if it has one of the following status ailments
    if (u->isLockedDown() || u->isMaelstrommed() || u->isStasised())
      continue;

    // Ignore the unit if it is in one of the following states
    if (u->isLoaded() || !u->isPowered() || u->isStuck())
      continue;

    // Ignore the unit if it is incomplete or busy constructing
    //if ( !u->isCompleted() || u->isConstructing() )
    //  continue;

    // TESTING: reserve a unit
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
      //inefficient way to do this but whatevs, until we update our unitLookup to operate solely off unit create or whatever this'll make do
      // add to our unitLookup if it's not already in it
      if (unitLookup.count(u) == 0)
      {
        unitLookup[u] = std::make_shared<ZerglingHandler>(ZerglingHandler(u));
      }
    }
    // If the unit is a worker unit **but not the reserve!
    else if (u->getType().isWorker() && u->getID() != reserve)
    {
      // if our worker is gathering or idle but doesn't belong to mainManager
      if (!mainManager->containsWorker(u) && (u->isIdle() || u->isGatheringMinerals() || u->isGatheringGas()))
      {
        mainManager->addWorker(u);
      }
    }
    else if (u->getType() == UnitTypes::Zerg_Larva)
    {
      if (!buildExecutor->containsLarva(u))
        buildExecutor->addLarva(u);
    }
    else if (u->getType() == UnitTypes::Zerg_Overlord)
    {
      if (!scoutManager->containsUnit(u))
        scoutManager->addScout(u);
    }
    else if (scoutManager->getScoutConditionFunction(buildExecutor->getBuildOrder())(u, mainManager))
    {
      printf("took worker from dynamic scout condition function\n");
      scoutManager->addScout(mainManager->takeWorker());
    }
    /*else if (u->getType() == UnitTypes::Zerg_Spawning_Pool)
    {
      // fuck this hack gonna cause problems as soon as we swap builds TODO FIX POTENTIAL ISSUE ALERT ALERT i'm too tired i just wanna finish this class and not have it crash pls
      if (!u->isCompleted() && u->getRemainingBuildTime() < 400 && mainManager->numWorkers() > 5)
        scoutManager->addScout(mainManager->takeWorker());
    }
    else if (u->getType().isResourceDepot()) // A resource depot is a Command Center, Nexus, or Hatchery
    { 
      // Train Zerglings if we have a Spawning Pool with a 70% chance 
      if (hasPool && Broodwar->self()->minerals() >= UnitTypes::Zerg_Zergling.mineralPrice() + mineralBuffer && rand() % 10 > 2)
      {
        u->train(UnitTypes::Zerg_Zergling);
      }
      // Otherwise Train Drones
      else if (droneCount < 24 && Broodwar->self()->minerals() >= UnitTypes::Zerg_Drone.mineralPrice() + mineralBuffer)
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
      } // closure: failed to train idle unit
      
    }*/
  } // closure: unit iterator

  // clean up enemy list (should prolly move this later)
  //InfoManager::Instance().cleanUpEnemyTypes();

}

void ExampleAIModule::onSendText(std::string text)
{
  Broodwar->sendText("%s", text.c_str());

  // Make sure to use %s and pass the text as a parameter,
  // otherwise you may run into problems when you use the %(percent) character!

  //debug trigger
  if (text == "print enemy") {
    InfoManager::Instance().debugUnits("enemy");
  }
  else if (text == "print player")
  {
    InfoManager::Instance().debugUnits("player");
  }
  else if (text == "debug IM on")
  {
    InfoManager::Instance().setDebug(true);
  }
  else if (text == "debug IM off")
  {
    InfoManager::Instance().setDebug(false);
  }
  else if (text == "debug UI off")
  {
    UnitInfo::setDebug(false);
  }
  else if (text == "debug UI on")
  {
    UnitInfo::setDebug(true);
  }
  else if (text == "debug enemy buildings")
  {
    InfoManager::Instance().debugEnemyBuildings();
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

    // Scout Manager bullshit zzzz
    // don't trigger addenemybase unless the unit belongs to an enemy
    if (unit->getType().isResourceDepot())
      scoutManager->addEnemyBase(unit->getPosition());
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
  else if (!unit->getPlayer()->isNeutral())
  {
    Broodwar->sendText("Enemy %s has perished.", unit->getType().c_str());
  }

  // remove unit type from  unit list if population hits 0
  if (!unit->getPlayer()->isNeutral()) InfoManager::Instance().removeUnitInfo(unit);

  // reset reserve if it's killed
  if (unit->getID() == reserve) reserve = 0;
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
  //Broodwar->sendText("A %s has completed its morph.", unit->getType().c_str());

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