#include "MusAirS/Action/SteppingAction.h++"
#include "MusAirS/Messenger/ActionMessenger.h++"

#include "G4ApplicationState.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWithoutParameter.hh"
#include "G4UIdirectory.hh"

namespace MusAirS::inline Messenger {

ActionMessenger::ActionMessenger() :
    SingletonMessenger{},
    fDirectory{},
    fSwitchToPCR{},
    fSwitchToGPSX{},
    fKillEMShower{},
    fKillNeutrino{},
    fKillChargedPion{} {

    fDirectory = std::make_unique<G4UIdirectory>("/MusAirS/Action/");
    fDirectory->SetGuidance("MusAirS user action controller.");

    fSwitchToPCR = std::make_unique<G4UIcmdWithoutParameter>("/MusAirS/Action/SwitchToPCR", this);
    fSwitchToPCR->SetGuidance("Switch to MusAirS::PrimaryCosmicRayGenerator in primary generator action.");
    fSwitchToPCR->AvailableForStates(G4State_Idle);

    fSwitchToGPSX = std::make_unique<G4UIcmdWithoutParameter>("/MusAirS/Action/SwitchToGPSX", this);
    fSwitchToGPSX->SetGuidance("Switch to extended general particle source in primary generator action.");
    fSwitchToGPSX->AvailableForStates(G4State_Idle);

    fKillEMShower = std::make_unique<G4UIcmdWithABool>("/MusAirS/Action/KillEMShower", this);
    fKillEMShower->SetGuidance("EM shower (e+, e-, gamma) will be killed if set.");
    fKillEMShower->SetParameterName("bool", false);
    fKillEMShower->AvailableForStates(G4State_Idle);

    fKillNeutrino = std::make_unique<G4UIcmdWithABool>("/MusAirS/Action/KillNeutrino", this);
    fKillNeutrino->SetGuidance("Neutrino will be killed if set.");
    fKillNeutrino->SetParameterName("bool", false);
    fKillNeutrino->AvailableForStates(G4State_Idle);

    fKillChargedPion = std::make_unique<G4UIcmdWithABool>("/MusAirS/Action/KillChargedPion", this);
    fKillChargedPion->SetGuidance("Charged pions will be killed if set.");
    fKillChargedPion->SetParameterName("bool", false);
    fKillChargedPion->AvailableForStates(G4State_Idle);
}

ActionMessenger::~ActionMessenger() = default;

auto ActionMessenger::SetNewValue(G4UIcommand* command, G4String value) -> void {
    if (command == fSwitchToPCR.get()) {
        Deliver<PrimaryGeneratorAction>([&](auto&& r) {
            r.SwitchToPCR();
        });
    } else if (command == fSwitchToGPSX.get()) {
        Deliver<PrimaryGeneratorAction>([&](auto&& r) {
            r.SwitchToGPSX();
        });
    } else if (command == fKillEMShower.get()) {
        Deliver<SteppingAction>([&](auto&& r) {
            r.KillEMShower(fKillEMShower->GetNewBoolValue(value));
        });
    } else if (command == fKillNeutrino.get()) {
        Deliver<SteppingAction>([&](auto&& r) {
            r.KillNeutrino(fKillNeutrino->GetNewBoolValue(value));
        });
    } else if (command == fKillChargedPion.get()) {
        const auto kill{fKillChargedPion->GetNewBoolValue(value)};
        Deliver<SteppingAction>([&](auto&& r) {
            r.KillChargedPion(kill);
        });
    }
}

} // namespace MusAirS::inline Messenger
