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

#include "MusAirS/Action/StackingAction.h++"

#include "G4ParticleDefinition.hh"
#include "G4Track.hh"

#include "muc/math"

namespace MusAirS::inline Action {

StackingAction::StackingAction() :
    PassiveSingleton{this},
    G4UserStackingAction{},
    fKillEMShower{false},
    fKillNeutrino{true},
    fKillChargedPion{false},
    fActionMessengerRegister{this} {}

auto StackingAction::ClassifyNewTrack(const G4Track* track) -> G4ClassificationOfNewTrack {
    const auto pdgID{track->GetParticleDefinition()->GetPDGEncoding()};
    enum { gamma = 22,
           e = 11,
           nuE = 12,
           nuMu = 14,
           nuTau = 16,
           pi = 211 };
    const auto absPDGID{muc::abs(pdgID)};
    auto mustKill{false};
    mustKill = mustKill or (fKillEMShower and (absPDGID == gamma or absPDGID == e));
    mustKill = mustKill or (fKillNeutrino and (absPDGID == nuE or absPDGID == nuMu or absPDGID == nuTau));
    mustKill = mustKill or (fKillChargedPion and absPDGID == pi);
    if (mustKill) {
        return fKill;
    }
    return fUrgent;
}

} // namespace MusAirS::inline Action
