#include "MusAirS/Action/SteppingAction.h++"

#include "Mustard/Env/Print.h++"

#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4ParticleDefinition.hh"
#include "G4Step.hh"
#include "G4String.hh"
#include "G4Track.hh"
#include "G4TrackStatus.hh"
#include "G4UnitsTable.hh"

#include <string_view>

namespace MusAirS::inline Action {

auto SteppingAction::UserSteppingAction(const G4Step* step) -> void {
    const auto track{step->GetTrack()};
    if (track->GetCurrentStepNumber() >= fgNStepLimit) [[unlikely]] {
        track->SetTrackStatus(fStopAndKill);
        Mustard::Env::PrintLnWarning("Warning: Event {} track {} (PDG ID: {}) has more than {} steps, killing it (Ek = {})",
                                     G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetEventID(),
                                     track->GetTrackID(), track->GetParticleDefinition()->GetPDGEncoding(), fgNStepLimit,
                                     std::string_view{G4String{G4BestUnit{track->GetKineticEnergy(), "Energy"}}});
    }
}

} // namespace MusAirS::inline Action
