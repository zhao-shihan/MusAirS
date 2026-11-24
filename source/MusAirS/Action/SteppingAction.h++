// -*- C++ -*-
//
// Copyright 2024-2026  Shihan Zhao
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

#pragma once

#include "Mustard/Env/ObjectRegistry/PassiveSingleton.h++"

#include "G4UserSteppingAction.hh"

class G4VPhysicalVolume;

namespace MusAirS::inline Action {

class SteppingAction final : public Mustard::Env::PassiveSingleton<SteppingAction>,
                             public G4UserSteppingAction {
public:
    SteppingAction();

    auto GroundPhysicalVolume() const -> auto { return fGroundPhysicalVolume; }
    auto GroundPhysicalVolume(const G4VPhysicalVolume* volume) -> void { fGroundPhysicalVolume = volume; }

    auto UserSteppingAction(const G4Step* step) -> void override;

private:
    const G4VPhysicalVolume* fGroundPhysicalVolume;

    static constexpr auto fgNStepLimit{1'000'000};
};

} // namespace MusAirS::inline Action
