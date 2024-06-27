#include "MusAirS/Action/RunAction.h++"
#include "MusAirS/Analysis.h++"

#include "G4Run.hh"

namespace MusAirS::inline Action {

auto RunAction::BeginOfRunAction(const G4Run* run) -> void {
    Analysis::Instance().RunBegin(run->GetRunID());
}

auto RunAction::EndOfRunAction(const G4Run*) -> void {
    Analysis::Instance().RunEnd();
}

} // namespace MusAirS::inline Action
