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

#pragma once

#include "MusAirS/Messenger/PrimaryCosmicRayGeneratorMessenger.h++"

#include "G4ThreeVector.hh"
#include "G4VPrimaryGenerator.hh"

#include <concepts>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <utility>

class G4Event;
class G4ParticleDefinition;
class TF1;
class TGraph;
class TH1;

namespace MusAirS::inline Generator {

namespace impl {

struct Spectrum {
    std::shared_ptr<TF1> function;
    std::function<double(double, double, int, bool)> integral;

    auto operator->() -> TF1* { return function.get(); }
    auto operator->() const -> const TF1* { return function.get(); }
    auto operator*() -> TF1& { return *function; }
    auto operator*() const -> const TF1& { return *function; }
    explicit operator bool() const { return function != nullptr; }
};

} // namespace impl

class PrimaryCosmicRayGenerator : public G4VPrimaryGenerator {
public:
    enum struct EnergySamplingMode {
        Normal,
        UniformBiased,
        LogUniformBiased,
        CustomBiased
    };
    enum struct DirectionSamplingMode {
        Normal,
        ZenithPowerBiased,
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
    auto EnergySamplingMode(enum EnergySamplingMode mode) -> void;
    auto DirectionSamplingMode(enum DirectionSamplingMode mode) -> void;
    auto ZenithBiasExponent(double n) -> void;
    auto NDirectionSpectrumPoint(int n) -> void;

    auto CustomBiasedEnergySpectrum(const std::string& formula) -> void;
    auto CustomBiasedEnergySpectrum(const TH1& histogram) -> void;
    auto CustomBiasedEnergySpectrum(const std::same_as<TGraph> auto& graph) -> void;
    template<typename T>
    auto CustomBiasedEnergySpectrum(const std::string& fileName, const std::string& name) -> void;

    auto CustomBiasedDirectionSpectrum(const std::string& formula) -> void;
    auto CustomBiasedDirectionSpectrum(const TH1& histogram) -> void;
    auto CustomBiasedDirectionSpectrum(const std::same_as<TGraph> auto& graph) -> void;
    template<typename T>
    auto CustomBiasedDirectionSpectrum(const std::string& fileName, const std::string& name) -> void;

    virtual auto GeneratePrimaryVertex(G4Event* event) -> void override;

private:
    struct SpectrumIntegralCache {
        std::optional<double> value;
        double xMin{};
        double xMax{};
        int nGrid{};
        bool logGrid{};
    };

private:
    auto ValidateEnergySpectrumForSampling() const -> void;
    auto ValidateEnergyRange(double eMin, double eMax) const -> void;
    auto SetupEnergySpectrum(impl::Spectrum spectrum) -> void;
    auto SpectrumIntegral(impl::Spectrum& spectrum, double xMin, double xMax, int nPoint, bool logGrid, SpectrumIntegralCache& cache) -> double;

    auto SampleEnergy() -> std::pair<double, double>;
    auto SampleDirection(double minCosTheta) -> std::pair<G4ThreeVector, double>;

private:
    G4ParticleDefinition* fParticle;

    impl::Spectrum fEnergySpectrum;
    int fNEnergySpectrumPoint;
    double fIntrinsicMinEnergy;
    double fIntrinsicMaxEnergy;
    SpectrumIntegralCache fEnergySpectrumIntegral;
    enum EnergySamplingMode fEnergySamplingMode;
    impl::Spectrum fCustomBiasedEnergySpectrum;
    SpectrumIntegralCache fCustomBiasedEnergyIntegral;

    enum DirectionSamplingMode fDirectionSamplingMode;
    double fZenithBiasExponent;
    int fNDirectionSpectrumPoint;
    impl::Spectrum fCustomBiasedDirectionSpectrum;
    SpectrumIntegralCache fCustomBiasedDirectionIntegral;

    PrimaryCosmicRayGeneratorMessenger::Register<PrimaryCosmicRayGenerator> fMessengerRegister;
};

} // namespace MusAirS::inline Generator
