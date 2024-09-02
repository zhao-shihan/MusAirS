#pragma once

#include "Mustard/Extension/Geant4X/Interface/SingletonMessenger.h++"

#include <memory>

class G4UIcmdWithABool;
class G4UIdirectory;

namespace MusAirS {

inline namespace Action {
class SteppingAction;
} // namespace Action

inline namespace Messenger {

class ActionMessenger final : public Mustard::Geant4X::SingletonMessenger<ActionMessenger,
                                                                          SteppingAction> {
    friend Mustard::Env::Memory::SingletonInstantiator;

private:
    ActionMessenger();
    ~ActionMessenger();

public:
    auto SetNewValue(G4UIcommand* command, G4String value) -> void override;

private:
    std::unique_ptr<G4UIdirectory> fDirectory;
    std::unique_ptr<G4UIcmdWithABool> fKillEMShower;
    std::unique_ptr<G4UIcmdWithABool> fKillNeutrino;
    std::unique_ptr<G4UIcmdWithABool> fKillChargedPion;
};

} // namespace Messenger

} // namespace MusAirS
