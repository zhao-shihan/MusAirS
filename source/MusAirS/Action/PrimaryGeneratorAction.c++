// -*- C++ -*-
//
// Copyright 2024-2026  Shihan Zhao
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

#include "MusAirS/Action/PrimaryGeneratorAction.h++"
#include "MusAirS/Analysis.h++"

#include "G4Event.hh"
#include "G4PrimaryVertex.hh"

namespace MusAirS::inline Action {

PrimaryGeneratorAction::PrimaryGeneratorAction() :
    PassiveSingleton{this},
    G4VUserPrimaryGeneratorAction{},
    fAvailableGenerator{},
    fGenerator{&fAvailableGenerator.pcr},
    fSavePrimaryVertexData{false},
    fPrimaryVertexData{},
    fActionMessengerRegister{this},
    fAnalysisMessengerRegister{this} {}

auto PrimaryGeneratorAction::GeneratePrimaries(G4Event* event) -> void {
    fGenerator->GeneratePrimaryVertex(event);
    if (fSavePrimaryVertexData) {
        UpdatePrimaryVertexData(*event);
    }
}

auto PrimaryGeneratorAction::UpdatePrimaryVertexData(const G4Event& event) -> void {
    fPrimaryVertexData.clear();
    fPrimaryVertexData.reserve(event.GetNumberOfPrimaryVertex());
    for (const auto* pv{event.GetPrimaryVertex()}; pv; pv = pv->GetNext()) {
        for (const auto* pp{pv->GetPrimary()}; pp; pp = pp->GetNext()) {
            auto& v{fPrimaryVertexData.emplace_back(Mustard::Data::MakeArcTuple<Data::PrimaryVertex>())};
            F<"EvtID">(*v) = event.GetEventID();
            F<"PDGID">(*v) = pp->GetPDGcode();
            F<"t0">(*v) = pv->GetT0();
            F<"x0">(*v) = pv->GetPosition();
            F<"Ek0">(*v) = pp->GetKineticEnergy();
            F<"p0">(*v) = pp->GetMomentum();
            F<"Weight">(*v) = pv->GetWeight();
        }
    }
    Analysis::Instance().SubmitPrimaryVertexData(fPrimaryVertexData);
}

} // namespace MusAirS::inline Action
