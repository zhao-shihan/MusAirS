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

#include "MusAirS/Action/DetectorConstruction.h++"
#include "MusAirS/Detector/Definition/Atmosphere.h++"
#include "MusAirS/Detector/Definition/Earth.h++"
#include "MusAirS/Detector/Definition/World.h++"
#include "MusAirS/Messenger/DetectorMessenger.h++"
#include "MusAirS/SD/EarthSD.h++"

#include "Mustard/Detector/Definition/DefinitionBase.h++"
#include "Mustard/Detector/Field/AsG4Field.h++"
#include "Mustard/Detector/Field/UniformMagneticField.h++"
#include "Mustard/Utility/LiteralUnit.h++"

#include "G4ChordFinder.hh"
#include "G4FieldManager.hh"
#include "G4InterpolationDriver.hh"
#include "G4Mag_SpinEqRhs.hh"
#include "G4TDormandPrince45.hh"
#include "G4ThreeVector.hh"

#include "gsl/gsl"

#include <utility>

namespace MusAirS::inline Action {

using namespace Mustard::LiteralUnit::Length;

DetectorConstruction::DetectorConstruction(bool checkOverlap) :
    PassiveSingleton{this},
    G4VUserDetectorConstruction{},
    fCheckOverlap{checkOverlap},
    fMinDriverStep{1_cm},
    fDeltaChord{1_m},
    fWorld{},
    fNumericMessengerRegister{this} {
    DetectorMessenger::EnsureInstantiation();
}

auto DetectorConstruction::Construct() -> G4VPhysicalVolume* {
    // Construct volumes

    fWorld = std::make_unique<Detector::Definition::World>();
    auto& earth{fWorld->NewDaughter<Detector::Definition::Earth>(fCheckOverlap)};
    fWorld->NewDaughter<Detector::Definition::Atmosphere>(fCheckOverlap);

    // Register SD

    earth.RegisterSD(new SD::EarthSD{"EarthSD"});

    // Register field

    using Field = Mustard::Detector::Field::AsG4Field<Mustard::Detector::Field::UniformMagneticField>;
    using Equation = G4Mag_SpinEqRhs;
    using Stepper = G4TDormandPrince45<Equation, 12>;
    using Driver = G4InterpolationDriver<Stepper>;

    const auto field{new Field{Detector::Description::Field::Instance().MagneticField()}};
    const auto equation{new Equation{field}}; // clang-format off
    const auto stepper{new Stepper{equation}};
    const auto driver{new Driver{fMinDriverStep, stepper, 12}}; // clang-format on
    const auto chordFinder{new G4ChordFinder{driver}};
    chordFinder->SetDeltaChord(fDeltaChord);
    fWorld->RegisterField(std::make_unique<G4FieldManager>(field, chordFinder), false);

    return fWorld->PhysicalVolume();
}

} // namespace MusAirS::inline Action
