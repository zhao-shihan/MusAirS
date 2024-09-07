#include "MusAirS/Generator/PrimaryCosmicRayGenerator.h++"

#include "TAxis.h"
#include "TFile.h"
#include "TH1.h"

#include "G4Event.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
#include "G4PrimaryParticle.hh"
#include "G4RandomTools.hh"

#include "muc/math"

#include "fmt/core.h"

#include <cmath>
#include <stdexcept>
#include <utility>

namespace MusAirS::inline Generator {

PrimaryCosmicRayGenerator::PrimaryCosmicRayGenerator() :
    fEnergySpectrum{},
    fMinEnergy{},
    fMaxEnergy{},
    fPrimaryParticle{} {}

auto PrimaryCosmicRayGenerator::PrimaryParticle(const std::string& particle) -> void {
    const auto p{G4ParticleTable::GetParticleTable()->FindParticle(particle)};
    if (p) {
        PrimaryParticle(p);
    } else {
        throw std::runtime_error{fmt::format("MusAirS::PrimaryCosmicRayGenerator::PrimaryParticle: No particle named '{}'", particle)};
    }
}

auto PrimaryCosmicRayGenerator::EnergySpectrum(std::unique_ptr<TH1> h) -> void {
    fEnergySpectrum = std::move(h);
    const auto spectrumAxis{fEnergySpectrum->GetXaxis()};
    fMinEnergy = spectrumAxis->GetXmin();
    fMaxEnergy = spectrumAxis->GetXmax();
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

    std::unique_ptr<TH1> h{dynamic_cast<TH1*>(histogram->Clone())};
    h->SetDirectory(nullptr);
    EnergySpectrum(std::move(h));
}

auto PrimaryCosmicRayGenerator::MinEnergy(double val) -> void {
    const auto spectrumAxis{fEnergySpectrum->GetXaxis()};
    fMinEnergy = muc::clamp<"[]">(val,
                                  spectrumAxis->GetXmin(),
                                  spectrumAxis->GetXmax());
}

auto PrimaryCosmicRayGenerator::MaxEnergy(double val) -> void {
    const auto spectrumAxis{fEnergySpectrum->GetXaxis()};
    fMaxEnergy = muc::clamp<"[]">(val,
                                  spectrumAxis->GetXmin(),
                                  spectrumAxis->GetXmax());
}

auto PrimaryCosmicRayGenerator::GeneratePrimaryVertex(G4Event* event) -> void {
    const auto kineticEnergy{G4RandFlat::shoot(fMinEnergy, fMaxEnergy)};
    const auto p{std::sqrt((2 * fPrimaryParticle->GetPDGMass() + kineticEnergy) * kineticEnergy) *
                 G4RandomDirection(G4RandFlat::shoot(-1, 0))}; // clang-format off
    const auto vertex{new G4PrimaryVertex{particle_position, particle_time}}; // clang-format on
    vertex->SetPrimary(new G4PrimaryParticle{fPrimaryParticle, p.x(), p.y(), p.z()});
    vertex->SetWeight(fEnergySpectrum->Interpolate(kineticEnergy));
    event->AddPrimaryVertex(vertex);
}

} // namespace MusAirS::inline Generator
