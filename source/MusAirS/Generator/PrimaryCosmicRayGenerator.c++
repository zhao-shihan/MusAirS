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

#include "MusAirS/Detector/Description/Atmosphere.h++"
#include "MusAirS/Detector/Description/Earth.h++"
#include "MusAirS/Detector/Description/World.h++"
#include "MusAirS/Generator/PrimaryCosmicRayGenerator.h++"

#include "Mustard/Env/MonteCarloEnv.h++"
#include "Mustard/Geant4X/Management/Exception.h++"
#include "Mustard/IO/PrettyLog.h++"
#include "Mustard/Math/Random/Distribution/Uniform.h++"
#include "Mustard/Utility/LiteralUnit.h++"

#include "TAxis.h"
#include "TF1.h"
#include "TFile.h"
#include "TGraph.h"
#include "TH1.h"

#include "G4Event.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
#include "G4PrimaryParticle.hh"

#include "muc/math"
#include "muc/utility"

#include "fmt/format.h"

#include <algorithm>
#include <cmath>
#include <functional>
#include <limits>
#include <stdexcept>

namespace MusAirS::inline Generator {

using namespace Mustard::LiteralUnit::Energy;

namespace {

auto FunctionIntegral(TF1& spectrum, double xMin, double xMax, int nGrid, bool logGrid) -> double {
    double integral{};
    if (logGrid) {
        const auto logStep{std::log(xMax / xMin) / nGrid};
        auto xPrevious{xMin};
        auto fPrevious{spectrum.Eval(xPrevious)};
        for (int i{1}; i <= nGrid; ++i) {
            const auto x{xMin * std::exp(i * logStep)};
            const auto f{spectrum.Eval(x)};
            integral += (fPrevious + f) * (x - xPrevious) / 2;
            xPrevious = x;
            fPrevious = f;
        }
    } else {
        const auto dx{(xMax - xMin) / nGrid};
        for (int i{}; i < nGrid; ++i) {
            integral += spectrum.Eval(xMin + (i + 0.5) * dx);
        }
        integral *= dx;
    }
    return integral;
}

auto HistogramIntegral(const TH1& histogram, double xMin, double xMax) -> double {
    const auto axis{histogram.GetXaxis()};
    double integral{};
    for (int i{1}; i <= axis->GetNbins(); ++i) {
        const auto overlap{std::min(axis->GetBinUpEdge(i), xMax) - std::max(axis->GetBinLowEdge(i), xMin)};
        if (overlap > 0) {
            integral += histogram.GetBinContent(i) * overlap;
        }
    }
    return integral;
}

auto MakeFunctionIntegral(std::shared_ptr<TF1> function) -> std::function<double(double, double, int, bool)> {
    return [function](double xMin, double xMax, int nGrid, bool logGrid) {
        return FunctionIntegral(*function, xMin, xMax, nGrid, logGrid);
    };
}

auto MakeSpectrum(const std::string& name, const std::string& formula, double xMin, double xMax, int nPoint) -> impl::Spectrum {
    auto function{std::make_shared<TF1>(name.c_str(), formula.c_str(), xMin, xMax)};
    function->SetNpx(nPoint);
    return {function, MakeFunctionIntegral(function)};
}

auto MakeSpectrum(const std::string& name, const TH1& histogram, int nPoint) -> impl::Spectrum {
    std::shared_ptr<TH1> h{dynamic_cast<TH1*>(histogram.Clone())};
    h->SetDirectory(nullptr);

    const auto xMin{h->GetXaxis()->GetXmin()};
    const auto xMax{h->GetXaxis()->GetXmax()};
    auto function{std::make_shared<TF1>(
        name.c_str(),
        std::function{[h](const double* x, const double*) {
            return h->GetBinContent(h->FindFixBin(*x));
        }},
        xMin, xMax)};
    function->SetNpx(nPoint);
    return {function, [h](double xMin, double xMax, int, bool) {
                return HistogramIntegral(*h, xMin, xMax);
            }};
}

auto MakeSpectrum(const std::string& name, const TGraph& graph, int nPoint) -> impl::Spectrum {
    std::shared_ptr<TGraph> g{dynamic_cast<TGraph*>(graph.Clone())};
    if (g->GetN() < 2) {
        Mustard::Throw<std::runtime_error>(fmt::format("A spectrum graph needs at least two points, but '{}' has {}.", g->GetName(), g->GetN()));
    }

    const auto xMin{g->GetX()[0]};
    const auto xMax{g->GetX()[g->GetN() - 1]};
    auto function{std::make_shared<TF1>(
        name.c_str(),
        std::function{[g](const double* x, const double*) {
            return g->Eval(*x);
        }},
        xMin, xMax)};
    function->SetNpx(nPoint);
    return {function, MakeFunctionIntegral(function)};
}

template<typename T>
auto LoadSpectrum(const std::string& name, const std::string& fileName, const std::string& objectName, int nPoint) -> impl::Spectrum {
    const std::unique_ptr<TFile> file{TFile::Open(fileName.c_str())};
    if (file == nullptr) {
        Mustard::Throw<std::runtime_error>(fmt::format("Cannot open '{}'.", fileName));
    }

    const auto spectrum{file->Get<T>(objectName.c_str())};
    if (spectrum == nullptr) {
        Mustard::Throw<std::runtime_error>(fmt::format("Cannot find object '{}' in '{}'.", objectName, fileName));
    }

    return MakeSpectrum(name, *spectrum, nPoint);
}

auto SyncSpectrum(impl::Spectrum& spectrum, double xMin, double xMax, int nPoint) -> void {
    if (spectrum->GetXmin() != xMin or spectrum->GetXmax() != xMax) {
        spectrum->SetRange(xMin, xMax);
    }
    if (spectrum->GetNpx() != nPoint) {
        spectrum->SetNpx(nPoint);
    }
}

auto ShootUniform(double inf, double sup) -> double {
    return Mustard::Random::Uniform<double>{inf, sup}(Mustard::Env::RandomEngine());
}

auto CustomSpectrumWeight(double physicalDensity, double sampledDensity, double integral) -> double {
    return physicalDensity * integral / sampledDensity;
}

auto EnergySpectrumValue(const TF1& spectrum, double energy) -> double {
    const auto value{spectrum(energy)};
    if (not(std::isfinite(value) and value >= 0)) {
        Mustard::Throw<std::runtime_error>(fmt::format("Energy spectrum must be finite and non-negative, but f({}) = {}.", energy, value));
    }
    return value;
}

} // namespace

PrimaryCosmicRayGenerator::PrimaryCosmicRayGenerator() :
    G4VPrimaryGenerator{},
    fParticle{},
    fEnergySpectrum{},
    fNEnergySpectrumPoint{10000},
    fIntrinsicMinEnergy{},
    fIntrinsicMaxEnergy{std::numeric_limits<double>::max()},
    fEnergySpectrumIntegral{},
    fEnergySamplingMode{EnergySamplingMode::Normal},
    fCustomBiasedEnergySpectrum{},
    fDirectionSamplingMode{DirectionSamplingMode::Normal},
    fZenithBiasExponent{2},
    fNDirectionSpectrumPoint{10000},
    fCustomBiasedDirectionSpectrum{},
    fMessengerRegister{this} {
    Particle("proton");
    SetupEnergySpectrum(MakeSpectrum("EnergySpectrum", "x^-2.7", 3_GeV, 10_TeV, fNEnergySpectrumPoint));
    fIntrinsicMinEnergy = 0;
    fIntrinsicMaxEnergy = std::numeric_limits<double>::max();
}

auto PrimaryCosmicRayGenerator::Particle(const std::string& particle) -> void {
    const auto p{G4ParticleTable::GetParticleTable()->FindParticle(particle)};
    if (p) {
        Particle(p);
    } else {
        Mustard::Throw<std::runtime_error>(fmt::format("No particle named '{}'.", particle));
    }
}

auto PrimaryCosmicRayGenerator::EnergySpectrum(const std::string& formula) -> void {
    const auto eMin{fEnergySpectrum ? fEnergySpectrum->GetXmin() : fIntrinsicMinEnergy};
    const auto eMax{fEnergySpectrum ? fEnergySpectrum->GetXmax() : fIntrinsicMaxEnergy};
    SetupEnergySpectrum(MakeSpectrum("EnergySpectrum", formula, eMin, eMax, fNEnergySpectrumPoint));
    fIntrinsicMinEnergy = 0;
    fIntrinsicMaxEnergy = std::numeric_limits<double>::max();
}

auto PrimaryCosmicRayGenerator::EnergySpectrum(const TH1& histogram) -> void {
    SetupEnergySpectrum(MakeSpectrum("EnergySpectrum", histogram, fNEnergySpectrumPoint));
}

template<>
auto PrimaryCosmicRayGenerator::EnergySpectrum<TGraph>(const TGraph& graph) -> void {
    SetupEnergySpectrum(MakeSpectrum("EnergySpectrum", graph, fNEnergySpectrumPoint));
}

template<typename T>
auto PrimaryCosmicRayGenerator::EnergySpectrum(const std::string& fileName, const std::string& name) -> void {
    SetupEnergySpectrum(LoadSpectrum<T>("EnergySpectrum", fileName, name, fNEnergySpectrumPoint));
}
template auto PrimaryCosmicRayGenerator::EnergySpectrum<TH1>(const std::string& fileName, const std::string& name) -> void;
template auto PrimaryCosmicRayGenerator::EnergySpectrum<TGraph>(const std::string& fileName, const std::string& name) -> void;

auto PrimaryCosmicRayGenerator::NEnergySpectrumPoint(int n) -> void {
    if (not(n >= 10 and n <= 10'000'000)) {
        Mustard::Throw<std::runtime_error>(fmt::format("Energy spectrum NPoint must be between 10 and 10000000, but n = {}.", n));
    }
    if (fEnergySpectrum) {
        fEnergySpectrum->SetNpx(n);
    }
    fNEnergySpectrumPoint = n;
}

auto PrimaryCosmicRayGenerator::MinEnergy(double val) -> void {
    if (not(val > 0)) {
        Mustard::Throw<std::runtime_error>(fmt::format("Minimum kinetic energy must be positive, but input = {}.", val));
    }
    const auto eMin{muc::clamp<"[]">(val, fIntrinsicMinEnergy, fIntrinsicMaxEnergy)};
    if (eMin != val) [[unlikely]] {
        Mustard::Geant4X::MasterException<JustWarning>(fmt::format(
            "Minimum kinetic energy {} MeV is outside the spectrum range [{}, {}] MeV and is clamped to {} MeV.",
            val, fIntrinsicMinEnergy, fIntrinsicMaxEnergy, eMin));
    }
    ValidateEnergyRange(eMin, fEnergySpectrum->GetXmax());
    fEnergySpectrum->SetRange(eMin, fEnergySpectrum->GetXmax());
    fEnergySpectrumIntegral = {};
}

auto PrimaryCosmicRayGenerator::MaxEnergy(double val) -> void {
    const auto eMax{muc::clamp<"[]">(val, fIntrinsicMinEnergy, fIntrinsicMaxEnergy)};
    if (eMax != val) [[unlikely]] {
        Mustard::Geant4X::MasterException<JustWarning>(fmt::format(
            "Maximum kinetic energy {} MeV is outside the spectrum range [{}, {}] MeV and is clamped to {} MeV.",
            val, fIntrinsicMinEnergy, fIntrinsicMaxEnergy, eMax));
    }
    ValidateEnergyRange(fEnergySpectrum->GetXmin(), eMax);
    fEnergySpectrum->SetRange(fEnergySpectrum->GetXmin(), eMax);
    fEnergySpectrumIntegral = {};
}

auto PrimaryCosmicRayGenerator::EnergySamplingMode(enum EnergySamplingMode mode) -> void {
    ValidateEnergySpectrumForSampling();
    fEnergySamplingMode = mode;
}

auto PrimaryCosmicRayGenerator::DirectionSamplingMode(enum DirectionSamplingMode mode) -> void {
    fDirectionSamplingMode = mode;
}

auto PrimaryCosmicRayGenerator::ZenithBiasExponent(double n) -> void {
    if (not(std::isfinite(n) and n >= 0)) {
        Mustard::Throw<std::runtime_error>(fmt::format("Zenith bias exponent must be finite and non-negative, but n = {}.", n));
    }
    fZenithBiasExponent = n;
}

auto PrimaryCosmicRayGenerator::NDirectionSpectrumPoint(int n) -> void {
    if (not(n >= 10 and n <= 10'000'000)) {
        Mustard::Throw<std::runtime_error>(fmt::format("Direction spectrum NPoint must be between 10 and 10000000, but n = {}.", n));
    }
    fNDirectionSpectrumPoint = n;
}

auto PrimaryCosmicRayGenerator::CustomBiasedEnergySpectrum(const std::string& formula) -> void {
    fCustomBiasedEnergySpectrum = MakeSpectrum("CustomBiasedEnergySpectrum", formula, 0., 1., fEnergySpectrum->GetNpx());
    fCustomBiasedEnergyIntegral = {};
}

auto PrimaryCosmicRayGenerator::CustomBiasedEnergySpectrum(const TH1& histogram) -> void {
    fCustomBiasedEnergySpectrum = MakeSpectrum("CustomBiasedEnergySpectrum", histogram, fEnergySpectrum->GetNpx());
    fCustomBiasedEnergyIntegral = {};
}

template<>
auto PrimaryCosmicRayGenerator::CustomBiasedEnergySpectrum<TGraph>(const TGraph& graph) -> void {
    fCustomBiasedEnergySpectrum = MakeSpectrum("CustomBiasedEnergySpectrum", graph, fEnergySpectrum->GetNpx());
    fCustomBiasedEnergyIntegral = {};
}

template<typename T>
auto PrimaryCosmicRayGenerator::CustomBiasedEnergySpectrum(const std::string& fileName, const std::string& name) -> void {
    fCustomBiasedEnergySpectrum = LoadSpectrum<T>("CustomBiasedEnergySpectrum", fileName, name, fEnergySpectrum->GetNpx());
    fCustomBiasedEnergyIntegral = {};
}
template auto PrimaryCosmicRayGenerator::CustomBiasedEnergySpectrum<TH1>(const std::string& fileName, const std::string& name) -> void;
template auto PrimaryCosmicRayGenerator::CustomBiasedEnergySpectrum<TGraph>(const std::string& fileName, const std::string& name) -> void;

auto PrimaryCosmicRayGenerator::CustomBiasedDirectionSpectrum(const std::string& formula) -> void {
    fCustomBiasedDirectionSpectrum = MakeSpectrum("CustomBiasedDirectionSpectrum", formula, 0., 1., fNDirectionSpectrumPoint);
    fCustomBiasedDirectionIntegral = {};
}

auto PrimaryCosmicRayGenerator::CustomBiasedDirectionSpectrum(const TH1& histogram) -> void {
    fCustomBiasedDirectionSpectrum = MakeSpectrum("CustomBiasedDirectionSpectrum", histogram, fNDirectionSpectrumPoint);
    fCustomBiasedDirectionIntegral = {};
}

template<>
auto PrimaryCosmicRayGenerator::CustomBiasedDirectionSpectrum<TGraph>(const TGraph& graph) -> void {
    fCustomBiasedDirectionSpectrum = MakeSpectrum("CustomBiasedDirectionSpectrum", graph, fNDirectionSpectrumPoint);
    fCustomBiasedDirectionIntegral = {};
}

template<typename T>
auto PrimaryCosmicRayGenerator::CustomBiasedDirectionSpectrum(const std::string& fileName, const std::string& name) -> void {
    fCustomBiasedDirectionSpectrum = LoadSpectrum<T>("CustomBiasedDirectionSpectrum", fileName, name, fNDirectionSpectrumPoint);
    fCustomBiasedDirectionIntegral = {};
}
template auto PrimaryCosmicRayGenerator::CustomBiasedDirectionSpectrum<TH1>(const std::string& fileName, const std::string& name) -> void;
template auto PrimaryCosmicRayGenerator::CustomBiasedDirectionSpectrum<TGraph>(const std::string& fileName, const std::string& name) -> void;

auto PrimaryCosmicRayGenerator::GeneratePrimaryVertex(G4Event* event) -> void {
    try {
        GeneratePrimaryVertexImpl(event);
    } catch (const std::runtime_error& e) {
        Mustard::Geant4X::Exception<RunMustBeAborted>(e.what());
    }
}

auto PrimaryCosmicRayGenerator::GeneratePrimaryVertexImpl(G4Event* event) -> void {
    using namespace Mustard::LiteralUnit::MathConstantSuffix;

    const auto hMax{Detector::Description::Atmosphere::Instance().MaxAltitude()};
    const auto zGround{Detector::Description::Earth::Instance().GroundAltitude()};
    const auto height{hMax - zGround};
    const auto halfWidth{Detector::Description::World::Instance().Width() / 2};
    const auto minCosTheta{height / muc::hypot(height, halfWidth)};

    const auto [ek, energyWeight]{SampleEnergy()};
    const auto [pHat, directionWeight]{SampleDirection(minCosTheta)};
    const auto pathLength{height / -pHat.z()};
    particle_position = {-pathLength * pHat.x(), -pathLength * pHat.y(), hMax};
    const auto p{std::sqrt((2 * fParticle->GetPDGMass() + ek) * ek) * pHat};

    // clang-format off
    const auto vertex{new G4PrimaryVertex{particle_position, particle_time}}; // clang-format on
    vertex->SetWeight(energyWeight * directionWeight);
    vertex->SetPrimary(new G4PrimaryParticle{fParticle, p.x(), p.y(), p.z()});
    event->AddPrimaryVertex(vertex);
}

auto PrimaryCosmicRayGenerator::ValidateEnergySpectrumForSampling() const -> void {
    if (not fEnergySpectrum) {
        Mustard::Throw<std::runtime_error>("Energy spectrum must be set before selecting a sampling mode.");
    }
    ValidateEnergyRange(fEnergySpectrum->GetXmin(), fEnergySpectrum->GetXmax());
}

auto PrimaryCosmicRayGenerator::ValidateEnergyRange(double eMin, double eMax) const -> void {
    if (not(eMin > 0)) {
        Mustard::Throw<std::runtime_error>(fmt::format("Minimum kinetic energy must be positive, but E_min = {}.", eMin));
    }
    if (not(eMax > eMin)) {
        Mustard::Throw<std::runtime_error>(fmt::format("Maximum kinetic energy {} MeV must be greater than minimum kinetic energy {} MeV.", eMax, eMin));
    }
}

auto PrimaryCosmicRayGenerator::SetupEnergySpectrum(impl::Spectrum spectrum) -> void {
    ValidateEnergyRange(spectrum->GetXmin(), spectrum->GetXmax());
    fIntrinsicMinEnergy = spectrum->GetXmin();
    fIntrinsicMaxEnergy = spectrum->GetXmax();
    fEnergySpectrum = std::move(spectrum);
    fEnergySpectrumIntegral = {};
}

auto PrimaryCosmicRayGenerator::SpectrumIntegral(impl::Spectrum& spectrum, double xMin, double xMax, int nPoint, bool logGrid, SpectrumIntegralCache& cache) -> double {
    SyncSpectrum(spectrum, xMin, xMax, nPoint);
    const auto nGrid{std::max(nPoint, 1'000'000)};
    if (not cache.value or cache.xMin != xMin or cache.xMax != xMax or cache.nGrid != nGrid or cache.logGrid != logGrid) [[unlikely]] {
        const auto integral{spectrum.integral(xMin, xMax, nGrid, logGrid)};
        if (not(std::isfinite(integral) and integral > 0)) {
            Mustard::Throw<std::runtime_error>(fmt::format("Spectrum must have a finite positive integral on [{}, {}].", xMin, xMax));
        }
        cache.value = integral;
        cache.xMin = xMin;
        cache.xMax = xMax;
        cache.nGrid = nGrid;
        cache.logGrid = logGrid;
    }
    return *cache.value;
}

auto PrimaryCosmicRayGenerator::SampleEnergy() -> std::pair<double, double> {
    const auto eMin{fEnergySpectrum->GetXmin()};
    const auto eMax{fEnergySpectrum->GetXmax()};
    const auto option{"LOG"};
    auto& rootRandomEngine{Mustard::Env::MonteCarloEnv::Instance().ROOTRandomEngine()};
    switch (fEnergySamplingMode) {
    case EnergySamplingMode::Normal: {
        const auto integral{SpectrumIntegral(fEnergySpectrum, eMin, eMax, fEnergySpectrum->GetNpx(), true, fEnergySpectrumIntegral)};
        const auto ek{fEnergySpectrum->GetRandom(&rootRandomEngine, option)};
        if (not std::isfinite(ek)) {
            Mustard::Throw<std::runtime_error>("Energy spectrum returned a non-finite value.");
        }
        EnergySpectrumValue(*fEnergySpectrum, ek);
        return {ek, integral};
    }
    case EnergySamplingMode::UniformBiased: {
        const auto ek{ShootUniform(eMin, eMax)};
        const auto fk{EnergySpectrumValue(*fEnergySpectrum, ek)};
        return {ek, fk * (eMax - eMin)};
    }
    case EnergySamplingMode::LogUniformBiased: {
        const auto ek{eMin * std::pow(eMax / eMin, ShootUniform(0., 1.))};
        const auto fk{EnergySpectrumValue(*fEnergySpectrum, ek)};
        return {ek, fk * ek * std::log(eMax / eMin)};
    }
    case EnergySamplingMode::CustomBiased: {
        if (not fCustomBiasedEnergySpectrum) {
            Mustard::Throw<std::runtime_error>("CustomBiased energy sampling requires a custom energy spectrum.");
        }
        const auto integral{SpectrumIntegral(fCustomBiasedEnergySpectrum, eMin, eMax, fEnergySpectrum->GetNpx(), true, fCustomBiasedEnergyIntegral)};
        const auto ek{fCustomBiasedEnergySpectrum->GetRandom(&rootRandomEngine, option)};
        if (not std::isfinite(ek)) {
            Mustard::Throw<std::runtime_error>("CustomBiased energy spectrum returned a non-finite value.");
        }
        const auto g{(*fCustomBiasedEnergySpectrum)(ek)};
        if (not(std::isfinite(g) and g > 0)) {
            Mustard::Throw<std::runtime_error>(fmt::format("CustomBiased energy spectrum must be finite and positive, but g({}) = {}.", ek, g));
        }
        const auto fk{EnergySpectrumValue(*fEnergySpectrum, ek)};
        return {ek, CustomSpectrumWeight(fk, g, integral)};
    }
    }
    muc::unreachable();
}

auto PrimaryCosmicRayGenerator::SampleDirection(double minCosTheta) -> std::pair<G4ThreeVector, double> {
    using namespace Mustard::LiteralUnit::MathConstantSuffix;
    auto& rootRandomEngine{Mustard::Env::MonteCarloEnv::Instance().ROOTRandomEngine()};

    double cosTheta{};
    double weight{1};
    switch (fDirectionSamplingMode) {
    case DirectionSamplingMode::Normal: {
        const auto minCosThetaSquared{muc::pow(minCosTheta, 2)};
        const auto acceptance{1 - minCosThetaSquared};
        cosTheta = std::sqrt(ShootUniform(minCosThetaSquared, 1.));
        weight = acceptance;
    } break;
    case DirectionSamplingMode::ZenithPowerBiased: {
        const auto n{fZenithBiasExponent};
        const auto uMinPow{std::pow(minCosTheta, n + 1)};
        cosTheta = std::pow(uMinPow + ShootUniform(0., 1.) * (1 - uMinPow), 1. / (n + 1));
        weight = 2. / (n + 1) * (1 - uMinPow) * std::pow(cosTheta, 1 - n);
    } break;
    case DirectionSamplingMode::CustomBiased: {
        if (not fCustomBiasedDirectionSpectrum) {
            Mustard::Throw<std::runtime_error>("CustomBiased direction sampling requires a custom direction spectrum.");
        }
        const auto integral{SpectrumIntegral(fCustomBiasedDirectionSpectrum, minCosTheta, 1., fNDirectionSpectrumPoint, false, fCustomBiasedDirectionIntegral)};
        cosTheta = fCustomBiasedDirectionSpectrum->GetRandom(&rootRandomEngine);
        if (not std::isfinite(cosTheta)) {
            Mustard::Throw<std::runtime_error>("CustomBiased direction spectrum returned a non-finite value.");
        }
        cosTheta = std::min(std::max(cosTheta, minCosTheta), 1.);
        const auto biased{(*fCustomBiasedDirectionSpectrum)(cosTheta)};
        if (not(std::isfinite(biased) and biased > 0)) {
            Mustard::Throw<std::runtime_error>(fmt::format("CustomBiased direction spectrum must be finite and positive, but g({}) = {}.", cosTheta, biased));
        }
        weight = CustomSpectrumWeight(2 * cosTheta, biased, integral);
    } break;
    default: {
        muc::unreachable();
    }
    }

    const auto sinTheta{std::sqrt((1 - cosTheta) * (1 + cosTheta))};
    const auto phi{ShootUniform(0., 2_pi)};
    const auto [sinPhi, cosPhi]{muc::sincos(phi)};
    const G4ThreeVector pHat{sinTheta * cosPhi, sinTheta * sinPhi, -cosTheta};
    return {pHat, weight};
}

} // namespace MusAirS::inline Generator
