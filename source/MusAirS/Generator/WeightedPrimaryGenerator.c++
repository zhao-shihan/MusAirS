#include "MusAirS/Generator/WeightedPrimaryGenerator.h++"

#include "TFile.h"
#include "TH1.h"

#include "G4Event.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
#include "G4PrimaryParticle.hh"
#include "G4RandomTools.hh"

#include "fmt/core.h"

#include <cmath>
#include <stdexcept>

namespace MusAirS::inline Generator {

WeightedPrimaryGenerator::WeightedPrimaryGenerator() :
    fEkHistogram{},
    fEkMin{},
    fEkMax{},
    fPrimaryParticle{} {}

auto WeightedPrimaryGenerator::PrimaryParticle(const std::string& particle) -> void {
    const auto p{G4ParticleTable::GetParticleTable()->FindParticle(particle)};
    if (p) {
        PrimaryParticle(p);
    } else {
        throw std::runtime_error{fmt::format("MusAirS::WeightedPrimaryGenerator::PrimaryParticle: No particle named '{}'", particle)};
    }
}

auto WeightedPrimaryGenerator::ApplyEkHistogram(std::unique_ptr<TH1> h) -> void {
    fEkHistogram = std::move(h);

    const auto iMin{fEkHistogram->GetMinimumBin()};
    const auto iMax{fEkHistogram->GetMaximumBin()};
    fEkMin = fEkHistogram->GetBinLowEdge(iMin);
    fEkMax = fEkHistogram->GetBinLowEdge(iMax) + fEkHistogram->GetBinWidth(iMax);
}

auto WeightedPrimaryGenerator::ApplyEkHistogram(const std::string& fileName, const std::string& th1Name) {
    const auto file{std::unique_ptr<TFile>{TFile::Open(fileName.c_str())}};
    if (file == nullptr) {
        throw std::runtime_error{fmt::format("MusAirS::WeightedPrimaryGenerator::ApplyEkHistogram: Cannot open '{}'", fileName)};
    }

    const auto histogram{file->Get<TH1>(th1Name.c_str())};
    if (histogram == nullptr) {
        throw std::runtime_error{fmt::format("MusAirS::WeightedPrimaryGenerator::ApplyEkHistogram: Cannot find TH1 '{}' in '{}'", th1Name, fileName)};
    }

    ApplyEkHistogram(std::unique_ptr<TH1>{dynamic_cast<TH1*>(histogram->Clone())});
}

auto WeightedPrimaryGenerator::GeneratePrimaryVertex(G4Event* event) -> void {
    const auto kineticEnergy{G4RandFlat::shoot(fEkMin, fEkMax)};
    const auto p{std::sqrt((2 * fPrimaryParticle->GetPDGMass() + kineticEnergy) * kineticEnergy) * G4RandomDirection()}; // clang-format off
    const auto vertex{new G4PrimaryVertex{particle_position, particle_time}}; // clang-format on
    vertex->SetPrimary(new G4PrimaryParticle{fPrimaryParticle, p.x(), p.y(), p.z()});
    vertex->SetWeight(fEkHistogram->Interpolate(kineticEnergy));
    event->AddPrimaryVertex(vertex);
}

} // namespace MusAirS::inline Generator
