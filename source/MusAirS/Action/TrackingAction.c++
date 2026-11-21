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

#include "MusAirS/Action/TrackingAction.h++"
#include "MusAirS/Analysis.h++"

#include "Mustard/IO/PrettyLog.h++"

#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4ProcessType.hh"
#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4SteppingManager.hh"
#include "G4Track.hh"
#include "G4TrackingManager.hh"
#include "G4VProcess.hh"

#include "fmt/format.h"

#include <cmath>
#include <numbers>
#include <stdexcept>
#include <string_view>

namespace MusAirS::inline Action {

TrackingAction::TrackingAction() :
    PassiveSingleton{this},
    G4UserTrackingAction{},
    fVertexSigma{},
    fTrackData{},
    fMessengerRegister{this} {}

auto TrackingAction::PreUserTrackingAction(const G4Track* track) -> void {
    fVertexSigma = track->GetPolarization();
}

auto TrackingAction::PostUserTrackingAction(const G4Track* track) -> void {
    UpdateTrackData(*track);
}

auto TrackingAction::UpdateTrackData(const G4Track& g4Track) -> void {
    const auto creatorProcess{g4Track.GetCreatorProcess()};
    const auto trackID{g4Track.GetTrackID()};
    const auto& particle{*g4Track.GetDefinition()};
    const auto vertexEk{g4Track.GetVertexKineticEnergy()};
    const auto vertexMomentum{g4Track.GetVertexMomentumDirection() * std::sqrt(vertexEk * (vertexEk + 2 * particle.GetPDGMass()))};
    auto momentumDirection{g4Track.GetMomentumDirection()}; // guaranteed to be normalized
    if (momentumDirection.mag2() == 0) [[unlikely]] {       // few processes (e.g. G4GammaConversionToMuons) reset direction to zero
        momentumDirection = g4Track.GetStep()->GetPreStepPoint()->GetMomentumDirection();
    }
    auto& eventManager{*G4EventManager::GetEventManager()};
    const auto killerProcess{eventManager.GetTrackingManager()->GetSteppingManager()->GetfCurrentProcess()};

    const auto [it, inserted]{fTrackData.try_emplace(trackID, Mustard::Data::MakeArcTuple<Data::Track>())};
    if (not inserted) {
        Mustard::Throw<std::runtime_error>(fmt::format("Track {} appeared twice. This should not happen.", trackID));
    }

    auto& track{*it->second};
    F<"EvtID">(track) = eventManager.GetConstCurrentEvent()->GetEventID();
    F<"ParTrkID">(track) = creatorProcess ? g4Track.GetParentID() : -1;
    // F<"ParPDGID">(track) = <delay to event end analysis>
    F<"CreatProc">(track) = creatorProcess ? std::string_view{creatorProcess->GetProcessName()} : "|0>";
    F<"TrkID">(track) = trackID;
    F<"PDGID">(track) = particle.GetPDGEncoding();
    F<"t0">(track) = g4Track.GetGlobalTime() - g4Track.GetLocalTime();
    F<"x0">(track) = g4Track.GetVertexPosition();
    F<"Ek0">(track) = vertexEk;
    F<"p0">(track) = vertexMomentum;
    F<"Sigma0">(track) = fVertexSigma;
    F<"Helicity0">(track) = g4Track.GetVertexMomentumDirection() * fVertexSigma;
    F<"t">(track) = g4Track.GetGlobalTime();
    F<"x">(track) = g4Track.GetPosition();
    F<"Ek">(track) = g4Track.GetKineticEnergy();
    F<"p">(track) = g4Track.GetMomentum();
    F<"Sigma">(track) = g4Track.GetPolarization();
    F<"Helicity">(track) = momentumDirection * g4Track.GetPolarization();
    F<"Zenith">(track) = 1 + momentumDirection.z();
    F<"phi">(track) = std::atan2(momentumDirection.x(), momentumDirection.y()) + std::numbers::pi;
    F<"TrkLen">(track) = g4Track.GetTrackLength();
    F<"KillProc">(track) = killerProcess->GetProcessName(); // if killed at the ground, will be <0| in event end analysis
    F<"Weight">(track) = g4Track.GetWeight();
}

} // namespace MusAirS::inline Action
