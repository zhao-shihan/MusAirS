#pragma once

#include "MusAirS/Detector/Description/Atmosphere.h++"
#include "MusAirS/Detector/Description/Earth.h++"
#include "MusAirS/Detector/Description/Field.h++"
#include "MusAirS/Messenger/NumericMessenger.h++"
#include "MusAirS/Detector/Description/World.h++"

#include "Mustard/Env/Memory/PassiveSingleton.h++"

#include "G4VUserDetectorConstruction.hh"

#include <memory>

namespace Mustard::Detector::Definition {
class DefinitionBase;
} // namespace Mustard::Detector::Definition

namespace MusAirS {

inline namespace SD {
class EarthSD;
} // namespace SD

inline namespace Action {

class DetectorConstruction final : public Mustard::Env::Memory::PassiveSingleton<DetectorConstruction>,
                                   public G4VUserDetectorConstruction {
public:
    DetectorConstruction(bool checkOverlap);

    auto MinDriverStep(double val) -> void { fMinDriverStep = val; }
    auto DeltaChord(double val) -> void { fDeltaChord = val; }

    auto Construct() -> G4VPhysicalVolume* override;

public:
    using DescriptionInUse = std::tuple<Detector::Description::Atmosphere,
                                        Detector::Description::Earth,
                                        Detector::Description::Field,
                                        Detector::Description::World>;

private:
    bool fCheckOverlap;

    double fMinDriverStep;
    double fDeltaChord;

    std::unique_ptr<Mustard::Detector::Definition::DefinitionBase> fWorld;

    NumericMessenger::Register<DetectorConstruction> fNumericMessengerRegister;
};

} // namespace Action

} // namespace MusAirS
