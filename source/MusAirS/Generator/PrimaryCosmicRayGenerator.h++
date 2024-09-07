#pragma once

#include "G4VPrimaryGenerator.hh"

#include <memory>
#include <string>

class G4Event;
class G4ParticleDefinition;
class TH1;

namespace MusAirS::inline Generator {

class PrimaryCosmicRayGenerator : public G4VPrimaryGenerator {
public:
    PrimaryCosmicRayGenerator();

    auto PrimaryParticle(G4ParticleDefinition* particle) -> void { fPrimaryParticle = particle; }
    auto PrimaryParticle(const std::string& particle) -> void;

    auto EnergySpectrum(std::unique_ptr<TH1> h) -> void;
    auto EnergySpectrum(const std::string& fileName, const std::string& th1Name = "Ek");

    auto MinEnergy(double val) -> void;
    auto MaxEnergy(double val) -> void;

    virtual auto GeneratePrimaryVertex(G4Event* event) -> void override;

private:
    std::unique_ptr<TH1> fEnergySpectrum;
    double fMinEnergy;
    double fMaxEnergy;
    G4ParticleDefinition* fPrimaryParticle;
};

} // namespace MusAirS::inline Generator
