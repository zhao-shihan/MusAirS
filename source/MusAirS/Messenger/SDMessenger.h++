#pragma once

#include "Mustard/Extension/Geant4X/Interface/SingletonMessenger.h++"

#include <memory>

class G4UIcmdWithABool;
class G4UIdirectory;

namespace MusAirS {

inline namespace SD {
class EarthSD;
} // namespace SD

inline namespace Messenger {

class SDMessenger final : public Mustard::Geant4X::SingletonMessenger<SDMessenger,
                                                                      EarthSD> {
    friend Mustard::Env::Memory::SingletonInstantiator;

private:
    SDMessenger();
    ~SDMessenger();

public:
    auto SetNewValue(G4UIcommand* command, G4String value) -> void override;

private:
    std::unique_ptr<G4UIdirectory> fDirectory;
    std::unique_ptr<G4UIcmdWithABool> fDetectNeutrino;
};

} // namespace Messenger

} // namespace MusAirS
