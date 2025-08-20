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

#include "Mustard/IO/PrettyLog.h++"

#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"
#include "G4ProcessTable.hh"
#include "G4ProcessType.hh"
#include "G4RunManager.hh"
#include "G4Step.hh"
#include "G4String.hh"
#include "G4Track.hh"
#include "G4TrackStatus.hh"
#include "G4UnitsTable.hh"

#include "muc/math"

#include "fmt/core.h"

#include <string_view>

namespace MusAirS::inline Action {

SteppingAction::SteppingAction() :
    PassiveSingleton{this},
    G4UserSteppingAction{},
    fKillEMShower{false},
    fKillNeutrino{true},
    fKillChargedPion{false},
    fActionMessengerRegister{this} {}

auto SteppingAction::KillEMShower(bool val) -> void {
    fKillEMShower = val;
    SetPhysicalProcessActivation(G4Gamma::Definition(), not val);
    SetPhysicalProcessActivation(G4Electron::Definition(), not val);
    SetPhysicalProcessActivation(G4Positron::Definition(), not val);
}

auto SteppingAction::KillNeutrino(bool val) -> void {
    fKillNeutrino = val;
    SetPhysicalProcessActivation(G4NeutrinoE::Definition(), not val);
    SetPhysicalProcessActivation(G4AntiNeutrinoE::Definition(), not val);
    SetPhysicalProcessActivation(G4NeutrinoMu::Definition(), not val);
    SetPhysicalProcessActivation(G4AntiNeutrinoMu::Definition(), not val);
    SetPhysicalProcessActivation(G4NeutrinoTau::Definition(), not val);
    SetPhysicalProcessActivation(G4AntiNeutrinoTau::Definition(), not val);
}

auto SteppingAction::KillChargedPion(bool val) -> void {
    fKillChargedPion = val;
    SetPhysicalProcessActivation(G4PionPlus::Definition(), not val);
    SetPhysicalProcessActivation(G4PionMinus::Definition(), not val);
}

auto SteppingAction::UserSteppingAction(const G4Step* step) -> void {
    const auto track{step->GetTrack()};
    const auto pdgID{track->GetParticleDefinition()->GetPDGEncoding()};

    if (track->GetCurrentStepNumber() >= fgNStepLimit) [[unlikely]] {
        track->SetTrackStatus(fStopAndKill);
        Mustard::PrintWarning(
            fmt::format("Warning: Event {} track {} (PDG ID: {}) has more than {} steps, killing it (Ek = {})",
                        G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetEventID(),
                        track->GetTrackID(), pdgID, fgNStepLimit,
                        std::string_view{G4String{G4BestUnit{track->GetKineticEnergy(), "Energy"}}}));
        return;
    }

    auto mustKill{false};
    enum { gamma = 22,
           e = 11,
           nuE = 12,
           nuMu = 14,
           nuTau = 16,
           pi = 211 };
    const auto absPDGID{muc::abs(pdgID)};
    mustKill = mustKill or (fKillEMShower and (absPDGID == gamma or absPDGID == e));
    mustKill = mustKill or (fKillNeutrino and (absPDGID == nuE or absPDGID == nuMu or absPDGID == nuTau));
    mustKill = mustKill or (fKillChargedPion and absPDGID == pi);
    if (mustKill) {
        track->SetTrackStatus(fKillTrackAndSecondaries);
    }
}

auto SteppingAction::SetPhysicalProcessActivation(gsl::not_null<G4ParticleDefinition*> particle, bool active) -> void {
    const auto processTable{G4ProcessTable::GetProcessTable()};
    processTable->SetProcessActivation(fElectromagnetic, particle, active);
    processTable->SetProcessActivation(fOptical, particle, active);
    processTable->SetProcessActivation(fHadronic, particle, active);
    processTable->SetProcessActivation(fPhotolepton_hadron, particle, active);
    processTable->SetProcessActivation(fDecay, particle, active);
    processTable->SetProcessActivation(fGeneral, particle, active);
    processTable->SetProcessActivation(fParameterisation, particle, active);
    processTable->SetProcessActivation(fPhonon, particle, active);
    processTable->SetProcessActivation(fUCN, particle, active);
    G4RunManager::GetRunManager()->PhysicsHasBeenModified();
}

} // namespace MusAirS::inline Action
