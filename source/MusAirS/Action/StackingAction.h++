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

#include "MusAirS/Messenger/ActionMessenger.h++"

#include "Mustard/Env/ObjectRegistry/PassiveSingleton.h++"

#include "G4UserStackingAction.hh"

namespace MusAirS::inline Action {

class StackingAction final : public Mustard::Env::PassiveSingleton<StackingAction>,
                             public G4UserStackingAction {
public:
    StackingAction();

    auto KillEMShower() const -> auto { return fKillEMShower; }
    auto KillNeutrino() const -> auto { return fKillNeutrino; }
    auto KillChargedPion() const -> auto { return fKillChargedPion; }

    auto KillEMShower(bool val) -> void { fKillEMShower = val; }
    auto KillNeutrino(bool val) -> void { fKillNeutrino = val; }
    auto KillChargedPion(bool val) -> void { fKillChargedPion = val; }

    auto ClassifyNewTrack(const G4Track* track) -> G4ClassificationOfNewTrack override;

private:
    bool fKillEMShower;
    bool fKillNeutrino;
    bool fKillChargedPion;

    ActionMessenger::Register<StackingAction> fActionMessengerRegister;
};

} // namespace MusAirS::inline Action
