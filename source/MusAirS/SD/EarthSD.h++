#pragma once

#include "MusAirS/Messenger/SDMessenger.h++"

#include "Mustard/Utility/NonMoveableBase.h++"

#include "G4VSensitiveDetector.hh"

#include <vector>

namespace MusAirS::inline SD {

class EarthSD : public Mustard::NonMoveableBase,
                public G4VSensitiveDetector {
public:
    using HitTrackIDDataType = std::vector<int>;

public:
    EarthSD(const G4String& sdName);

    auto DetectNeutrino() const -> auto { return fDetectNeutrino; }
    auto DetectNeutrino(bool val) -> void { fDetectNeutrino = val; }

    virtual auto Initialize(G4HCofThisEvent* hitsCollection) -> void override;
    virtual auto ProcessHits(G4Step* theStep, G4TouchableHistory*) -> G4bool override;
    virtual auto EndOfEvent(G4HCofThisEvent*) -> void override;

protected:
    bool fDetectNeutrino;

    HitTrackIDDataType fHitTrackIDData;

    SDMessenger::Register<EarthSD> fSDMessengerRegister;
};

} // namespace MusAirS::inline SD
