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

    auto KillEMShower(bool val) -> void;
    auto KillNeutrino(bool val) -> void;
    auto KillChargedPion(bool val) -> void;

    auto UserSteppingAction(const G4Step* step) -> void override;

private:
    auto SetPhysicalProcessActivation(gsl::not_null<G4ParticleDefinition*> particle, bool active) -> void;

private:
    bool fKillEMShower;
    bool fKillNeutrino;
    bool fKillChargedPion;

    ActionMessenger::Register<SteppingAction> fActionMessengerRegister;

    static constexpr auto fgNStepLimit{1000'0000};
};

} // namespace MusAirS::inline Action
