#include "MusAirS/Action/RunAction.h++"
#include "MusAirS/Analysis.h++"

#include "G4Run.hh"

namespace MusAirS::inline Action {

RunAction::RunAction() :
    PassiveSingleton{this},
    G4UserRunAction{} {}

auto RunAction::BeginOfRunAction(const G4Run* run) -> void {
    Analysis::Instance().RunBeginAction(run->GetRunID());
}

auto RunAction::EndOfRunAction(const G4Run* run) -> void {
    Analysis::Instance().RunEndAction(run->GetRunID());
}

} // namespace MusAirS::inline Action
