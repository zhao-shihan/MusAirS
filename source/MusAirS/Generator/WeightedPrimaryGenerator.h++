#pragma once

#include "G4VPrimaryGenerator.hh"

#include <memory>
#include <string>
#include <utility>

class G4Event;
class G4ParticleDefinition;
class TH1;

namespace MusAirS::inline Generator {

class WeightedPrimaryGenerator : public G4VPrimaryGenerator {
public:
    WeightedPrimaryGenerator();

    auto PrimaryParticle(G4ParticleDefinition* particle) -> void { fPrimaryParticle = particle; }
    auto PrimaryParticle(const std::string& particle) -> void;

    auto ApplyEkHistogram(std::unique_ptr<TH1> h) -> void;
    auto ApplyEkHistogram(const std::string& fileName, const std::string& th1Name = "Ek");

    virtual auto GeneratePrimaryVertex(G4Event* event) -> void override;

private:
    std::unique_ptr<TH1> fEkHistogram;
    double fEkMin;
    double fEkMax;
    G4ParticleDefinition* fPrimaryParticle;
};

} // namespace MusAirS::inline Generator
