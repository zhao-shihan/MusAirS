#pragma once

#include "MusAirS/Messenger/PrimaryCosmicRayGeneratorMessenger.h++"

#include "G4VPrimaryGenerator.hh"

#include <concepts>
#include <memory>
#include <string>
#include <utility>

class G4Event;
class G4ParticleDefinition;
class TF1;
class TGraph;
class TH1;

namespace MusAirS::inline Generator {

class PrimaryCosmicRayGenerator : public G4VPrimaryGenerator {
public:
    enum struct EnergySampling {
        Normal,
        UniformBiased,
        MinVarBiased,
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
    auto EnergySampling(enum EnergySampling mode) -> void { fEnergySampling = mode; }

    auto CustomBiasedEnergySpectrum(const std::string& formula) -> void;
    auto CustomBiasedEnergySpectrum(const TH1& histogram) -> void;
    auto CustomBiasedEnergySpectrum(const std::same_as<TGraph> auto& graph) -> void;
    template<typename T>
    auto CustomBiasedEnergySpectrum(const std::string& fileName, const std::string& name) -> void;

    virtual auto GeneratePrimaryVertex(G4Event* event) -> void override;

private:
    auto SyncBiasedEnergySpectrum() -> void;

    auto SampleEnergy() const -> std::pair<double, double>;

private:
    G4ParticleDefinition* fParticle;

    std::unique_ptr<TF1> fEnergySpectrum;
    int fNEnergySpectrumPoint;
    double fIntrinsicMinEnergy;
    double fIntrinsicMaxEnergy;
    enum EnergySampling fEnergySampling;
    std::unique_ptr<TF1> fMinVarBiasedEnergySpectrum;
    std::unique_ptr<TF1> fCustomBiasedEnergySpectrum;

    PrimaryCosmicRayGeneratorMessenger::Register<PrimaryCosmicRayGenerator> fMessengerRegister;
};

} // namespace MusAirS::inline Generator
