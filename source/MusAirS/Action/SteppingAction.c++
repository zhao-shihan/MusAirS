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

#include "MusAirS/Action/SteppingAction.h++"
#include "MusAirS/Analysis.h++"

#include "Mustard/IO/PrettyLog.h++"

#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4ParticleDefinition.hh"
#include "G4Step.hh"
#include "G4String.hh"
#include "G4Track.hh"
#include "G4TrackStatus.hh"
#include "G4UnitsTable.hh"
#include "G4VPhysicalVolume.hh"

#include "fmt/format.h"

#include <string_view>

namespace MusAirS::inline Action {

SteppingAction::SteppingAction() :
    PassiveSingleton{this},
    G4UserSteppingAction{},
    fGroundPhysicalVolume{} {}

auto SteppingAction::UserSteppingAction(const G4Step* step) -> void {
    const auto track{step->GetTrack()};

    if (track->GetCurrentStepNumber() >= fgNStepLimit) [[unlikely]] {
        track->SetTrackStatus(fStopAndKill);
        Mustard::PrintWarning(fmt::format(
            "Event {} track {} (PDG ID: {}) has more than {} steps, killing it (Ek = {}).",
            G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetEventID(),
            track->GetTrackID(), track->GetParticleDefinition()->GetPDGEncoding(), fgNStepLimit,
            std::string_view{G4String{G4BestUnit{track->GetKineticEnergy(), "Energy"}}}));
    }

    // Kill a track at the step that ends on the ground surface, so that its
    // final state is exactly the state at the ground surface.
    const auto currentVolume{track->GetVolume()};  // pre-step volume
    const auto nextVolume{track->GetNextVolume()}; // post-step volume
    if (nextVolume == fGroundPhysicalVolume or currentVolume == fGroundPhysicalVolume) [[unlikely]] {
        if (nextVolume == fGroundPhysicalVolume and currentVolume != fGroundPhysicalVolume) {
            Analysis::Instance().AddGroundHitTrackID(track->GetTrackID());
        }
        track->SetTrackStatus(fStopAndKill);
    }
}

} // namespace MusAirS::inline Action
