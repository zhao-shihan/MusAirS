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

#include "MusAirS/Detector/Description/Atmosphere.h++"
#include "MusAirS/Detector/Description/Earth.h++"
#include "MusAirS/Detector/Description/World.h++"
#include "MusAirS/Generator/PrimaryCosmicRayGenerator.h++"

#include "Mustard/IO/PrettyLog.h++"
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
#include "Randomize.hh"

#include "muc/math"
#include "muc/utility"

#include "fmt/core.h"

#include <algorithm>
#include <cmath>
#include <functional>
#include <limits>
#include <stdexcept>

namespace MusAirS::inline Generator {

using namespace Mustard::LiteralUnit::Energy;

namespace {

auto MakeCustomSpectrum(const std::string& name, const std::string& formula, int nPoint) -> std::unique_ptr<TF1> {
    auto spectrum{std::make_unique<TF1>(name.c_str(), formula.c_str())};
    spectrum->SetNpx(nPoint);
    return spectrum;
}

auto MakeCustomSpectrum(const std::string& name, const TH1& histogram, int nPoint) -> std::unique_ptr<TF1> {
    std::shared_ptr<TH1> h{dynamic_cast<TH1*>(histogram.Clone())};
    h->SetDirectory(nullptr);

    auto spectrum{std::make_unique<TF1>(
        name.c_str(),
        std::function{[spectrum = std::move(h)](const double* x, const double*) {
            return spectrum->GetBinContent(spectrum->FindFixBin(*x));
        }})};
    spectrum->SetNpx(nPoint);
    return spectrum;
}

auto MakeCustomSpectrum(const std::string& name, const TGraph& graph, int nPoint) -> std::unique_ptr<TF1> {
    std::shared_ptr<TGraph> g{dynamic_cast<TGraph*>(graph.Clone())};
    if (g->GetN() < 2) {
        Mustard::Throw<std::runtime_error>(fmt::format("A custom spectrum graph needs at least two points, but '{}' has {}", g->GetName(), g->GetN()));
    }

    auto spectrum{std::make_unique<TF1>(
        name.c_str(),
        std::function{[spectrum = std::move(g)](const double* x, const double*) {
            return spectrum->Eval(*x);
        }})};
    spectrum->SetNpx(nPoint);
    return spectrum;
}

template<typename T>
auto LoadCustomSpectrum(const std::string& name, const std::string& fileName, const std::string& objectName, int nPoint) -> std::unique_ptr<TF1> {
    const auto file{std::unique_ptr<TFile>{TFile::Open(fileName.c_str())}};
    if (file == nullptr) {
        Mustard::Throw<std::runtime_error>(fmt::format("Cannot open '{}'", fileName));
    }

    const auto spectrum{file->Get<T>(objectName.c_str())};
    if (spectrum == nullptr) {
        Mustard::Throw<std::runtime_error>(fmt::format("Cannot find object '{}' in '{}'", objectName, fileName));
    }

    return MakeCustomSpectrum(name, *spectrum, nPoint);
}

auto SyncCustomSpectrum(TF1& spectrum, double xMin, double xMax, int nPoint) -> void {
    if (spectrum.GetXmin() != xMin or spectrum.GetXmax() != xMax) {
        spectrum.SetRange(xMin, xMax);
    }
    if (spectrum.GetNpx() != nPoint) {
        spectrum.SetNpx(nPoint);
    }
}

} // namespace

PrimaryCosmicRayGenerator::PrimaryCosmicRayGenerator() :
    G4VPrimaryGenerator{},
    fParticle{},
    fEnergySpectrum{},
    fNEnergySpectrumPoint{10000},
    fIntrinsicMinEnergy{},
    fIntrinsicMaxEnergy{std::numeric_limits<double>::max()},
    fEnergySamplingMode{EnergySamplingMode::Normal},
    fCustomBiasedEnergySpectrum{},
    fDirectionSamplingMode{DirectionSamplingMode::Normal},
    fZenithBiasExponent{2},
    fNDirectionSpectrumPoint{10000},
    fCustomBiasedDirectionSpectrum{},
    fMessengerRegister{this} {
    Particle("proton");
    EnergySpectrum("x^-2.7");
    MinEnergy(3_GeV);
    MaxEnergy(10_TeV);
}

auto PrimaryCosmicRayGenerator::Particle(const std::string& particle) -> void {
    const auto p{G4ParticleTable::GetParticleTable()->FindParticle(particle)};
    if (p) {
        Particle(p);
    } else {
        Mustard::Throw<std::runtime_error>(fmt::format("No particle named '{}'", particle));
    }
}

auto PrimaryCosmicRayGenerator::EnergySpectrum(const std::string& formula) -> void {
    const auto eMin{fEnergySpectrum ? fEnergySpectrum->GetXmin() : fIntrinsicMinEnergy};
    const auto eMax{fEnergySpectrum ? fEnergySpectrum->GetXmax() : fIntrinsicMaxEnergy};
    ValidateEnergyRangeForSampling(fEnergySamplingMode, eMin);
    fEnergySpectrum = std::make_unique<TF1>("EnergySpectrum", formula.c_str(), eMin, eMax);
    fEnergySpectrum->SetNpx(fNEnergySpectrumPoint);
    fIntrinsicMinEnergy = 0;
    fIntrinsicMaxEnergy = std::numeric_limits<double>::max();
}

auto PrimaryCosmicRayGenerator::EnergySpectrum(const TH1& histogram) -> void {
    std::shared_ptr<TH1> h{dynamic_cast<TH1*>(histogram.Clone())};
    h->SetDirectory(nullptr);

    const auto spectrumAxis{h->GetXaxis()};
    const auto eMin{spectrumAxis->GetXmin()};
    const auto eMax{spectrumAxis->GetXmax()};
    ValidateEnergyRangeForSampling(fEnergySamplingMode, eMin);
    fIntrinsicMinEnergy = eMin;
    fIntrinsicMaxEnergy = eMax;

    fEnergySpectrum = std::make_unique<TF1>(
        "EnergySpectrum",
        std::function{[spectrum = std::move(h)](const double* x, const double*) {
            return spectrum->GetBinContent(spectrum->FindFixBin(*x));
        }},
        fIntrinsicMinEnergy, fIntrinsicMaxEnergy);
    fEnergySpectrum->SetNpx(fNEnergySpectrumPoint);
}

template<>
auto PrimaryCosmicRayGenerator::EnergySpectrum<TGraph>(const TGraph& graph) -> void {
    std::shared_ptr<TGraph> g{dynamic_cast<TGraph*>(graph.Clone())};
    if (g->GetN() < 2) {
        Mustard::Throw<std::runtime_error>(fmt::format("A spectrum graph needs at least two points, but '{}' has {}", g->GetName(), g->GetN()));
    }

    const auto eMin{g->GetX()[0]};
    const auto eMax{g->GetX()[g->GetN() - 1]};
    ValidateEnergyRangeForSampling(fEnergySamplingMode, eMin);
    fIntrinsicMinEnergy = eMin;
    fIntrinsicMaxEnergy = eMax;

    fEnergySpectrum = std::make_unique<TF1>(
        "EnergySpectrum",
        std::function{[spectrum = std::move(g)](const double* x, const double*) {
            return spectrum->Eval(*x);
        }},
        fIntrinsicMinEnergy, fIntrinsicMaxEnergy);
    fEnergySpectrum->SetNpx(fNEnergySpectrumPoint);
}

template<typename T>
auto PrimaryCosmicRayGenerator::EnergySpectrum(const std::string& fileName, const std::string& name) -> void {
    const auto file{std::unique_ptr<TFile>{TFile::Open(fileName.c_str())}};
    if (file == nullptr) {
        Mustard::Throw<std::runtime_error>(fmt::format("Cannot open '{}'", fileName));
    }

    const auto spectrum{file->Get<T>(name.c_str())};
    if (spectrum == nullptr) {
        Mustard::Throw<std::runtime_error>(fmt::format("Cannot find object '{}' in '{}'", name, fileName));
    }

    EnergySpectrum(*spectrum);
}
template auto PrimaryCosmicRayGenerator::EnergySpectrum<TH1>(const std::string& fileName, const std::string& name) -> void;
template auto PrimaryCosmicRayGenerator::EnergySpectrum<TGraph>(const std::string& fileName, const std::string& name) -> void;

auto PrimaryCosmicRayGenerator::CustomBiasedEnergySpectrum(const std::string& formula) -> void {
    fCustomBiasedEnergySpectrum = MakeCustomSpectrum("CustomBiasedEnergySpectrum", formula, fEnergySpectrum->GetNpx());
}

auto PrimaryCosmicRayGenerator::CustomBiasedEnergySpectrum(const TH1& histogram) -> void {
    fCustomBiasedEnergySpectrum = MakeCustomSpectrum("CustomBiasedEnergySpectrum", histogram, fEnergySpectrum->GetNpx());
}

template<>
auto PrimaryCosmicRayGenerator::CustomBiasedEnergySpectrum<TGraph>(const TGraph& graph) -> void {
    fCustomBiasedEnergySpectrum = MakeCustomSpectrum("CustomBiasedEnergySpectrum", graph, fEnergySpectrum->GetNpx());
}

template<typename T>
auto PrimaryCosmicRayGenerator::CustomBiasedEnergySpectrum(const std::string& fileName, const std::string& name) -> void {
    fCustomBiasedEnergySpectrum = LoadCustomSpectrum<T>("CustomBiasedEnergySpectrum", fileName, name, fEnergySpectrum->GetNpx());
}
template auto PrimaryCosmicRayGenerator::CustomBiasedEnergySpectrum<TH1>(const std::string& fileName, const std::string& name) -> void;
template auto PrimaryCosmicRayGenerator::CustomBiasedEnergySpectrum<TGraph>(const std::string& fileName, const std::string& name) -> void;

auto PrimaryCosmicRayGenerator::CustomBiasedDirectionSpectrum(const std::string& formula) -> void {
    fCustomBiasedDirectionSpectrum = MakeCustomSpectrum("CustomBiasedDirectionSpectrum", formula, fNDirectionSpectrumPoint);
}

auto PrimaryCosmicRayGenerator::CustomBiasedDirectionSpectrum(const TH1& histogram) -> void {
    fCustomBiasedDirectionSpectrum = MakeCustomSpectrum("CustomBiasedDirectionSpectrum", histogram, fNDirectionSpectrumPoint);
}

template<>
auto PrimaryCosmicRayGenerator::CustomBiasedDirectionSpectrum<TGraph>(const TGraph& graph) -> void {
    fCustomBiasedDirectionSpectrum = MakeCustomSpectrum("CustomBiasedDirectionSpectrum", graph, fNDirectionSpectrumPoint);
}

template<typename T>
auto PrimaryCosmicRayGenerator::CustomBiasedDirectionSpectrum(const std::string& fileName, const std::string& name) -> void {
    fCustomBiasedDirectionSpectrum = LoadCustomSpectrum<T>("CustomBiasedDirectionSpectrum", fileName, name, fNDirectionSpectrumPoint);
}
template auto PrimaryCosmicRayGenerator::CustomBiasedDirectionSpectrum<TH1>(const std::string& fileName, const std::string& name) -> void;
template auto PrimaryCosmicRayGenerator::CustomBiasedDirectionSpectrum<TGraph>(const std::string& fileName, const std::string& name) -> void;

auto PrimaryCosmicRayGenerator::NEnergySpectrumPoint(int n) -> void {
    if (not(n >= 10 and n <= 10'000'000)) {
        Mustard::Throw<std::runtime_error>(fmt::format("Energy spectrum NPoint must be between 10 and 10000000, but n = {}", n));
    }
    if (fEnergySpectrum) {
        fEnergySpectrum->SetNpx(n);
    }
    fNEnergySpectrumPoint = n;
}

auto PrimaryCosmicRayGenerator::MinEnergy(double val) -> void {
    const auto eMin{muc::clamp<"[]">(val, fIntrinsicMinEnergy, fIntrinsicMaxEnergy)};
    if (eMin != val) {
        Mustard::MasterPrintWarning(fmt::format("Minimum kinetic energy {} MeV is outside the spectrum range [{}, {}] MeV and is clamped to {} MeV",
                                                val, fIntrinsicMinEnergy, fIntrinsicMaxEnergy, eMin));
    }
    if (not(eMin < fEnergySpectrum->GetXmax())) {
        Mustard::Throw<std::runtime_error>(fmt::format("Minimum kinetic energy {} MeV is not below the maximum kinetic energy {} MeV",
                                                       eMin, fEnergySpectrum->GetXmax()));
    }
    ValidateEnergyRangeForSampling(fEnergySamplingMode, eMin);
    fEnergySpectrum->SetRange(eMin, fEnergySpectrum->GetXmax());
}

auto PrimaryCosmicRayGenerator::MaxEnergy(double val) -> void {
    const auto eMax{muc::clamp<"[]">(val, fIntrinsicMinEnergy, fIntrinsicMaxEnergy)};
    if (eMax != val) {
        Mustard::MasterPrintWarning(fmt::format("Maximum kinetic energy {} MeV is outside the spectrum range [{}, {}] MeV and is clamped to {} MeV",
                                                val, fIntrinsicMinEnergy, fIntrinsicMaxEnergy, eMax));
    }
    if (not(fEnergySpectrum->GetXmin() < eMax)) {
        Mustard::Throw<std::runtime_error>(fmt::format("Maximum kinetic energy {} MeV is not above the minimum kinetic energy {} MeV",
                                                       eMax, fEnergySpectrum->GetXmin()));
    }
    fEnergySpectrum->SetRange(fEnergySpectrum->GetXmin(), eMax);
}

auto PrimaryCosmicRayGenerator::ValidateEnergyRangeForSampling(enum EnergySamplingMode mode, double eMin) const -> void {
    if (mode != EnergySamplingMode::LogUniformBiased) {
        return;
    }
    if (not(eMin > 0)) {
        Mustard::Throw<std::runtime_error>(fmt::format("LogUniformBiased sampling requires E_min > 0, but E_min = {}", eMin));
    }
}

auto PrimaryCosmicRayGenerator::ValidateEnergySpectrumForSampling(enum EnergySamplingMode mode) const -> void {
    if (mode != EnergySamplingMode::LogUniformBiased) {
        return;
    }
    if (not fEnergySpectrum) {
        Mustard::Throw<std::runtime_error>("LogUniformBiased sampling requires an energy spectrum; set the spectrum before selecting the mode");
    }
    ValidateEnergyRangeForSampling(mode, fEnergySpectrum->GetXmin());
}

auto PrimaryCosmicRayGenerator::EnergySamplingMode(enum EnergySamplingMode mode) -> void {
    ValidateEnergySpectrumForSampling(mode);
    fEnergySamplingMode = mode;
}

auto PrimaryCosmicRayGenerator::DirectionSamplingMode(enum DirectionSamplingMode mode) -> void {
    fDirectionSamplingMode = mode;
}

auto PrimaryCosmicRayGenerator::ZenithBiasExponent(double n) -> void {
    if (not(std::isfinite(n) and n >= 0)) {
        Mustard::Throw<std::runtime_error>(fmt::format("Zenith bias exponent must be finite and non-negative, but n = {}", n));
    }
    fZenithBiasExponent = n;
}

auto PrimaryCosmicRayGenerator::NDirectionSpectrumPoint(int n) -> void {
    if (not(n >= 10 and n <= 10'000'000)) {
        Mustard::Throw<std::runtime_error>(fmt::format("Direction spectrum NPoint must be between 10 and 10000000, but n = {}", n));
    }
    fNDirectionSpectrumPoint = n;
}

auto PrimaryCosmicRayGenerator::GeneratePrimaryVertex(G4Event* event) -> void {
    using namespace Mustard::LiteralUnit::MathConstantSuffix;

    const auto hMax{Detector::Description::Atmosphere::Instance().MaxAltitude()};
    const auto zGround{Detector::Description::Earth::Instance().GroundAltitude()};
    const auto height{hMax - zGround};
    const auto halfWidth{Detector::Description::World::Instance().Width() / 2};
    const auto minCosTheta{height / muc::hypot(height, halfWidth)};

    const auto [ek, weight]{SampleEnergy()};
    const auto [pHat, directionWeight]{SampleDirection(minCosTheta)};
    const auto pathLength{height / -pHat.z()};
    particle_position = {-pathLength * pHat.x(), -pathLength * pHat.y(), hMax};
    const auto p{std::sqrt((2 * fParticle->GetPDGMass() + ek) * ek) * pHat};

    // clang-format off
    const auto vertex{new G4PrimaryVertex{particle_position, particle_time}}; // clang-format on
    vertex->SetWeight(weight * directionWeight);
    vertex->SetPrimary(new G4PrimaryParticle{fParticle, p.x(), p.y(), p.z()});
    event->AddPrimaryVertex(vertex);
}

auto PrimaryCosmicRayGenerator::SampleDirection(double minCosTheta) -> std::pair<G4ThreeVector, double> {
    using namespace Mustard::LiteralUnit::MathConstantSuffix;

    const auto minCosThetaSquared{minCosTheta * minCosTheta};
    const auto acceptance{1 - minCosThetaSquared};
    double cosTheta{};
    double directionWeight{1};
    switch (fDirectionSamplingMode) {
    case DirectionSamplingMode::Normal: {
        cosTheta = std::sqrt(G4RandFlat::shoot(minCosThetaSquared, 1.));
        directionWeight = acceptance;
    } break;
    case DirectionSamplingMode::ZenithPowerBiased: {
        const auto n{fZenithBiasExponent};
        const auto uMinPow{std::pow(minCosTheta, n + 1)};
        cosTheta = std::pow(uMinPow + G4RandFlat::shoot(0., 1.) * (1 - uMinPow), 1. / (n + 1));
        directionWeight = 2. / (n + 1) * (1 - uMinPow) * std::pow(cosTheta, 1 - n);
    } break;
    case DirectionSamplingMode::CustomBiased: {
        if (not fCustomBiasedDirectionSpectrum) [[unlikely]] {
            Mustard::Throw<std::runtime_error>("CustomBiased direction sampling requires a custom direction spectrum");
        }
        SyncCustomSpectrum(*fCustomBiasedDirectionSpectrum, minCosTheta, 1., fNDirectionSpectrumPoint);
        cosTheta = fCustomBiasedDirectionSpectrum->GetRandom();
        if (not std::isfinite(cosTheta)) [[unlikely]] {
            Mustard::Throw<std::runtime_error>("CustomBiased direction spectrum returned a non-finite value");
        }
        cosTheta = std::min(std::max(cosTheta, minCosTheta), 1.);
        const auto biased{(*fCustomBiasedDirectionSpectrum)(cosTheta)};
        if (not(biased > 0)) [[unlikely]] {
            Mustard::Throw<std::runtime_error>(fmt::format("CustomBiased direction spectrum must be positive, but g({}) = {}", cosTheta, biased));
        }
        directionWeight = acceptance * cosTheta / biased;
    } break;
    default: {
        muc::unreachable();
    }
    }
    const auto sinTheta{std::sqrt((1 - cosTheta) * (1 + cosTheta))};
    const auto phi{G4RandFlat::shoot(0., 2_pi)};
    const auto [sinPhi, cosPhi]{muc::sincos(phi)};
    const G4ThreeVector pHat{sinTheta * cosPhi, sinTheta * sinPhi, -cosTheta};
    return {pHat, directionWeight};
}

auto PrimaryCosmicRayGenerator::SampleEnergy() -> std::pair<double, double> {
    const auto option{fEnergySpectrum->GetXmin() > 0 ? "LOG" : nullptr};
    switch (fEnergySamplingMode) {
    case EnergySamplingMode::Normal: {
        const auto ek{fEnergySpectrum->GetRandom(nullptr, option)};
        if (not std::isfinite(ek)) [[unlikely]] {
            Mustard::Throw<std::runtime_error>("Energy spectrum returned a non-finite value");
        }
        return {ek, 1};
    }
    case EnergySamplingMode::UniformBiased: {
        const auto eMin{fEnergySpectrum->GetXmin()};
        const auto eMax{fEnergySpectrum->GetXmax()};
        const auto ek{G4RandFlat::shoot(eMin, eMax)};
        return {ek, (*fEnergySpectrum)(ek) * (eMax - eMin)};
    }
    case EnergySamplingMode::LogUniformBiased: {
        const auto eMin{fEnergySpectrum->GetXmin()};
        const auto eMax{fEnergySpectrum->GetXmax()};
        if (not(eMin > 0)) [[unlikely]] {
            Mustard::Throw<std::runtime_error>(fmt::format("LogUniformBiased sampling requires E_min > 0, but E_min = {}", eMin));
        }
        const auto ek{eMin * std::pow(eMax / eMin, G4RandFlat::shoot(0., 1.))};
        const auto fk{(*fEnergySpectrum)(ek)};
        return {ek, fk * ek * std::log(eMax / eMin)};
    }
    case EnergySamplingMode::CustomBiased: {
        if (not fCustomBiasedEnergySpectrum) [[unlikely]] {
            Mustard::Throw<std::runtime_error>("CustomBiased energy sampling requires a custom energy spectrum");
        }
        SyncCustomSpectrum(*fCustomBiasedEnergySpectrum, fEnergySpectrum->GetXmin(), fEnergySpectrum->GetXmax(), fEnergySpectrum->GetNpx());
        const auto ek{fCustomBiasedEnergySpectrum->GetRandom(nullptr, option)};
        if (not std::isfinite(ek)) [[unlikely]] {
            Mustard::Throw<std::runtime_error>("CustomBiased energy spectrum returned a non-finite value");
        }
        const auto g{(*fCustomBiasedEnergySpectrum)(ek)};
        if (not(g > 0)) [[unlikely]] {
            Mustard::Throw<std::runtime_error>(fmt::format("CustomBiased energy spectrum must be positive, but g({}) = {}", ek, g));
        }
        return {ek, (*fEnergySpectrum)(ek) / g};
    }
    }
    muc::unreachable();
}

} // namespace MusAirS::inline Generator
