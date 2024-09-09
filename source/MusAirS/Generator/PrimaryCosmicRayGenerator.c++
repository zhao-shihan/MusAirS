#include "MusAirS/Detector/Description/Atmosphere.h++"
#include "MusAirS/Detector/Description/World.h++"
#include "MusAirS/Generator/PrimaryCosmicRayGenerator.h++"

#include "TAxis.h"
#include "TF1.h"
#include "TFile.h"
#include "TH1.h"

#include "G4Event.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
#include "G4PrimaryParticle.hh"
#include "G4RandomTools.hh"

#include "muc/math"
#include "muc/utility"

#include "fmt/core.h"

#include <cmath>
#include <functional>
#include <limits>
#include <stdexcept>

namespace MusAirS::inline Generator {

PrimaryCosmicRayGenerator::PrimaryCosmicRayGenerator() :
    G4VPrimaryGenerator{},
    fParticle{},
    fEnergySpectrum{},
    fIntrinsicMinEnergy{},
    fIntrinsicMaxEnergy{std::numeric_limits<double>::max()},
    fEnergySampling{EnergySampling::Normal} {}

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
                                            fEnergySpectrum->GetXmin(), fEnergySpectrum->GetXmax());
    fIntrinsicMinEnergy = 0;
    fIntrinsicMaxEnergy = std::numeric_limits<double>::max();
}

auto PrimaryCosmicRayGenerator::EnergySpectrum(std::shared_ptr<TH1> h) -> void {
    const auto spectrumAxis{h->GetXaxis()};
    fIntrinsicMinEnergy = spectrumAxis->GetXmin();
    fIntrinsicMaxEnergy = spectrumAxis->GetXmax();
    std::function spectrum{
        [spectrum = std::move(h)](const double* x, const double*) {
            return spectrum->Interpolate(*x);
        }};
    fEnergySpectrum = std::make_unique<TF1>("EnergySpectrum", spectrum,
                                            fIntrinsicMinEnergy, fIntrinsicMaxEnergy);
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

    std::shared_ptr<TH1> h{dynamic_cast<TH1*>(histogram->Clone())};
    h->SetDirectory(nullptr);
    EnergySpectrum(std::move(h));
}

auto PrimaryCosmicRayGenerator::MinEnergy(double val) -> void {
    fEnergySpectrum->SetRange(muc::clamp<"[]">(val, fIntrinsicMinEnergy, fIntrinsicMaxEnergy),
                              fEnergySpectrum->GetXmax());
}

auto PrimaryCosmicRayGenerator::MaxEnergy(double val) -> void {
    fEnergySpectrum->SetRange(fEnergySpectrum->GetXmin(),
                              muc::clamp<"[]">(val, fIntrinsicMinEnergy, fIntrinsicMaxEnergy));
}

auto PrimaryCosmicRayGenerator::GeneratePrimaryVertex(G4Event* event) -> void {
    const auto hMax{Detector::Description::Atmosphere::Instance().MaxAltitude()};
    particle_position = {0, 0, hMax};
    const auto woh{Detector::Description::World::Instance().Width() / hMax};
    const auto maxCosTheta{woh / std::sqrt(4 + muc::pow<2>(woh))};

    const auto [ek, weight]{SampleEnergy()};
    const auto p{std::sqrt((2 * fParticle->GetPDGMass() + ek) * ek) *
                 G4RandomDirection(G4RandFlat::shoot(-1, maxCosTheta))};

    // clang-format off
    const auto vertex{new G4PrimaryVertex{particle_position, particle_time}}; // clang-format on
    vertex->SetWeight(weight);
    vertex->SetPrimary(new G4PrimaryParticle{fParticle, p.x(), p.y(), p.z()});
    event->AddPrimaryVertex(vertex);
}

auto PrimaryCosmicRayGenerator::SampleEnergy() const -> std::pair<double, double> {
    switch (fEnergySampling) {
    case EnergySampling::Normal: {
        return {fEnergySpectrum->GetRandom(), 1};
    }
    case EnergySampling::WeightedUniform: {
        const auto ek{G4RandFlat::shoot(fEnergySpectrum->GetXmin(), fEnergySpectrum->GetXmax())};
        return {ek, fEnergySpectrum->Eval(ek)};
    }
    }
    muc::unreachable();
}

} // namespace MusAirS::inline Generator
