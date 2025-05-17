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

#include "MusAirS/Action/EventAction.h++"
#include "MusAirS/Action/PrimaryGeneratorAction.h++"
#include "MusAirS/Action/TrackingAction.h++"
#include "MusAirS/Analysis.h++"

#include "G4Event.hh"

namespace MusAirS::inline Action {

EventAction::EventAction() :
    PassiveSingleton{this},
    G4UserEventAction{} {}

auto EventAction::BeginOfEventAction(const G4Event*) -> void {
    TrackingAction::Instance().ClearTrackData();
}

auto EventAction::EndOfEventAction(const G4Event*) -> void {
    auto& analysis{Analysis::Instance()};
    analysis.SubmitTrackData(TrackingAction::Instance().TrackData());
    analysis.EventEndAction();
}

} // namespace MusAirS::inline Action
