#pragma once

#include "MusAirS/Action/PrimaryGeneratorAction.h++"
#include "MusAirS/Action/TrackingAction.h++"
#include "MusAirS/Data/Track.h++"
#include "MusAirS/Messenger/AnalysisMessenger.h++"
#include "MusAirS/SD/EarthSD.h++"

#include "Mustard/Data/Output.h++"
#include "Mustard/Data/Tuple.h++"
#include "Mustard/Env/Memory/PassiveSingleton.h++"

#include "G4Types.hh"

#include "gsl/gsl"

#include <filesystem>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <utility>

class TFile;

namespace MusAirS {

class Analysis final : public Mustard::Env::Memory::PassiveSingleton<Analysis> {
public:
    Analysis();

    auto FilePath(std::filesystem::path path) -> void { fFilePath = std::move(path); }
    auto FileMode(std::string mode) -> void { fFileMode = std::move(mode); }

    auto RunBegin(int runID) -> void;

    auto SubmitPrimaryVertexData(const typename PrimaryGeneratorAction::PrimaryVertexDataType& data) -> void { fPrimaryVertexData = &data; }
    auto SubmitTrackData(typename TrackingAction::TrackDataType& data) -> void { fTrackData = &data; }
    auto SubmitEarthSDHitTrackID(const typename EarthSD::HitTrackIDDataType& hc) -> void { fEarthSDHitTrackIDData = &hc; }
    auto EventEnd() -> void;

    auto RunEnd(Option_t* option = {}) -> void;

private:
    using ReactionChain = std::unordered_set<Mustard::Data::Tuple<Data::Track>*>;
    auto BuildReactionChain() const -> std::unordered_map<int, ReactionChain>;
    auto BuildReactionChainImpl(Mustard::Data::Tuple<Data::Track>& track, ReactionChain& chain) const -> void;

private:
    std::filesystem::path fFilePath;
    std::string fFileMode;

    int fCurrentRunID;
    std::filesystem::path fLastUsedFullFilePath;

    gsl::owner<TFile*> fFile;
    std::optional<Mustard::Data::Output<Data::PrimaryVertex>> fPrimaryVertexOutput;
    std::unordered_map<int, Mustard::Data::Output<Data::Track>> fReactionChainOutput;

    const typename PrimaryGeneratorAction::PrimaryVertexDataType* fPrimaryVertexData;
    typename TrackingAction::TrackDataType* fTrackData;
    const typename EarthSD::HitTrackIDDataType* fEarthSDHitTrackIDData;

    AnalysisMessenger::Register<Analysis> fMessengerRegister;
};

} // namespace MusAirS
