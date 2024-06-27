#pragma once

#include "MusAirS/Hit/EarthHit.h++"

#include "Mustard/Utility/NonMoveableBase.h++"

#include "G4VSensitiveDetector.hh"

namespace MusAirS::inline SD {

class EarthSD : public Mustard::NonMoveableBase,
                  public G4VSensitiveDetector {
public:
    EarthSD(const G4String& sdName);

    virtual auto Initialize(G4HCofThisEvent* hitsCollection) -> void override;
    virtual auto ProcessHits(G4Step* theStep, G4TouchableHistory*) -> G4bool override;
    virtual auto EndOfEvent(G4HCofThisEvent*) -> void override;

protected:
    EarthHitCollection* fHitsCollection;
};

} // namespace MusAirS::inline SD
