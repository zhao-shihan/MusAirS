#pragma once

#include "MusAirS/Data/Track.h++"
#include "MusAirS/Messenger/AnalysisMessenger.h++"

#include "Mustard/Data/Tuple.h++"
#include "Mustard/Env/Memory/PassiveSingleton.h++"

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

    auto PostUserTrackingAction(const G4Track* track) -> void override;

    auto ClearTrackData() -> void { return fTrackData.clear(); }
    auto TrackData() -> auto& { return fTrackData; }

private:
    auto UpdateTrackData(const G4Track& track) -> void;

private:
    std::pmr::unsynchronized_pool_resource fMemoryPool;
    TrackDataType fTrackData;

    AnalysisMessenger::Register<TrackingAction> fMessengerRegister;
};

} // namespace MusAirS::inline Action
