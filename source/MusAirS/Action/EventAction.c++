#include "MusAirS/Action/EventAction.h++"
#include "MusAirS/Action/PrimaryGeneratorAction.h++"
#include "MusAirS/Action/TrackingAction.h++"
#include "MusAirS/Analysis.h++"

#include "G4Event.hh"

namespace MusAirS::inline Action {

auto EventAction::BeginOfEventAction(const G4Event*) -> void {
    TrackingAction::Instance().ClearTrackData();
}

auto EventAction::EndOfEventAction(const G4Event*) -> void {
    auto& analysis{Analysis::Instance()};
    analysis.SubmitTrackData(TrackingAction::Instance().TrackData());
    analysis.EventEnd();
}

} // namespace MusAirS::inline Action
