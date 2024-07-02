#include "MusAirS/Action/DetectorConstruction.h++"
#include "MusAirS/Messenger/NumericMessenger.h++"

#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIdirectory.hh"

namespace MusAirS::inline Messenger {

NumericMessenger::NumericMessenger() :
    SingletonMessenger{},
    fDirectory{},
    fMinDriverStep{},
    fDeltaChord{} {

    fDirectory = std::make_unique<G4UIdirectory>("/MusAirS/Numeric/");
    fDirectory->SetGuidance("MusAirS numeric controller.");

    fMinDriverStep = std::make_unique<G4UIcmdWithADoubleAndUnit>("/MusAirS/Numeric/MinDriverStep", this);
    fMinDriverStep->SetGuidance("Set h_min for particle stepper driver in field.");
    fMinDriverStep->SetParameterName("hMin", false);
    fMinDriverStep->SetUnitCategory("Length");
    fMinDriverStep->AvailableForStates(G4State_PreInit);

    fDeltaChord = std::make_unique<G4UIcmdWithADoubleAndUnit>("/MusAirS/Numeric/DeltaChord", this);
    fDeltaChord->SetGuidance("Set delta chord.");
    fDeltaChord->SetParameterName("delta", false);
    fDeltaChord->SetUnitCategory("Length");
    fDeltaChord->AvailableForStates(G4State_PreInit);
}

NumericMessenger::~NumericMessenger() = default;

auto NumericMessenger::SetNewValue(G4UIcommand* command, G4String value) -> void {
    if (command == fMinDriverStep.get()) {
        Deliver<DetectorConstruction>([&](auto&& r) {
            r.MinDriverStep(fMinDriverStep->GetNewDoubleValue(value));
        });
    } else if (command == fDeltaChord.get()) {
        Deliver<DetectorConstruction>([&](auto&& r) {
            r.DeltaChord(fDeltaChord->GetNewDoubleValue(value));
        });
    }
}

} // namespace MusAirS::inline Messenger
