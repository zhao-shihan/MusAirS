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
