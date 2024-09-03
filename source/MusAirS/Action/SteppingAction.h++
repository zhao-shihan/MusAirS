#pragma once

#include "MusAirS/Messenger/ActionMessenger.h++"
#include "MusAirS/Messenger/SDMessenger.h++"

#include "Mustard/Env/Memory/PassiveSingleton.h++"

#include "G4UserSteppingAction.hh"

#include "gsl/gsl"

class G4ParticleDefinition;

namespace MusAirS::inline Action {

class SteppingAction final : public Mustard::Env::Memory::PassiveSingleton<SteppingAction>,
                             public G4UserSteppingAction {
public:
    SteppingAction();

    auto KillEMShower() const -> auto { return fKillEMShower; }
    auto KillNeutrino() const -> auto { return fKillNeutrino; }
    auto KillChargedPion() const -> auto { return fKillChargedPion; }

    auto KillEMShower(bool val) -> void { fKillEMShower = val; }
    auto KillNeutrino(bool val) -> void { fKillNeutrino = val; }
    auto KillChargedPion(bool val) -> void { fKillChargedPion = val; }

    auto UserSteppingAction(const G4Step* step) -> void override;

private:
    bool fKillEMShower;
    bool fKillNeutrino;
    bool fKillChargedPion;

    ActionMessenger::Register<SteppingAction> fActionMessengerRegister;

    static constexpr auto fgNStepLimit{1000'0000};
};

} // namespace MusAirS::inline Action
