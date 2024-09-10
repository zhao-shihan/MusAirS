#pragma once

#include "MusAirS/Messenger/PrimaryCosmicRayGeneratorMessenger.h++"

#include "G4VPrimaryGenerator.hh"

#include <memory>
#include <string>
#include <utility>

class G4Event;
class G4ParticleDefinition;
class TF1;
class TH1;

namespace MusAirS::inline Generator {

class PrimaryCosmicRayGenerator : public G4VPrimaryGenerator {
public:
    enum struct EnergySampling {
        Normal,
        WeightedUniform,
        WeightedMinVar
    };

public:
    PrimaryCosmicRayGenerator();

    auto Particle(G4ParticleDefinition* particle) -> void { fParticle = particle; }
    auto Particle(const std::string& particle) -> void;

    auto EnergySpectrum(const std::string& formula) -> void;
    auto EnergySpectrum(const TH1& histogram) -> void;
    auto EnergySpectrum(const std::string& fileName, const std::string& th1Name);

    auto NEnergySpectrumPoint(int n) -> void;
    auto MinEnergy(double val) -> void;
    auto MaxEnergy(double val) -> void;
    auto EnergySampling(enum EnergySampling mode) -> void { fEnergySampling = mode; }

    virtual auto GeneratePrimaryVertex(G4Event* event) -> void override;

private:
    auto SyncMinVarBiasedEnergySpectrum() -> void;

    auto SampleEnergy() const -> std::pair<double, double>;

private:
    G4ParticleDefinition* fParticle;

    std::unique_ptr<TF1> fEnergySpectrum;
    std::unique_ptr<TF1> fMinVarBiasedEnergySpectrum;
    int fNEnergySpectrumPoint;
    double fIntrinsicMinEnergy;
    double fIntrinsicMaxEnergy;
    enum EnergySampling fEnergySampling;

    PrimaryCosmicRayGeneratorMessenger::Register<PrimaryCosmicRayGenerator> fMessengerRegister;
};

} // namespace MusAirS::inline Generator
