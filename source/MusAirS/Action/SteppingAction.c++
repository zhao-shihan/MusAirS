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

#include "muc/math"

#include <string_view>

namespace MusAirS::inline Action {

SteppingAction::SteppingAction() :
    fKillNeutrino{true},
    fKillChargedPion{false},
    fActionMessengerRegister{this},
    fSDMessengerRegister{this} {}

auto SteppingAction::UserSteppingAction(const G4Step* step) -> void {
    const auto track{step->GetTrack()};
    const auto pdgID{track->GetParticleDefinition()->GetPDGEncoding()};
    const auto absPDGID{muc::abs(pdgID)};
    enum { nuE = 12,
           nuMu = 14,
           nuTau = 16,
           pi = 211 };

    auto mustKill{false};

    if (track->GetCurrentStepNumber() >= fgNStepLimit) [[unlikely]] {
        mustKill = true;
        Mustard::Env::PrintLnWarning("Warning: Event {} track {} (PDG ID: {}) has more than {} steps, killing it (Ek = {})",
                                     G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetEventID(),
                                     track->GetTrackID(), pdgID, fgNStepLimit,
                                     std::string_view{G4String{G4BestUnit{track->GetKineticEnergy(), "Energy"}}});
    }
    mustKill = fKillNeutrino and (absPDGID == nuE or absPDGID == nuMu or absPDGID == nuTau);
    mustKill = fKillChargedPion and absPDGID == pi;

    if (mustKill) {
        track->SetTrackStatus(fStopAndKill);
    }
}

} // namespace MusAirS::inline Action
