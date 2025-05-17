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

#include "MusAirS/Physics/SpinDecayPhysicsWithKaon.h++"

#include "G4KaonMinus.hh"
#include "G4KaonPlus.hh"
#include "G4KaonZeroLong.hh"
#include "G4PionPlus.hh"
#include "G4ProcessManager.hh"
#include "G4ProcessTable.hh"

namespace MusAirS::inline Physics {

auto SpinDecayPhysicsWithKaon::ConstructProcess() -> void {
    G4SpinDecayPhysics::ConstructProcess();

    const auto processTable{G4ProcessTable::GetProcessTable()};
    const auto RegisterDecayMakeSpin{
        [&, decayMakeSpin = processTable->FindProcess("Decay", G4PionPlus::Definition())](auto&& particle) {
            const auto decay{processTable->FindProcess("Decay", particle)};
            const auto manager{particle->GetProcessManager()};
            if (manager) {
                if (decay) { manager->RemoveProcess(decay); }
                manager->AddProcess(decayMakeSpin);
                // set ordering for PostStepDoIt and AtRestDoIt
                manager->SetProcessOrdering(decayMakeSpin, idxPostStep);
                manager->SetProcessOrdering(decayMakeSpin, idxAtRest);
            }
        }};
    RegisterDecayMakeSpin(G4KaonPlus::Definition());
    RegisterDecayMakeSpin(G4KaonMinus::Definition());
    RegisterDecayMakeSpin(G4KaonZeroLong::Definition());
}

} // namespace MusAirS::inline Physics
