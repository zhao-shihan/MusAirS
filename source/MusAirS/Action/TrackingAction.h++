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

#include "MusAirS/Data/Track.h++"
#include "MusAirS/Messenger/AnalysisMessenger.h++"

#include "Mustard/Data/Tuple.h++"
#include "Mustard/Env/Memory/PassiveSingleton.h++"

#include "G4ThreeVector.hh"
#include "G4UserTrackingAction.hh"

#include <memory>
#include <memory_resource>
#include <unordered_map>
#include <vector>

namespace MusAirS::inline Action {

class TrackingAction final : public Mustard::Env::Memory::PassiveSingleton<TrackingAction>,
                             public G4UserTrackingAction {
public:
    using TrackDataType = std::pmr::unordered_map<int, Mustard::Data::Tuple<Data::Track>>;

public:
    TrackingAction();

    auto PreUserTrackingAction(const G4Track* track) -> void override;
    auto PostUserTrackingAction(const G4Track* track) -> void override;

    auto ClearTrackData() -> void { return fTrackData.clear(); }
    auto TrackData() -> auto& { return fTrackData; }

private:
    auto UpdateTrackData(const G4Track& track) -> void;

private:
    G4ThreeVector fVertexSigma;

    std::pmr::unsynchronized_pool_resource fMemoryPool;
    TrackDataType fTrackData;

    AnalysisMessenger::Register<TrackingAction> fMessengerRegister;
};

} // namespace MusAirS::inline Action
