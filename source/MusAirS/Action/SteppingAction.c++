#include "MusAirS/Action/SteppingAction.h++"

#include "Mustard/Env/Print.h++"

#include "G4ParticleDefinition.hh"
#include "G4Step.hh"
#include "G4Track.hh"
#include "G4TrackStatus.hh"

namespace MusAirS::inline Action {

auto SteppingAction::UserSteppingAction(const G4Step* step) -> void {
    const auto track{step->GetTrack()};
    if (track->GetCurrentStepNumber() >= fgNStepLimit) [[unlikely]] {
        track->SetTrackStatus(fStopAndKill);
        Mustard::Env::PrintLnWarning("Warning: track {} (PDG ID: {}) has more than {} steps, killing it",
                                     track->GetTrackID(), track->GetParticleDefinition()->GetPDGEncoding(), fgNStepLimit);
    }
}

} // namespace MusAirS::inline Action
