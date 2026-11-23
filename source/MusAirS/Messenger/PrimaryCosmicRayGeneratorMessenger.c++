// -*- C++ -*-
//
// Copyright 2024-2026  Shihan Zhao
//
// This file is part of MusAirS, an air shower simulation tool.
//
// MusAirS is free software: you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the Free Software
// Foundation, either version 3 of the License, or (at your option) any later
// version.
//
// MusAirS is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with
// MusAirS. If not, see <https://www.gnu.org/licenses/>.

#include "MusAirS/Generator/PrimaryCosmicRayGenerator.h++"
#include "MusAirS/Messenger/PrimaryCosmicRayGeneratorMessenger.h++"

#include "Mustard/Geant4X/Management/Exception.h++"

#include "G4ApplicationState.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcommand.hh"
#include "G4UIdirectory.hh"
#include "G4UIparameter.hh"

#include "fmt/format.h"

#include <ranges>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace MusAirS::inline Messenger {

PrimaryCosmicRayGeneratorMessenger::PrimaryCosmicRayGeneratorMessenger() :
    SingletonMessenger{},
    fDirectory{},
    fParticle{},
    fEnergySpectrumFormula{},
    fEnergySpectrumHistogram{},
    fEnergySpectrumGraph{},
    fNEnergySpectrumPoint{},
    fMinEnergy{},
    fMaxEnergy{},
    fEnergySamplingMode{},
    fCustomBiasedEnergySpectrumFormula{},
    fCustomBiasedEnergySpectrumHistogram{},
    fCustomBiasedEnergySpectrumGraph{},
    fDirectionSamplingMode{},
    fZenithBiasExponent{},
    fCustomBiasedDirectionSpectrumFormula{},
    fCustomBiasedDirectionSpectrumHistogram{},
    fCustomBiasedDirectionSpectrumGraph{},
    fNDirectionSpectrumPoint{} {

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

    fEnergySpectrumGraph = std::make_unique<G4UIcommand>("/MusAirS/PCR/Energy/Spectrum/Graph", this);
    fEnergySpectrumGraph->SetGuidance("Set kinetic energy graph from a TGraph in a ROOT file.");
    fEnergySpectrumGraph->SetParameter(new G4UIparameter{"file", 's', false});
    fEnergySpectrumGraph->SetParameter(new G4UIparameter{"g", 's', false});
    fEnergySpectrumGraph->AvailableForStates(G4State_Idle);

    fNEnergySpectrumPoint = std::make_unique<G4UIcmdWithAnInteger>("/MusAirS/PCR/Energy/Spectrum/NPoint", this);
    fNEnergySpectrumPoint->SetGuidance("Set number of points in integration of spectrum.");
    fNEnergySpectrumPoint->SetParameterName("n", false);
    fNEnergySpectrumPoint->SetRange("n >= 10 && n <= 10000000");
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

    fEnergySamplingMode = std::make_unique<G4UIcmdWithAString>("/MusAirS/PCR/Energy/Sampling/Mode", this);
    fEnergySamplingMode->SetGuidance("Set energy importance sampling.");
    fEnergySamplingMode->SetParameterName("mode", false);
    fEnergySamplingMode->SetCandidates("Normal UniformBiased LogUniformBiased CustomBiased");
    fEnergySamplingMode->AvailableForStates(G4State_Idle);

    fCustomBiasedEnergySpectrumFormula = std::make_unique<G4UIcmdWithAString>("/MusAirS/PCR/Energy/Sampling/Custom/Formula", this);
    fCustomBiasedEnergySpectrumFormula->SetGuidance("Set formula for custom-biased kinetic energy spectrum (ROOT TFormula form).");
    fCustomBiasedEnergySpectrumFormula->SetParameterName("formula", false);
    fCustomBiasedEnergySpectrumFormula->AvailableForStates(G4State_Idle);

    fCustomBiasedEnergySpectrumHistogram = std::make_unique<G4UIcommand>("/MusAirS/PCR/Energy/Sampling/Custom/Histogram", this);
    fCustomBiasedEnergySpectrumHistogram->SetGuidance("Set custom-biased kinetic energy histogram from a TH1 in a ROOT file.");
    fCustomBiasedEnergySpectrumHistogram->SetParameter(new G4UIparameter{"file", 's', false});
    fCustomBiasedEnergySpectrumHistogram->SetParameter(new G4UIparameter{"h", 's', false});
    fCustomBiasedEnergySpectrumHistogram->AvailableForStates(G4State_Idle);

    fCustomBiasedEnergySpectrumGraph = std::make_unique<G4UIcommand>("/MusAirS/PCR/Energy/Sampling/Custom/Graph", this);
    fCustomBiasedEnergySpectrumGraph->SetGuidance("Set custom-biased kinetic energy graph from a TGraph in a ROOT file.");
    fCustomBiasedEnergySpectrumGraph->SetParameter(new G4UIparameter{"file", 's', false});
    fCustomBiasedEnergySpectrumGraph->SetParameter(new G4UIparameter{"g", 's', false});
    fCustomBiasedEnergySpectrumGraph->AvailableForStates(G4State_Idle);

    fDirectionSamplingMode = std::make_unique<G4UIcmdWithAString>("/MusAirS/PCR/Direction/Sampling/Mode", this);
    fDirectionSamplingMode->SetGuidance("Set primary direction importance sampling.");
    fDirectionSamplingMode->SetParameterName("mode", false);
    fDirectionSamplingMode->SetCandidates("Normal ZenithPowerBiased CustomBiased");
    fDirectionSamplingMode->AvailableForStates(G4State_Idle);

    fZenithBiasExponent = std::make_unique<G4UIcmdWithADouble>("/MusAirS/PCR/Direction/Sampling/Power", this);
    fZenithBiasExponent->SetGuidance("Set the power n of the cos^n(theta) direction bias.");
    fZenithBiasExponent->SetParameterName("n", false);
    fZenithBiasExponent->SetRange("n >= 0");
    fZenithBiasExponent->AvailableForStates(G4State_Idle);

    fCustomBiasedDirectionSpectrumFormula = std::make_unique<G4UIcmdWithAString>("/MusAirS/PCR/Direction/Sampling/Custom/Formula", this);
    fCustomBiasedDirectionSpectrumFormula->SetGuidance("Set formula for custom-biased cos(theta) distribution (ROOT TFormula form, x = cos(theta)).");
    fCustomBiasedDirectionSpectrumFormula->SetParameterName("formula", false);
    fCustomBiasedDirectionSpectrumFormula->AvailableForStates(G4State_Idle);

    fCustomBiasedDirectionSpectrumHistogram = std::make_unique<G4UIcommand>("/MusAirS/PCR/Direction/Sampling/Custom/Histogram", this);
    fCustomBiasedDirectionSpectrumHistogram->SetGuidance("Set custom-biased cos(theta) histogram from a TH1 in a ROOT file.");
    fCustomBiasedDirectionSpectrumHistogram->SetParameter(new G4UIparameter{"file", 's', false});
    fCustomBiasedDirectionSpectrumHistogram->SetParameter(new G4UIparameter{"h", 's', false});
    fCustomBiasedDirectionSpectrumHistogram->AvailableForStates(G4State_Idle);

    fCustomBiasedDirectionSpectrumGraph = std::make_unique<G4UIcommand>("/MusAirS/PCR/Direction/Sampling/Custom/Graph", this);
    fCustomBiasedDirectionSpectrumGraph->SetGuidance("Set custom-biased cos(theta) graph from a TGraph in a ROOT file.");
    fCustomBiasedDirectionSpectrumGraph->SetParameter(new G4UIparameter{"file", 's', false});
    fCustomBiasedDirectionSpectrumGraph->SetParameter(new G4UIparameter{"g", 's', false});
    fCustomBiasedDirectionSpectrumGraph->AvailableForStates(G4State_Idle);

    fNDirectionSpectrumPoint = std::make_unique<G4UIcmdWithAnInteger>("/MusAirS/PCR/Direction/Sampling/Custom/NPoint", this);
    fNDirectionSpectrumPoint->SetGuidance("Set number of points in the custom direction spectrum.");
    fNDirectionSpectrumPoint->SetParameterName("n", false);
    fNDirectionSpectrumPoint->SetRange("n >= 10 && n <= 10000000");
    fNDirectionSpectrumPoint->AvailableForStates(G4State_Idle);
}

PrimaryCosmicRayGeneratorMessenger::~PrimaryCosmicRayGeneratorMessenger() = default;

auto PrimaryCosmicRayGeneratorMessenger::SetNewValue(G4UIcommand* command, G4String value) -> void {
    const auto deliver{[&](auto&& action) {
        Deliver<PrimaryCosmicRayGenerator>([&](auto&& r) {
            try {
                action(r);
            } catch (const std::runtime_error& e) {
                Mustard::Geant4X::Exception<JustWarning>(e.what());
            }
        });
    }};

    if (command == fParticle.get()) {
        deliver([&](auto&& r) {
            r.Particle(value);
        });
    } else if (command == fEnergySpectrumFormula.get()) {
        deliver([&](auto&& r) {
            r.EnergySpectrum(value);
        });
    } else if (command == fEnergySpectrumHistogram.get()) {
        const auto [file, h]{ParseFileNameAndObjectName(value)};
        if (file.empty()) [[unlikely]] {
            Mustard::Geant4X::Exception<JustWarning>(fmt::format(
                "Command '{}' expects two parameters: <file> <object>.", command->GetCommandPath()));
            return;
        }
        deliver([&](auto&& r) {
            r.template EnergySpectrum<TH1>(std::string{file}, std::string{h});
        });
    } else if (command == fEnergySpectrumGraph.get()) {
        const auto [file, g]{ParseFileNameAndObjectName(value)};
        if (file.empty()) [[unlikely]] {
            Mustard::Geant4X::Exception<JustWarning>(fmt::format(
                "Command '{}' expects two parameters: <file> <object>.", command->GetCommandPath()));
            return;
        }
        deliver([&](auto&& r) {
            r.template EnergySpectrum<TGraph>(std::string{file}, std::string{g});
        });
    } else if (command == fNEnergySpectrumPoint.get()) {
        deliver([&](auto&& r) {
            r.NEnergySpectrumPoint(fNEnergySpectrumPoint->GetNewIntValue(value));
        });
    } else if (command == fMinEnergy.get()) {
        deliver([&](auto&& r) {
            r.MinEnergy(fMinEnergy->GetNewDoubleValue(value));
        });
    } else if (command == fMaxEnergy.get()) {
        deliver([&](auto&& r) {
            r.MaxEnergy(fMaxEnergy->GetNewDoubleValue(value));
        });
    } else if (command == fEnergySamplingMode.get()) {
        static const std::unordered_map<std::string_view, enum PrimaryCosmicRayGenerator::EnergySamplingMode> energySamplingMap{
            {"Normal",           PrimaryCosmicRayGenerator::EnergySamplingMode::Normal          },
            {"UniformBiased",    PrimaryCosmicRayGenerator::EnergySamplingMode::UniformBiased   },
            {"LogUniformBiased", PrimaryCosmicRayGenerator::EnergySamplingMode::LogUniformBiased},
            {"CustomBiased",     PrimaryCosmicRayGenerator::EnergySamplingMode::CustomBiased    }
        };
        const auto mode{energySamplingMap.find(value)};
        if (mode == energySamplingMap.end()) [[unlikely]] {
            Mustard::Geant4X::Exception<JustWarning>(fmt::format("Unknown energy sampling mode '{}'.", value));
            return;
        }
        deliver([&](auto&& r) {
            r.EnergySamplingMode(mode->second);
        });
    } else if (command == fCustomBiasedEnergySpectrumFormula.get()) {
        deliver([&](auto&& r) {
            r.CustomBiasedEnergySpectrum(value);
        });
    } else if (command == fCustomBiasedEnergySpectrumHistogram.get()) {
        const auto [file, h]{ParseFileNameAndObjectName(value)};
        if (file.empty()) [[unlikely]] {
            Mustard::Geant4X::Exception<JustWarning>(fmt::format(
                "Command '{}' expects two parameters: <file> <object>.", command->GetCommandPath()));
            return;
        }
        deliver([&](auto&& r) {
            r.template CustomBiasedEnergySpectrum<TH1>(std::string{file}, std::string{h});
        });
    } else if (command == fCustomBiasedEnergySpectrumGraph.get()) {
        const auto [file, g]{ParseFileNameAndObjectName(value)};
        if (file.empty()) [[unlikely]] {
            Mustard::Geant4X::Exception<JustWarning>(fmt::format(
                "Command '{}' expects two parameters: <file> <object>.", command->GetCommandPath()));
            return;
        }
        deliver([&](auto&& r) {
            r.template CustomBiasedEnergySpectrum<TGraph>(std::string{file}, std::string{g});
        });
    } else if (command == fDirectionSamplingMode.get()) {
        static const std::unordered_map<std::string_view, enum PrimaryCosmicRayGenerator::DirectionSamplingMode> directionSamplingMap{
            {"Normal",            PrimaryCosmicRayGenerator::DirectionSamplingMode::Normal           },
            {"ZenithPowerBiased", PrimaryCosmicRayGenerator::DirectionSamplingMode::ZenithPowerBiased},
            {"CustomBiased",      PrimaryCosmicRayGenerator::DirectionSamplingMode::CustomBiased     }
        };
        const auto mode{directionSamplingMap.find(value)};
        if (mode == directionSamplingMap.end()) [[unlikely]] {
            Mustard::Geant4X::Exception<JustWarning>(fmt::format("Unknown direction sampling mode '{}'.", value));
            return;
        }
        deliver([&](auto&& r) {
            r.DirectionSamplingMode(mode->second);
        });
    } else if (command == fZenithBiasExponent.get()) {
        deliver([&](auto&& r) {
            r.ZenithBiasExponent(fZenithBiasExponent->GetNewDoubleValue(value));
        });
    } else if (command == fCustomBiasedDirectionSpectrumFormula.get()) {
        deliver([&](auto&& r) {
            r.CustomBiasedDirectionSpectrum(value);
        });
    } else if (command == fCustomBiasedDirectionSpectrumHistogram.get()) {
        const auto [file, h]{ParseFileNameAndObjectName(value)};
        if (file.empty()) [[unlikely]] {
            Mustard::Geant4X::Exception<JustWarning>(fmt::format(
                "Command '{}' expects two parameters: <file> <object>.", command->GetCommandPath()));
            return;
        }
        deliver([&](auto&& r) {
            r.template CustomBiasedDirectionSpectrum<TH1>(std::string{file}, std::string{h});
        });
    } else if (command == fCustomBiasedDirectionSpectrumGraph.get()) {
        const auto [file, g]{ParseFileNameAndObjectName(value)};
        if (file.empty()) [[unlikely]] {
            Mustard::Geant4X::Exception<JustWarning>(fmt::format(
                "Command '{}' expects two parameters: <file> <object>.", command->GetCommandPath()));
            return;
        }
        deliver([&](auto&& r) {
            r.template CustomBiasedDirectionSpectrum<TGraph>(std::string{file}, std::string{g});
        });
    } else if (command == fNDirectionSpectrumPoint.get()) {
        deliver([&](auto&& r) {
            r.NDirectionSpectrumPoint(fNDirectionSpectrumPoint->GetNewIntValue(value));
        });
    }
}

auto PrimaryCosmicRayGeneratorMessenger::ParseFileNameAndObjectName(std::string_view value) -> std::pair<std::string_view, std::string_view> {
    std::vector<std::string_view> parameter;
    parameter.reserve(2);
    for (auto&& token : value | std::views::split(' ')) {
        if (token.empty()) {
            continue;
        }
        parameter.emplace_back(token.begin(), token.end());
        if (parameter.size() == 2) {
            return {parameter.front(), parameter.back()};
        }
    }
    return {};
}

} // namespace MusAirS::inline Messenger
