#include "MusAirS/Action/SteppingAction.h++"
#include "MusAirS/Messenger/ActionMessenger.h++"

#include "G4UIcmdWithABool.hh"
#include "G4UIdirectory.hh"

namespace MusAirS::inline Messenger {

ActionMessenger::ActionMessenger() :
    SingletonMessenger{},
    fDirectory{},
    fKillChargedPion{} {

    fDirectory = std::make_unique<G4UIdirectory>("/MusAirS/Action/");
    fDirectory->SetGuidance("MusAirS user action controller.");

    fKillChargedPion = std::make_unique<G4UIcmdWithABool>("/MusAirS/Action/KillChargedPion", this);
    fKillChargedPion->SetGuidance("Charged pions will be killed if set.");
    fKillChargedPion->SetParameterName("bool", false);
    fKillChargedPion->AvailableForStates(G4State_Idle);
}

ActionMessenger::~ActionMessenger() = default;

auto ActionMessenger::SetNewValue(G4UIcommand* command, G4String value) -> void {
    if (command == fKillChargedPion.get()) {
        Deliver<SteppingAction>([&](auto&& r) {
            r.KillChargedPion(fKillChargedPion->GetNewBoolValue(value));
        });
    }
}

} // namespace MusAirS::inline Messenger
