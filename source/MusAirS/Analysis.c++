#include "MusAirS/Action/PrimaryGeneratorAction.h++"
#include "MusAirS/Action/TrackingAction.h++"
#include "MusAirS/Analysis.h++"

#include "Mustard/Env/MPIEnv.h++"
#include "Mustard/Extension/Geant4X/Utility/ConvertGeometry.h++"
#include "Mustard/Extension/MPIX/ParallelizePath.h++"
#include "Mustard/Utility/VectorCast.h++"

#include "TFile.h"
#include "TMacro.h"

#include "Eigen/Core"

#include "fmt/format.h"

#include <cmath>
#include <numbers>
#include <optional>
#include <stdexcept>
#include <string_view>
#include <unordered_map>

namespace MusAirS {

Analysis::Analysis() :
    AnalysisBase{this},
    fCurrentRunID{},
    fPrimaryVertexOutput{},
    fReactionChainOutput{},
    fPrimaryVertexData{},
    fTrackData{},
    fEarthSDHitTrackIDData{},
    fMessengerRegister{this} {}

auto Analysis::RunBeginUserAction(int runID) -> void {
    fCurrentRunID = runID;
    if (PrimaryGeneratorAction::Instance().SavePrimaryVertexData()) { fPrimaryVertexOutput.emplace(fmt::format("G4Run{}/PrimaryVertex", runID)); }
}

auto Analysis::EventEndUserAction() -> void {
    if (fPrimaryVertexOutput) { fPrimaryVertexOutput->Fill(*fPrimaryVertexData); }
    for (auto&& [pdgID, chain] : BuildReactionChain()) {
        const auto [it, _]{fReactionChainOutput.try_emplace(pdgID, fmt::format("G4Run{}/ReactionChain({})", fCurrentRunID, pdgID))};
        auto&& [__, output]{*it};
        output.Fill(std::move(chain));
    }
    fPrimaryVertexData = {};
    fTrackData = {};
    fEarthSDHitTrackIDData = {};
}

auto Analysis::RunEndUserAction(int) -> void {
    // write data
    if (fPrimaryVertexOutput) { fPrimaryVertexOutput->Write(); }
    for (auto&& [_, output] : fReactionChainOutput) { output.Write(); }
    // reset output
    fPrimaryVertexOutput.reset();
    fReactionChainOutput.clear();
}

auto Analysis::BuildReactionChain() const -> std::unordered_map<int, ReactionChain> {
    std::unordered_map<int, ReactionChain> chain;
    for (auto&& trackID : *fEarthSDHitTrackIDData) {
        auto& track{fTrackData->at(trackID)};
        Get<"KillProc">(track) = "<0|"; // track is killed by EarthSD
        BuildReactionChainImpl(track, chain[Get<"PDGID">(track)]);
    }
    return chain;
}

auto Analysis::BuildReactionChainImpl(Mustard::Data::Tuple<Data::Track>& track, ReactionChain& chain) const -> void {
    if (Get<"ParTrkID">(track) != -1) {
        auto& parentTrack{fTrackData->at(Get<"ParTrkID">(track))};
        // Amend parent PDGID here
        Get<"ParPDGID">(track) = Get<"PDGID">(parentTrack);
        // Trace recursively
        BuildReactionChainImpl(parentTrack, chain);
    }
    // Amend Zenith and phi here
    const auto p{Mustard::VectorCast<Eigen::Vector3d>(Get<"p">(track))};
    Get<"Zenith">(track) = 1 + p.z() / p.norm();
    Get<"phi">(track) = std::atan2(p.x(), p.y()) + std::numbers::pi;
    // Insert into reaction chain
    chain.emplace(&track);
}

} // namespace MusAirS
