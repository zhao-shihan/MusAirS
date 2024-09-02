#pragma once

#include "MusAirS/Messenger/ActionMessenger.h++"
#include "MusAirS/Messenger/SDMessenger.h++"

#include "Mustard/Env/Memory/PassiveSingleton.h++"

#include "G4UserSteppingAction.hh"

namespace MusAirS::inline Action {

class SteppingAction final : public Mustard::Env::Memory::PassiveSingleton<SteppingAction>,
                             public G4UserSteppingAction {
public:
    SteppingAction();

    auto KillNeutrino() const -> auto { return fKillNeutrino; }
    auto KillChargedPion() const -> auto { return fKillChargedPion; }

    auto KillNeutrino(bool val) -> auto { fKillNeutrino = val; }
    auto KillChargedPion(bool val) -> auto { fKillChargedPion = val; }

    auto UserSteppingAction(const G4Step* step) -> void override;

private:
    bool fKillNeutrino;
    bool fKillChargedPion;

    ActionMessenger::Register<SteppingAction> fActionMessengerRegister;
    SDMessenger::Register<SteppingAction> fSDMessengerRegister;

    static constexpr auto fgNStepLimit{1000'0000};
};

} // namespace MusAirS::inline Action
