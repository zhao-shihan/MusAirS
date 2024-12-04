#include "MusAirS/Analysis.h++"
#include "MusAirS/SD/EarthSD.h++"

#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4RotationMatrix.hh"
#include "G4SDManager.hh"
#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4String.hh"
#include "G4TrackStatus.hh"

#include "muc/math"

namespace MusAirS::inline SD {

EarthSD::EarthSD(const G4String& sdName) :
    Mustard::NonMoveableBase{},
    G4VSensitiveDetector{sdName},
    fDetectNeutrino{false},
    fHitTrackIDData{},
    fSDMessengerRegister{this} {
    collectionName.insert(sdName + "HC");
}

auto EarthSD::Initialize(G4HCofThisEvent*) -> void {
    fHitTrackIDData.clear();
}

auto EarthSD::ProcessHits(G4Step* theStep, G4TouchableHistory*) -> G4bool {
    // just kill the track underground
    theStep->GetTrack()->SetTrackStatus(fStopAndKill);

    const auto& step{*theStep};
    if (not step.IsFirstStepInVolume()) { return false; } // ensure first step

    const auto& track{*step.GetTrack()};
    if (track.GetCurrentStepNumber() < 2) { return false; } // ensure track coming from outside

    const auto& particle{*track.GetDefinition()};
    const auto pdgID{particle.GetPDGEncoding()};

    if (fDetectNeutrino) { goto SkipIgnoringNeutrino; }
    enum { vE = 12,
           vMu = 14,
           vTau = 16 };
    if (const auto absPDGID{muc::abs(pdgID)};
        absPDGID == vE or absPDGID == vMu or absPDGID == vTau) { // ignoring neutrino
        return false;
    }
SkipIgnoringNeutrino:

    fHitTrackIDData.emplace_back(track.GetTrackID());

    return true;
}

auto EarthSD::EndOfEvent(G4HCofThisEvent*) -> void {
    Analysis::Instance().SubmitEarthSDHitTrackID(fHitTrackIDData);
}

} // namespace MusAirS::inline SD
