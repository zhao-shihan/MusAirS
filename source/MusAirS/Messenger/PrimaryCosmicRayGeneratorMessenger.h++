#pragma once

#include "Mustard/Extension/Geant4X/Interface/SingletonMessenger.h++"

#include "G4String.hh"

#include <memory>

class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithAnInteger;
class G4UIcmdWithAString;
class G4UIcommand;
class G4UIdirectory;

namespace MusAirS {

inline namespace Generator {
class PrimaryCosmicRayGenerator;
} // namespace Generator

inline namespace Messenger {

class PrimaryCosmicRayGeneratorMessenger final : public Mustard::Geant4X::SingletonMessenger<PrimaryCosmicRayGeneratorMessenger,
                                                                                             PrimaryCosmicRayGenerator> {
    friend Mustard::Env::Memory::SingletonInstantiator;

private:
    PrimaryCosmicRayGeneratorMessenger();
    ~PrimaryCosmicRayGeneratorMessenger() = default;

public:
    auto SetNewValue(G4UIcommand* command, G4String value) -> void override;

private:
    std::unique_ptr<G4UIdirectory> fDirectory;
    std::unique_ptr<G4UIcmdWithAString> fParticle;
    std::unique_ptr<G4UIcmdWithAString> fEnergySpectrumFormula;
    std::unique_ptr<G4UIcommand> fEnergySpectrumHistogram;
    std::unique_ptr<G4UIcmdWithAnInteger> fNEnergySpectrumPoint;
    std::unique_ptr<G4UIcmdWithADoubleAndUnit> fMinEnergy;
    std::unique_ptr<G4UIcmdWithADoubleAndUnit> fMaxEnergy;
    std::unique_ptr<G4UIcmdWithAString> fEnergySampling;
    std::unique_ptr<G4UIcmdWithAString> fCustomBiasedEnergySpectrumFormula;
    std::unique_ptr<G4UIcommand> fCustomBiasedEnergySpectrumHistogram;
};

} // namespace Messenger

} // namespace MusAirS
