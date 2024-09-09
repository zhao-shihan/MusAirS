#include "MusAirS/Generator/PrimaryCosmicRayGenerator.h++"
#include "MusAirS/Messenger/PrimaryCosmicRayGeneratorMessenger.h++"

#include "G4ApplicationState.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcommand.hh"
#include "G4UIdirectory.hh"
#include "G4UIparameter.hh"
#include "G4ios.hh"

#include <stdexcept>
#include <string_view>
#include <unordered_map>

namespace MusAirS::inline Messenger {

PrimaryCosmicRayGeneratorMessenger::PrimaryCosmicRayGeneratorMessenger() :
    SingletonMessenger{},
    fDirectory{},
    fParticle{},
    fEnergySpectrumFormula{},
    fEnergySpectrumHistogram{},
    fMinEnergy{},
    fMaxEnergy{},
    fEnergySampling{} {

    fDirectory = std::make_unique<G4UIdirectory>("/MusAirS/PCG/");
    fDirectory->SetGuidance("MusAirS primary cosmic ray generator controller.");

    fParticle = std::make_unique<G4UIcmdWithAString>("/MusAirS/PCG/Particle", this);
    fParticle->SetGuidance("Set primary particle.");
    fParticle->SetParameterName("particle", false);
    fParticle->AvailableForStates(G4State_Idle);

    fEnergySpectrumFormula = std::make_unique<G4UIcmdWithAString>("/MusAirS/PCG/Energy/Spectrum/Formula", this);
    fEnergySpectrumFormula->SetGuidance("Set formula for kinetic energy spectrum (ROOT TFormula form).");
    fEnergySpectrumFormula->SetParameterName("formula", false);
    fEnergySpectrumFormula->AvailableForStates(G4State_Idle);

    fEnergySpectrumHistogram = std::make_unique<G4UIcommand>("/MusAirS/PCG/Energy/Spectrum/Histogram", this);
    fEnergySpectrumHistogram->SetGuidance("Set kinetic energy histogram from a TH1 in a ROOT file.");
    fEnergySpectrumHistogram->SetParameter(new G4UIparameter{"file", 's', false});
    fEnergySpectrumHistogram->SetParameter(new G4UIparameter{"h", 's', false});
    fEnergySpectrumHistogram->AvailableForStates(G4State_Idle);

    fMinEnergy = std::make_unique<G4UIcmdWithADoubleAndUnit>("/MusAirS/PCG/Energy/Min", this);
    fMinEnergy->SetGuidance("Set minimum kinetic energy.");
    fMinEnergy->SetParameterName("E_min", false);
    fMinEnergy->SetUnitCategory("Energy");
    fMinEnergy->SetRange("E_min >= 0");
    fMinEnergy->AvailableForStates(G4State_Idle);

    fMaxEnergy = std::make_unique<G4UIcmdWithADoubleAndUnit>("/MusAirS/PCG/Energy/Max", this);
    fMaxEnergy->SetGuidance("Set maximum kinetic energy.");
    fMaxEnergy->SetParameterName("E_max", false);
    fMaxEnergy->SetUnitCategory("Energy");
    fMaxEnergy->SetRange("E_max > 0");
    fMaxEnergy->AvailableForStates(G4State_Idle);

    fEnergySampling = std::make_unique<G4UIcmdWithAString>("/MusAirS/PCG/Energy/Sampling", this);
    fEnergySampling->SetGuidance("Set energy importance sampling.");
    fEnergySampling->SetParameterName("mode", false);
    fEnergySampling->SetCandidates("Normal WeightedUniform");
    fEnergySampling->AvailableForStates(G4State_Idle);
}

auto PrimaryCosmicRayGeneratorMessenger::SetNewValue(G4UIcommand* command, G4String value) -> void {
    if (command == fParticle.get()) {
        Deliver<PrimaryCosmicRayGenerator>([&](auto&& r) {
            try {
                r.Particle(value);
            } catch (const std::runtime_error& e) {
                G4cerr << e.what() << G4endl;
            }
        });
    } else if (command == fEnergySpectrumFormula.get()) {
        Deliver<PrimaryCosmicRayGenerator>([&](auto&& r) {
            r.EnergySpectrum(value);
        });
    } else if (command == fEnergySpectrumHistogram.get()) {
        Deliver<PrimaryCosmicRayGenerator>([&](auto&& r) {
            G4cout << value << G4endl;
            // r.EnergySpectrum(fEnergySpectrumHistogram->GetCurrentValue());
        });
    } else if (command == fMinEnergy.get()) {
        Deliver<PrimaryCosmicRayGenerator>([&](auto&& r) {
            r.MinEnergy(fMinEnergy->GetNewDoubleValue(value));
        });
    } else if (command == fMaxEnergy.get()) {
        Deliver<PrimaryCosmicRayGenerator>([&](auto&& r) {
            r.MaxEnergy(fMaxEnergy->GetNewDoubleValue(value));
        });
    } else if (command == fEnergySampling.get()) {
        Deliver<PrimaryCosmicRayGenerator>([&](auto&& r) {
            static const std::unordered_map<std::string_view, enum PrimaryCosmicRayGenerator::EnergySampling> energySamplingMap {
                {"Normal",          PrimaryCosmicRayGenerator::EnergySampling::Normal         },
                {"WeightedUniform", PrimaryCosmicRayGenerator::EnergySampling::WeightedUniform}
            };
            r.EnergySampling(energySamplingMap.at(value));
        });
    }
}

} // namespace MusAirS::inline Messenger
