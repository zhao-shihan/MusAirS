#include "MusAirS/Generator/PrimaryCosmicRayGenerator.h++"
#include "MusAirS/Messenger/PrimaryCosmicRayGeneratorMessenger.h++"

#include "G4ApplicationState.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcommand.hh"
#include "G4UIdirectory.hh"
#include "G4UIparameter.hh"
#include "G4ios.hh"

#include <ranges>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace MusAirS::inline Messenger {

PrimaryCosmicRayGeneratorMessenger::PrimaryCosmicRayGeneratorMessenger() :
    SingletonMessenger{},
    fDirectory{},
    fParticle{},
    fEnergySpectrumFormula{},
    fEnergySpectrumHistogram{},
    fNEnergySpectrumPoint{},
    fMinEnergy{},
    fMaxEnergy{},
    fEnergySampling{} {

    fDirectory = std::make_unique<G4UIdirectory>("/MusAirS/PCR/");
    fDirectory->SetGuidance("MusAirS primary cosmic ray generator controller.");

    fParticle = std::make_unique<G4UIcmdWithAString>("/MusAirS/PCR/Particle", this);
    fParticle->SetGuidance("Set primary particle.");
    fParticle->SetParameterName("particle", false);
    fParticle->AvailableForStates(G4State_Idle);

    fEnergySpectrumFormula = std::make_unique<G4UIcmdWithAString>("/MusAirS/PCR/Energy/Spectrum/Formula", this);
    fEnergySpectrumFormula->SetGuidance("Set formula for kinetic energy spectrum (ROOT TFormula form).");
    fEnergySpectrumFormula->SetParameterName("formula", false);
    fEnergySpectrumFormula->AvailableForStates(G4State_Idle);

    fEnergySpectrumHistogram = std::make_unique<G4UIcommand>("/MusAirS/PCR/Energy/Spectrum/Histogram", this);
    fEnergySpectrumHistogram->SetGuidance("Set kinetic energy histogram from a TH1 in a ROOT file.");
    fEnergySpectrumHistogram->SetParameter(new G4UIparameter{"file", 's', false});
    fEnergySpectrumHistogram->SetParameter(new G4UIparameter{"h", 's', false});
    fEnergySpectrumHistogram->AvailableForStates(G4State_Idle);

    fNEnergySpectrumPoint = std::make_unique<G4UIcmdWithAnInteger>("/MusAirS/PCR/Energy/Spectrum/NPoint", this);
    fNEnergySpectrumPoint->SetGuidance("Set number of points in integration of spectrum.");
    fNEnergySpectrumPoint->SetParameterName("n", false);
    fNEnergySpectrumPoint->SetRange("n >= 10");
    fNEnergySpectrumPoint->AvailableForStates(G4State_Idle);

    fMinEnergy = std::make_unique<G4UIcmdWithADoubleAndUnit>("/MusAirS/PCR/Energy/Min", this);
    fMinEnergy->SetGuidance("Set minimum kinetic energy.");
    fMinEnergy->SetParameterName("E_min", false);
    fMinEnergy->SetUnitCategory("Energy");
    fMinEnergy->SetRange("E_min >= 0");
    fMinEnergy->AvailableForStates(G4State_Idle);

    fMaxEnergy = std::make_unique<G4UIcmdWithADoubleAndUnit>("/MusAirS/PCR/Energy/Max", this);
    fMaxEnergy->SetGuidance("Set maximum kinetic energy.");
    fMaxEnergy->SetParameterName("E_max", false);
    fMaxEnergy->SetUnitCategory("Energy");
    fMaxEnergy->SetRange("E_max > 0");
    fMaxEnergy->AvailableForStates(G4State_Idle);

    fEnergySampling = std::make_unique<G4UIcmdWithAString>("/MusAirS/PCR/Energy/Sampling", this);
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
        std::vector<std::string> parameter;
        parameter.reserve(2);
        for (auto&& token : value | std::views::split(' ')) {
            if (token.empty()) { continue; }
            parameter.emplace_back(token.begin(), token.end());
            if (parameter.size() == 2) { break; }
        }
        Deliver<PrimaryCosmicRayGenerator>([&](auto&& r) {
            try {
                r.EnergySpectrum(parameter.front(), parameter.back());
            } catch (const std::runtime_error& e) {
                G4cerr << e.what() << G4endl;
            }
        });
    } else if (command == fNEnergySpectrumPoint.get()) {
        Deliver<PrimaryCosmicRayGenerator>([&](auto&& r) {
            r.NEnergySpectrumPoint(fNEnergySpectrumPoint->GetNewIntValue(value));
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
        static const std::unordered_map<std::string_view, enum PrimaryCosmicRayGenerator::EnergySampling> energySamplingMap {
            {"Normal",          PrimaryCosmicRayGenerator::EnergySampling::Normal         },
            {"WeightedUniform", PrimaryCosmicRayGenerator::EnergySampling::WeightedUniform}
        };
        Deliver<PrimaryCosmicRayGenerator>([&](auto&& r) {
            r.EnergySampling(energySamplingMap.at(value));
        });
    }
}

} // namespace MusAirS::inline Messenger
