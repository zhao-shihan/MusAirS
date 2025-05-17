#include "MusAirS/Action/PrimaryGeneratorAction.h++"
#include "MusAirS/Action/TrackingAction.h++"
#include "MusAirS/Analysis.h++"

#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4ProcessType.hh"
#include "G4SteppingManager.hh"
#include "G4Track.hh"
#include "G4TrackingManager.hh"
#include "G4VProcess.hh"

#include "fmt/core.h"

#include <stdexcept>
#include <string_view>
#include <vector>

namespace MusAirS::inline Action {

TrackingAction::TrackingAction() :
    PassiveSingleton{this},
    G4UserTrackingAction{},
    fVertexSigma{},
    fMemoryPool{},
    fTrackData{&fMemoryPool},
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
    auto& eventManager{*G4EventManager::GetEventManager()};
    const auto killerProcess{eventManager.GetTrackingManager()->GetSteppingManager()->GetfCurrentProcess()};

    const auto [it, inserted]{fTrackData.insert({trackID, {}})};
    if (not inserted) {
        throw std::runtime_error{fmt::format("MusAirS::TrackingAction: Track {} appeared twice. This should not happen.", trackID)};
    }

    auto& track{it->second};
    Get<"EvtID">(track) = eventManager.GetConstCurrentEvent()->GetEventID();
    Get<"ParTrkID">(track) = creatorProcess ? g4Track.GetParentID() : -1;
    // Get<"ParPDGID">(track) = <delay to event end analysis>
    Get<"CreatProc">(track) = creatorProcess ? std::string_view{creatorProcess->GetProcessName()} : "|0>";
    Get<"TrkID">(track) = trackID;
    Get<"PDGID">(track) = particle.GetPDGEncoding();
    Get<"t0">(track) = g4Track.GetGlobalTime() - g4Track.GetLocalTime();
    Get<"x0">(track) = g4Track.GetVertexPosition();
    Get<"Ek0">(track) = vertexEk;
    Get<"p0">(track) = vertexMomentum;
    Get<"Sigma0">(track) = fVertexSigma;
    Get<"Helicity0">(track) = g4Track.GetVertexMomentumDirection() * fVertexSigma;
    Get<"t">(track) = g4Track.GetGlobalTime();
    Get<"x">(track) = g4Track.GetPosition();
    Get<"Ek">(track) = g4Track.GetKineticEnergy();
    Get<"p">(track) = g4Track.GetMomentum();
    Get<"Sigma">(track) = g4Track.GetPolarization();
    Get<"Helicity">(track) = g4Track.GetMomentumDirection() * g4Track.GetPolarization();
    // Get<"Zenith">(track) = <delay to event end analysis>
    // Get<"phi">(track) = <delay to event end analysis>
    Get<"TrkLen">(track) = g4Track.GetTrackLength();
    Get<"KillProc">(track) = killerProcess->GetProcessName(); // if hit EarthSD, will be <0| in event end analysis
    Get<"Weight">(track) = g4Track.GetWeight();
}

} // namespace MusAirS::inline Action
