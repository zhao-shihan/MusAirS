#pragma once

#include "Mustard/Env/Memory/PassiveSingleton.h++"

#include "G4UserSteppingAction.hh"

namespace MusAirS::inline Action {

class SteppingAction final : public Mustard::Env::Memory::PassiveSingleton<SteppingAction>,
                             public G4UserSteppingAction {
public:
    auto UserSteppingAction(const G4Step* step) -> void override;

private:
    static constexpr auto fgNStepLimit{1000'0000};
};

} // namespace MusAirS::inline Action
