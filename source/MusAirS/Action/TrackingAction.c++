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

#include <cassert>
#include <string_view>
#include <vector>

namespace MusAirS::inline Action {

TrackingAction::TrackingAction() :
    PassiveSingleton{},
    G4UserTrackingAction{},
    fSaveDecayVertexData{true},
    fDecayVertexData{},
    fMessengerRegister{this} {}

auto TrackingAction::PostUserTrackingAction(const G4Track* track) -> void {
    if (fSaveDecayVertexData) { UpdateDecayVertexData(*track); }
}

auto TrackingAction::UpdateDecayVertexData(const G4Track& track) -> void {
    if (auto& eventManager{*G4EventManager::GetEventManager()};
        eventManager.GetTrackingManager()
            ->GetSteppingManager()
            ->GetfCurrentProcess()
            ->GetProcessType() == fDecay) {
        const auto creatorProcess{track.GetCreatorProcess()};
        std::vector<int> secondaryPDGID;
        secondaryPDGID.reserve(track.GetStep()->GetSecondary()->size());
        for (auto&& sec : *track.GetStep()->GetSecondary()) {
            secondaryPDGID.emplace_back(sec->GetParticleDefinition()->GetPDGEncoding());
        }
        auto& vertex{fDecayVertexData.emplace_back()};
        Get<"EvtID">(vertex) = eventManager.GetConstCurrentEvent()->GetEventID();
        Get<"TrkID">(vertex) = track.GetTrackID();
        Get<"PDGID">(vertex) = track.GetParticleDefinition()->GetPDGEncoding();
        Get<"CreatProc">(vertex) = creatorProcess ? std::string_view{creatorProcess->GetProcessName()} : "|0>";
        Get<"t">(vertex) = track.GetGlobalTime();
        Get<"x">(vertex) = track.GetPosition();
        Get<"Ek">(vertex) = track.GetKineticEnergy();
        Get<"p">(vertex) = track.GetMomentum();
        Get<"SecPDGID">(vertex) = std::move(secondaryPDGID);
    }
}

} // namespace MusAirS::inline Action
