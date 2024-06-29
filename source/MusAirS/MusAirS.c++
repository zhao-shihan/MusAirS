#include "FTFP_BERT.hh"
#include "MusAirS/Action/ActionInitialization.h++"
#include "MusAirS/Action/DetectorConstruction.h++"
#include "MusAirS/Analysis.h++"
#include "MusAirS/DefaultMacro.h++"

#include "Mustard/Env/CLI/Geant4CLI.h++"
#include "Mustard/Env/MPIEnv.h++"
#include "Mustard/Extension/Geant4X/Interface/MPIExecutive.h++"
#include "Mustard/Extension/Geant4X/Run/MPIRunManager.h++"
#include "Mustard/Utility/UseXoshiro.h++"

#include "Randomize.hh"

auto main(int argc, char* argv[]) -> int {
    Mustard::Env::CLI::Geant4CLI<> cli;
    Mustard::Env::MPIEnv env{argc, argv, cli};

    Mustard::UseXoshiro<256> random;
    // First set random seed here
    cli.SeedRandomIfFlagged();

    // Mutually exclusive random seeds are distributed to all processes upon each BeamOn.
    Mustard::Geant4X::MPIRunManager runManager;
    // Physics lists
    runManager.SetUserInitialization(new FTFP_BERT{muc::to_underlying(env.VerboseLevel())});
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
