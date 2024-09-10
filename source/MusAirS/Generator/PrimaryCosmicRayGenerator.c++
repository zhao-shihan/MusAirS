#include "MusAirS/Detector/Description/Atmosphere.h++"
#include "MusAirS/Detector/Description/World.h++"
#include "MusAirS/Generator/PrimaryCosmicRayGenerator.h++"

#include "Mustard/Utility/LiteralUnit.h++"

#include "TAxis.h"
#include "TF1.h"
#include "TFile.h"
#include "TH1.h"

#include "G4Event.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
#include "G4PrimaryParticle.hh"
#include "Randomize.hh"

#include "muc/math"
#include "muc/utility"

#include "fmt/core.h"

#include <cmath>
#include <functional>
#include <limits>
#include <stdexcept>

namespace MusAirS::inline Generator {

using namespace Mustard::LiteralUnit::Energy;

PrimaryCosmicRayGenerator::PrimaryCosmicRayGenerator() :
    G4VPrimaryGenerator{},
    fParticle{},
    fEnergySpectrum{},
    fMinVarBiasedEnergySpectrum{std::make_unique<TF1>("MinVarBiasedEnergySpectrum",
                                                      std::function{[this](const double* x, const double*) { return *x * (*fEnergySpectrum)(*x); }})},
    fNEnergySpectrumPoint{10000},
    fIntrinsicMinEnergy{},
    fIntrinsicMaxEnergy{std::numeric_limits<double>::max()},
    fEnergySampling{EnergySampling::Normal},
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
        throw std::runtime_error{fmt::format("MusAirS::PrimaryCosmicRayGenerator::Particle: No particle named '{}'", particle)};
    }
}

auto PrimaryCosmicRayGenerator::EnergySpectrum(const std::string& formula) -> void {
    fEnergySpectrum = std::make_unique<TF1>("EnergySpectrum", formula.c_str(),
                                            fEnergySpectrum ? fEnergySpectrum->GetXmin() : fIntrinsicMinEnergy,
                                            fEnergySpectrum ? fEnergySpectrum->GetXmax() : fIntrinsicMaxEnergy);
    fEnergySpectrum->SetNpx(fNEnergySpectrumPoint);
    SyncMinVarBiasedEnergySpectrum();
    fIntrinsicMinEnergy = 0;
    fIntrinsicMaxEnergy = std::numeric_limits<double>::max();
}

auto PrimaryCosmicRayGenerator::EnergySpectrum(const TH1& histogram) -> void {
    std::shared_ptr<TH1> h{dynamic_cast<TH1*>(histogram.Clone())};
    h->SetDirectory(nullptr);

    const auto spectrumAxis{h->GetXaxis()};
    fIntrinsicMinEnergy = spectrumAxis->GetXmin();
    fIntrinsicMaxEnergy = spectrumAxis->GetXmax();

    fEnergySpectrum = std::make_unique<TF1>(
        "EnergySpectrum",
        std::function{[spectrum = std::move(h)](const double* x, const double*) {
            return spectrum->Interpolate(*x);
        }},
        fIntrinsicMinEnergy, fIntrinsicMaxEnergy);
    fEnergySpectrum->SetNpx(fNEnergySpectrumPoint);
    SyncMinVarBiasedEnergySpectrum();
}

auto PrimaryCosmicRayGenerator::EnergySpectrum(const std::string& fileName, const std::string& th1Name) {
    const auto file{std::unique_ptr<TFile>{TFile::Open(fileName.c_str())}};
    if (file == nullptr) {
        throw std::runtime_error{fmt::format("MusAirS::PrimaryCosmicRayGenerator::EnergySpectrum: Cannot open '{}'", fileName)};
    }

    const auto histogram{file->Get<TH1>(th1Name.c_str())};
    if (histogram == nullptr) {
        throw std::runtime_error{fmt::format("MusAirS::PrimaryCosmicRayGenerator::EnergySpectrum: Cannot find TH1 '{}' in '{}'", th1Name, fileName)};
    }

    EnergySpectrum(*histogram);
}

auto PrimaryCosmicRayGenerator::NEnergySpectrumPoint(int n) -> void {
    if (fEnergySpectrum) {
        fEnergySpectrum->SetNpx(n);
        SyncMinVarBiasedEnergySpectrum();
    }
    fNEnergySpectrumPoint = n;
}

auto PrimaryCosmicRayGenerator::MinEnergy(double val) -> void {
    fEnergySpectrum->SetRange(muc::clamp<"[]">(val, fIntrinsicMinEnergy, fIntrinsicMaxEnergy),
                              fEnergySpectrum->GetXmax());
    SyncMinVarBiasedEnergySpectrum();
}

auto PrimaryCosmicRayGenerator::MaxEnergy(double val) -> void {
    fEnergySpectrum->SetRange(fEnergySpectrum->GetXmin(),
                              muc::clamp<"[]">(val, fIntrinsicMinEnergy, fIntrinsicMaxEnergy));
    SyncMinVarBiasedEnergySpectrum();
}

auto PrimaryCosmicRayGenerator::GeneratePrimaryVertex(G4Event* event) -> void {
    using namespace Mustard::LiteralUnit::MathConstantSuffix;

    const auto hMax{Detector::Description::Atmosphere::Instance().MaxAltitude()};
    particle_position = {0, 0, hMax};
    const auto wo2h{Detector::Description::World::Instance().Width() / (2 * hMax)};
    const auto maxCosTheta{-1 / std::sqrt(1 + muc::pow<2>(wo2h))};

    const auto [ek, weight]{SampleEnergy()};
    const auto pHat{[&maxCosTheta] {
        const auto z{G4RandFlat::shoot(-1, maxCosTheta)};
        const auto rho{std::sqrt((1 + z) * (1 - z))};
        const auto phi{G4RandFlat::shoot(0., 2_pi)};
        return G4ThreeVector{rho * std::cos(phi), rho * std::sin(phi), z};
    }()};
    const auto p{std::sqrt((2 * fParticle->GetPDGMass() + ek) * ek) * pHat};

    // clang-format off
    const auto vertex{new G4PrimaryVertex{particle_position, particle_time}}; // clang-format on
    vertex->SetWeight(weight);
    vertex->SetPrimary(new G4PrimaryParticle{fParticle, p.x(), p.y(), p.z()});
    event->AddPrimaryVertex(vertex);
}

auto PrimaryCosmicRayGenerator::SyncMinVarBiasedEnergySpectrum() -> void {
    fMinVarBiasedEnergySpectrum->SetRange(fEnergySpectrum->GetXmin(), fEnergySpectrum->GetXmax());
    fMinVarBiasedEnergySpectrum->SetNpx(fEnergySpectrum->GetNpx());
}

auto PrimaryCosmicRayGenerator::SampleEnergy() const -> std::pair<double, double> {
    switch (fEnergySampling) {
    case EnergySampling::Normal: {
        return {fEnergySpectrum->GetRandom(), 1};
    }
    case EnergySampling::WeightedUniform: {
        const auto ek{G4RandFlat::shoot(fEnergySpectrum->GetXmin(), fEnergySpectrum->GetXmax())};
        return {ek, (*fEnergySpectrum)(ek)};
    }
    case EnergySampling::WeightedMinVar: {
        const auto ek{fMinVarBiasedEnergySpectrum->GetRandom()};
        return {ek, 1 / ek};
    }
    }
    muc::unreachable();
}

} // namespace MusAirS::inline Generator
