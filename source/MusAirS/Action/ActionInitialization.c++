#include "MusAirS/Action/ActionInitialization.h++"
#include "MusAirS/Action/EventAction.h++"
#include "MusAirS/Action/PrimaryGeneratorAction.h++"
#include "MusAirS/Action/RunAction.h++"
#include "MusAirS/Action/TrackingAction.h++"

namespace MusAirS::inline Action {

auto ActionInitialization::Build() const -> void {
    SetUserAction(new RunAction);
    SetUserAction(new PrimaryGeneratorAction);
    SetUserAction(new EventAction);
    SetUserAction(new TrackingAction);
}

} // namespace MusAirS
