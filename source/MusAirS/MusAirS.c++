// -*- C++ -*-
//
// Copyright 2024-2025  Shihan Zhao
//
// This file is part of MusAirS, an air shower simulation tool.
//
// MusAirS is free software: you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the Free Software
// Foundation, either version 3 of the License, or (at your option) any later
// version.
//
// MusAirS is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with
// MusAirS. If not, see <https://www.gnu.org/licenses/>.

#include "MusAirS/Action/ActionInitialization.h++"
#include "MusAirS/Action/DetectorConstruction.h++"
#include "MusAirS/Analysis.h++"
#include "MusAirS/DefaultMacro.h++"
#include "MusAirS/Physics/SpinDecayPhysicsWithKaon.h++"
#include "MusAirS/Version.h++"

#include "Mustard/Env/CLI/Geant4CLI.h++"
#include "Mustard/Env/CLI/Module/Geant4ReferencePhysicsListModule.h++"
#include "Mustard/Env/MPIEnv.h++"
#include "Mustard/Geant4X/Interface/MPIExecutive.h++"
#include "Mustard/Geant4X/Run/MPIRunManager.h++"
#include "Mustard/IO/Print.h++"
#include "Mustard/Utility/LiteralUnit.h++"
#include "Mustard/Utility/UseXoshiro.h++"

#include "G4EmStandardPhysics_option1.hh"
#include "G4GeometryManager.hh"

#include "muc/utility"

#include "fmt/color.h"

#include <algorithm>

auto main(int argc, char* argv[]) -> int {
    // Use default Geant4 CLI and extends it with reference physics list interface
    Mustard::Env::CLI::Geant4CLI<Mustard::Env::CLI::Geant4ReferencePhysicsListModule<"FTFP_BERT">> cli;
    Mustard::Env::MPIEnv env{argc, argv, cli};

    Mustard::Print(fmt::emphasis::bold, R"(
--------------------------------------------------
 |\/|   _ /|.  (`  An air shower simulation tool
 |  |L|_\/-|||`_)  Version {}
 Copyright 2024-2025  Shihan Zhao
--------------------------------------------------
)",
                   MUSAIRS_VERSION_STRING);

    Mustard::UseXoshiro<256> random;
    // First set random seed here
    cli.SeedRandomIfFlagged();

    // Set geometry tolerance
    using namespace Mustard::LiteralUnit::Length;
    G4GeometryManager::GetInstance()->SetWorldMaximumExtent(100_km); // just a scale

    // Mutually exclusive random seeds are distributed to all processes upon each BeamOn.
    Mustard::Geant4X::MPIRunManager runManager;

    // Physics lists
    const auto physicsList{cli.PhysicsList()};
    physicsList->ReplacePhysics(new G4EmStandardPhysics_option1{muc::to_underlying(env.VerboseLevel())}); // force to EMV
    physicsList->RegisterPhysics(new MusAirS::SpinDecayPhysicsWithKaon{muc::to_underlying(env.VerboseLevel())});
    runManager.SetUserInitialization(physicsList);

    // Register detector construction
    runManager.SetUserInitialization(new MusAirS::DetectorConstruction{env.VerboseLevelReach<'I'>()});

    // Register action initialization, including run action, event action, etc.
    runManager.SetUserInitialization(new MusAirS::ActionInitialization);

    // Instantiate analysis
    MusAirS::Analysis analysis;

    // Start UI session
    Mustard::Geant4X::MPIExecutive{}.StartSession(cli, MusAirS::defaultMacro);

    return EXIT_SUCCESS;
}
