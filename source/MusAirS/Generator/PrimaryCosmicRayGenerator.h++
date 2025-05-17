// -*- C++ -*-
//
// Copyright 2024-2025  Shihan Zhao
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

#pragma once

#include "MusAirS/Messenger/PrimaryCosmicRayGeneratorMessenger.h++"

#include "G4VPrimaryGenerator.hh"

#include <concepts>
#include <memory>
#include <string>
#include <utility>

class G4Event;
class G4ParticleDefinition;
class TF1;
class TGraph;
class TH1;

namespace MusAirS::inline Generator {

class PrimaryCosmicRayGenerator : public G4VPrimaryGenerator {
public:
    enum struct EnergySamplingMode {
        Normal,
        UniformBiased,
        MinVarBiased,
        CustomBiased
    };

public:
    PrimaryCosmicRayGenerator();

    auto Particle(G4ParticleDefinition* particle) -> void { fParticle = particle; }
    auto Particle(const std::string& particle) -> void;

    auto EnergySpectrum(const std::string& formula) -> void;
    auto EnergySpectrum(const TH1& histogram) -> void;
    auto EnergySpectrum(const std::same_as<TGraph> auto& graph) -> void;
    template<typename T>
    auto EnergySpectrum(const std::string& fileName, const std::string& name) -> void;

    auto NEnergySpectrumPoint(int n) -> void;
    auto MinEnergy(double val) -> void;
    auto MaxEnergy(double val) -> void;
    auto EnergySamplingMode(enum EnergySamplingMode mode) -> void { fEnergySamplingMode = mode; }

    auto CustomBiasedEnergySpectrum(const std::string& formula) -> void;
    auto CustomBiasedEnergySpectrum(const TH1& histogram) -> void;
    auto CustomBiasedEnergySpectrum(const std::same_as<TGraph> auto& graph) -> void;
    template<typename T>
    auto CustomBiasedEnergySpectrum(const std::string& fileName, const std::string& name) -> void;

    virtual auto GeneratePrimaryVertex(G4Event* event) -> void override;

private:
    auto SyncBiasedEnergySpectrum() -> void;

    auto SampleEnergy() const -> std::pair<double, double>;

private:
    G4ParticleDefinition* fParticle;

    std::unique_ptr<TF1> fEnergySpectrum;
    int fNEnergySpectrumPoint;
    double fIntrinsicMinEnergy;
    double fIntrinsicMaxEnergy;
    enum EnergySamplingMode fEnergySamplingMode;
    std::unique_ptr<TF1> fMinVarBiasedEnergySpectrum;
    std::unique_ptr<TF1> fCustomBiasedEnergySpectrum;

    PrimaryCosmicRayGeneratorMessenger::Register<PrimaryCosmicRayGenerator> fMessengerRegister;
};

} // namespace MusAirS::inline Generator
