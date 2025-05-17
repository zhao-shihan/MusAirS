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
