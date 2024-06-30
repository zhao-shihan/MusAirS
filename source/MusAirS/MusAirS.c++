#include "MusAirS/Action/ActionInitialization.h++"
#include "MusAirS/Action/DetectorConstruction.h++"
#include "MusAirS/Analysis.h++"
#include "MusAirS/DefaultMacro.h++"
#include "MusAirS/Detector/Description/World.h++"

#include "Mustard/Env/CLI/Geant4CLI.h++"
#include "Mustard/Env/CLI/Module/Geant4ReferencePhysicsListModule.h++"
#include "Mustard/Env/MPIEnv.h++"
#include "Mustard/Extension/Geant4X/Interface/MPIExecutive.h++"
#include "Mustard/Extension/Geant4X/Run/MPIRunManager.h++"
#include "Mustard/Utility/UseXoshiro.h++"

#include "G4GeometryManager.hh"

#include <algorithm>

auto main(int argc, char* argv[]) -> int {
    // Use default Geant4 CLI and extends it with reference physics list interface
    Mustard::Env::CLI::Geant4CLI<Mustard::Env::CLI::Geant4ReferencePhysicsListModule<>> cli;
    Mustard::Env::MPIEnv env{argc, argv, cli};

    Mustard::UseXoshiro<256> random;
    // First set random seed here
    cli.SeedRandomIfFlagged();

    // Set geometry tolerance
    const auto& world{MusAirS::Detector::Description::World::Instance()};
    G4GeometryManager::GetInstance()->SetWorldMaximumExtent(std::max(2 * world.MaxHeight(), world.Width()));

    // Mutually exclusive random seeds are distributed to all processes upon each BeamOn.
    Mustard::Geant4X::MPIRunManager runManager;
    // Physics lists
    runManager.SetUserInitialization(cli.PhysicsList());
    // Register detector construction
    runManager.SetUserInitialization(new MusAirS::DetectorConstruction{env.VerboseLevelReach<'I'>()});
    // Register action initialization, including run action, event action, etc.
    runManager.SetUserInitialization(new MusAirS::ActionInitialization);
    // Instanitiate analysis
    MusAirS::Analysis anaylsis;

    // Start UI session
    Mustard::Geant4X::MPIExecutive{}.StartSession(cli, MusAirS::defaultMacro);

    return EXIT_SUCCESS;
}
