#include "MusAirS/Action/SteppingAction.h++"
#include "MusAirS/Messenger/SDMessenger.h++"

#include "G4UIcmdWithABool.hh"
#include "G4UIdirectory.hh"
#include "G4UImanager.hh"

#include "fmt/core.h"

namespace MusAirS::inline Messenger {

SDMessenger::SDMessenger() :
    SingletonMessenger{},
    fDirectory{},
    fDetectNeutrino{} {

    fDirectory = std::make_unique<G4UIdirectory>("/MusAirS/SD/");
    fDirectory->SetGuidance("MusAirS SD controller.");

    fDetectNeutrino = std::make_unique<G4UIcmdWithABool>("/MusAirS/SD/EarthSD/DetectNeutrino", this);
    fDetectNeutrino->SetGuidance("Set whether Earth SD is sensitive to neutrino.");
    fDetectNeutrino->SetParameterName("bool", false);
    fDetectNeutrino->AvailableForStates(G4State_Idle);
}

SDMessenger::~SDMessenger() = default;

auto SDMessenger::SetNewValue(G4UIcommand* command, G4String value) -> void {
    if (command == fDetectNeutrino.get()) {
        const auto detect{fDetectNeutrino->GetNewBoolValue(value)};
        Deliver<EarthSD>([&](auto&& r) {
            r.DetectNeutrino(detect);
        });
        G4UImanager::GetUIpointer()->ApplyCommand(fmt::format("/MusAirS/Action/KillNeutrino {}", detect ? "yes" : "no"));
    }
}

} // namespace MusAirS::inline Messenger
