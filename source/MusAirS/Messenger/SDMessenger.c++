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

#include "MusAirS/Action/SteppingAction.h++"
#include "MusAirS/Messenger/SDMessenger.h++"

#include "G4UIcmdWithABool.hh"
#include "G4UIdirectory.hh"
#include "G4UImanager.hh"

#include "fmt/core.h"

namespace MusAirS::inline Messenger {

SDMessenger::SDMessenger() :
    SingletonMessenger{},
    fDirectory{},
    fDetectNeutrino{} {

    fDirectory = std::make_unique<G4UIdirectory>("/MusAirS/SD/");
    fDirectory->SetGuidance("MusAirS SD controller.");

    fDetectNeutrino = std::make_unique<G4UIcmdWithABool>("/MusAirS/SD/EarthSD/DetectNeutrino", this);
    fDetectNeutrino->SetGuidance("Set whether Earth SD is sensitive to neutrino.");
    fDetectNeutrino->SetParameterName("bool", false);
    fDetectNeutrino->AvailableForStates(G4State_Idle);
}

SDMessenger::~SDMessenger() = default;

auto SDMessenger::SetNewValue(G4UIcommand* command, G4String value) -> void {
    if (command == fDetectNeutrino.get()) {
        const auto detect{fDetectNeutrino->GetNewBoolValue(value)};
        Deliver<EarthSD>([&](auto&& r) {
            r.DetectNeutrino(detect);
        });
        G4UImanager::GetUIpointer()->ApplyCommand(fmt::format("/MusAirS/Action/KillNeutrino {}", detect ? "no" : "yes"));
    }
}

} // namespace MusAirS::inline Messenger
