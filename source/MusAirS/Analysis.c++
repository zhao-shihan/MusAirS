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
#include "MusAirS/Action/TrackingAction.h++"
#include "MusAirS/Analysis.h++"

#include "Mustard/Geant4X/Management/Exception.h++"

#include "muc/algorithm"

#include "fmt/format.h"

namespace MusAirS {

Analysis::Analysis() :
    AnalysisBase{this},
    fCurrentRunID{},
    fWriterTarget{},
    fPrimaryVertexOutput{},
    fReactionChainOutput{},
    fPrimaryVertexData{},
    fTrackData{},
    fGroundHitTrackIDData{},
    fMessengerRegister{this} {}

auto Analysis::RunBeginUserAction(int runID) -> void {
    fCurrentRunID = runID;
    if (PrimaryGeneratorAction::Instance().SavePrimaryVertexData()) {
        const auto name{fmt::format("G4Run{}/PrimaryVertex", runID)};
        if (fWriterTarget) {
            fPrimaryVertexOutput.emplace(name, *fWriterTarget);
        } else {
            fPrimaryVertexOutput.emplace(name);
        }
    }
}

auto Analysis::EventEndUserAction() -> void {
    if (fPrimaryVertexOutput) {
        fPrimaryVertexOutput->Fill(*fPrimaryVertexData);
    }
    for (auto&& [pdgID, chain] : BuildReactionChain()) {
        const auto name{fmt::format("G4Run{}/ReactionChain({})", fCurrentRunID, pdgID)};
        auto it{fReactionChainOutput.end()};
        if (fWriterTarget) {
            it = fReactionChainOutput.try_emplace(pdgID, name, *fWriterTarget).first;
        } else {
            it = fReactionChainOutput.try_emplace(pdgID, name).first;
        }
        auto& output{it->second};
        Mustard::Data::ArcTupleVector<Data::Track> sorted;
        sorted.reserve(chain.size());
        for (auto trackID : chain) {
            sorted.push_back(fTrackData->at(trackID));
        }
        // Sort by track ID so that the parents always precede their children
        // and the output does not depend on the hash container's iteration order
        muc::timsort(sorted, [](const auto& lhs, const auto& rhs) { return F<"TrkID">(*lhs) < F<"TrkID">(*rhs); });
        output.Fill(sorted);
    }
    fPrimaryVertexData = {};
    fTrackData = {};
    fGroundHitTrackIDData = {};
}

auto Analysis::RunEndUserAction(int) -> void {
    // Writing is handled by the writers' destructors, so destroy them here,
    // before the analysis base closes the output file.
    fPrimaryVertexOutput.reset();
    fReactionChainOutput.clear();
}

auto Analysis::BuildReactionChain() const -> gtl::flat_hash_map<int, ReactionChain> {
    gtl::flat_hash_map<int, ReactionChain> chain;
    for (auto trackID : fGroundHitTrackIDData) {
        const auto it{fTrackData->find(trackID)};
        if (it == fTrackData->end()) [[unlikely]] {
            Mustard::Geant4X::Exception<FatalException>(
                fmt::format("Run {}: ground hit track {} is not in the track data.", fCurrentRunID, trackID));
            continue;
        }
        auto& track{*it->second};
        F<"KillProc">(track) = "<0|"; // track is killed at the ground
        BuildReactionChainImpl(trackID, chain[F<"PDGID">(track)]);
    }
    return chain;
}

auto Analysis::BuildReactionChainImpl(int trackID, ReactionChain& chain) const -> void {
    const auto it{fTrackData->find(trackID)};
    if (it == fTrackData->end()) [[unlikely]] {
        Mustard::Geant4X::Exception<FatalException>(
            fmt::format("Run {}: track {} is not in the track data.", fCurrentRunID, trackID));
        return;
    }
    auto& track{*it->second};
    if (const auto parTrkID{F<"ParTrkID">(track)}; parTrkID != -1) {
        const auto parentIt{fTrackData->find(parTrkID)};
        if (parentIt == fTrackData->end()) [[unlikely]] {
            Mustard::Geant4X::Exception<FatalException>(fmt::format(
                "Run {}: parent track {} of track {} is not in the track data.",
                fCurrentRunID, static_cast<int>(parTrkID), trackID));
            return;
        }
        auto& parentTrack{*parentIt->second};
        // Amend parent PDGID here
        F<"ParPDGID">(track) = F<"PDGID">(parentTrack);
        // Trace recursively
        BuildReactionChainImpl(parTrkID, chain);
    }
    // Insert into reaction chain
    chain.emplace(trackID);
}

} // namespace MusAirS
