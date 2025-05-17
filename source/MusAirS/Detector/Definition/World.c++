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

#include "MusAirS/Detector/Definition/World.h++"
#include "MusAirS/Detector/Description/World.h++"

#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4NistManager.hh"
#include "G4PVPlacement.hh"
#include "G4Transform3D.hh"

namespace MusAirS::Detector::Definition {

auto World::Construct(bool checkOverlaps) -> void {
    const auto& world{Description::World::Instance()};

    const auto solid{Make<G4Box>(
        world.Name(),
        world.Width() / 2,
        world.Width() / 2,
        world.MaxHeight())};
    const auto logic{Make<G4LogicalVolume>(
        solid,
        G4NistManager::Instance()->FindOrBuildMaterial("G4_Galactic"),
        world.Name())};
    Make<G4PVPlacement>(
        G4Transform3D{},
        logic,
        world.Name(),
        nullptr,
        false,
        0,
        checkOverlaps);
}

} // namespace MusAirS::Detector::Definition
