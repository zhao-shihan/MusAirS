#pragma once

#include "MusAirS/Data/PrimaryVertex.h++"
#include "MusAirS/Generator/PrimaryCosmicRayGenerator.h++"
#include "MusAirS/Messenger/ActionMessenger.h++"
#include "MusAirS/Messenger/AnalysisMessenger.h++"

#include "Mustard/Data/Tuple.h++"
#include "Mustard/Env/Memory/PassiveSingleton.h++"
#include "Mustard/Extension/Geant4X/Generator/GeneralParticleSourceX.h++"

#include "G4VUserPrimaryGeneratorAction.hh"

#include "muc/ptr_vector"

#include <memory>
#include <vector>

namespace MusAirS::inline Action {

class PrimaryGeneratorAction final : public Mustard::Env::Memory::PassiveSingleton<PrimaryGeneratorAction>,
                                     public G4VUserPrimaryGeneratorAction {
public:
    using PrimaryVertexDataType = muc::unique_ptr_vector<Mustard::Data::Tuple<Data::PrimaryVertex>>;

public:
    PrimaryGeneratorAction();

    auto SwitchToPCR() -> void { fGenerator = &fAvailableGenerator.pcr; }
    auto SwitchToGPSX() -> void { fGenerator = &fAvailableGenerator.gpsx; }

    auto SavePrimaryVertexData() const -> auto { return fSavePrimaryVertexData; }
    auto SavePrimaryVertexData(bool val) -> void { fSavePrimaryVertexData = val; }

    auto GeneratePrimaries(G4Event* event) -> void override;

private:
    auto UpdatePrimaryVertexData(const G4Event& event) -> void;

private:
    struct {
        PrimaryCosmicRayGenerator pcr;
        Mustard::Geant4X::GeneralParticleSourceX gpsx;
    } fAvailableGenerator;
    G4VPrimaryGenerator* fGenerator;

    bool fSavePrimaryVertexData;
    PrimaryVertexDataType fPrimaryVertexData;

    ActionMessenger::Register<PrimaryGeneratorAction> fActionMessengerRegister;
    AnalysisMessenger::Register<PrimaryGeneratorAction> fAnalysisMessengerRegister;
};

} // namespace MusAirS::inline Action
