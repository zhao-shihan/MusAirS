#pragma once

#include "Mustard/Extension/Geant4X/Interface/SingletonMessenger.h++"

#include <memory>

class G4UIcmdWithADoubleAndUnit;
class G4UIdirectory;

namespace MusAirS {

inline namespace Action {
class DetectorConstruction;
} // namespace Action

inline namespace Messenger {

class NumericMessenger final : public Mustard::Geant4X::SingletonMessenger<NumericMessenger,
                                                                           DetectorConstruction> {
    friend Mustard::Env::Memory::SingletonInstantiator;

private:
    NumericMessenger();
    ~NumericMessenger();

public:
    auto SetNewValue(G4UIcommand* command, G4String value) -> void override;

private:
    std::unique_ptr<G4UIdirectory> fDirectory;
    std::unique_ptr<G4UIcmdWithADoubleAndUnit> fMinDriverStep;
    std::unique_ptr<G4UIcmdWithADoubleAndUnit> fDeltaChord;
};

} // namespace Messenger

} // namespace MusAirS
