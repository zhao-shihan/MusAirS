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

#pragma once

#include "MusAirS/Messenger/ActionMessenger.h++"
#include "MusAirS/Messenger/SDMessenger.h++"

#include "Mustard/Env/Memory/PassiveSingleton.h++"

#include "G4UserSteppingAction.hh"

#include "gsl/gsl"

class G4ParticleDefinition;

namespace MusAirS::inline Action {

class SteppingAction final : public Mustard::Env::Memory::PassiveSingleton<SteppingAction>,
                             public G4UserSteppingAction {
public:
    SteppingAction();

    auto KillEMShower() const -> auto { return fKillEMShower; }
    auto KillNeutrino() const -> auto { return fKillNeutrino; }
    auto KillChargedPion() const -> auto { return fKillChargedPion; }

    auto KillEMShower(bool val) -> void;
    auto KillNeutrino(bool val) -> void;
    auto KillChargedPion(bool val) -> void;

    auto UserSteppingAction(const G4Step* step) -> void override;

private:
    auto SetPhysicalProcessActivation(gsl::not_null<G4ParticleDefinition*> particle, bool active) -> void;

private:
    bool fKillEMShower;
    bool fKillNeutrino;
    bool fKillChargedPion;

    ActionMessenger::Register<SteppingAction> fActionMessengerRegister;

    static constexpr auto fgNStepLimit{1000'0000};
};

} // namespace MusAirS::inline Action
