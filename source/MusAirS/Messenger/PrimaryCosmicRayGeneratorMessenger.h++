#pragma once

#include "Mustard/Extension/Geant4X/Interface/SingletonMessenger.h++"

#include "G4String.hh"

#include <memory>
#include <string_view>

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
    static auto ParseFileNameAndObjectName(std::string_view value) -> std::pair<std::string_view, std::string_view>;

private:
    std::unique_ptr<G4UIdirectory> fDirectory;
    std::unique_ptr<G4UIcmdWithAString> fParticle;
    std::unique_ptr<G4UIcmdWithAString> fEnergySpectrumFormula;
    std::unique_ptr<G4UIcommand> fEnergySpectrumHistogram;
    std::unique_ptr<G4UIcommand> fEnergySpectrumGraph;
    std::unique_ptr<G4UIcmdWithAnInteger> fNEnergySpectrumPoint;
    std::unique_ptr<G4UIcmdWithADoubleAndUnit> fMinEnergy;
    std::unique_ptr<G4UIcmdWithADoubleAndUnit> fMaxEnergy;
    std::unique_ptr<G4UIcmdWithAString> fEnergySamplingMode;
    std::unique_ptr<G4UIcmdWithAString> fCustomBiasedEnergySpectrumFormula;
    std::unique_ptr<G4UIcommand> fCustomBiasedEnergySpectrumHistogram;
    std::unique_ptr<G4UIcommand> fCustomBiasedEnergySpectrumGraph;
};

} // namespace Messenger

} // namespace MusAirS
