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

#include "MusAirS/Action/PrimaryGeneratorAction.h++"
#include "MusAirS/Action/TrackingAction.h++"
#include "MusAirS/Data/Track.h++"
#include "MusAirS/Messenger/AnalysisMessenger.h++"
#include "MusAirS/SD/EarthSD.h++"

#include "Mustard/Data/Output.h++"
#include "Mustard/Data/Tuple.h++"
#include "Mustard/Simulation/AnalysisBase.h++"

#include "G4Types.hh"

#include "gsl/gsl"

#include <filesystem>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <utility>

class TFile;

namespace MusAirS {

class Analysis final : public Mustard::Simulation::AnalysisBase<Analysis, "MusAirs"> {
public:
    Analysis();

    auto SubmitPrimaryVertexData(const typename PrimaryGeneratorAction::PrimaryVertexDataType& data) -> void { fPrimaryVertexData = &data; }
    auto SubmitTrackData(typename TrackingAction::TrackDataType& data) -> void { fTrackData = &data; }
    auto SubmitEarthSDHitTrackID(const typename EarthSD::HitTrackIDDataType& hc) -> void { fEarthSDHitTrackIDData = &hc; }

private:
    auto RunBeginUserAction(int runID) -> void override;
    auto EventEndUserAction() -> void override;
    auto RunEndUserAction(int) -> void override;

    using ReactionChain = std::unordered_set<Mustard::Data::Tuple<Data::Track>*>;
    auto BuildReactionChain() const -> std::unordered_map<int, ReactionChain>;
    auto BuildReactionChainImpl(Mustard::Data::Tuple<Data::Track>& track, ReactionChain& chain) const -> void;

private:
    int fCurrentRunID;

    std::optional<Mustard::Data::Output<Data::PrimaryVertex>> fPrimaryVertexOutput;
    std::unordered_map<int, Mustard::Data::Output<Data::Track>> fReactionChainOutput;

    const typename PrimaryGeneratorAction::PrimaryVertexDataType* fPrimaryVertexData;
    typename TrackingAction::TrackDataType* fTrackData;
    const typename EarthSD::HitTrackIDDataType* fEarthSDHitTrackIDData;

    AnalysisMessenger::Register<Analysis> fMessengerRegister;
};

} // namespace MusAirS
