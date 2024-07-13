#include "MusAirS/Physics/SpinDecayPhysicsWithKaon.h++"

#include "G4KaonMinus.hh"
#include "G4KaonPlus.hh"
#include "G4KaonZeroLong.hh"
#include "G4PionPlus.hh"
#include "G4ProcessManager.hh"
#include "G4ProcessTable.hh"

namespace MusAirS::inline Physics {

auto SpinDecayPhysicsWithKaon::ConstructProcess() -> void {
    G4SpinDecayPhysics::ConstructProcess();

    const auto processTable{G4ProcessTable::GetProcessTable()};
    const auto RegisterDecayMakeSpin{
        [&, decayMakeSpin = processTable->FindProcess("Decay", G4PionPlus::Definition())](auto&& particle) {
            const auto decay{processTable->FindProcess("Decay", particle)};
            const auto manager{particle->GetProcessManager()};
            if (manager) {
                if (decay) { manager->RemoveProcess(decay); }
                manager->AddProcess(decayMakeSpin);
                // set ordering for PostStepDoIt and AtRestDoIt
                manager->SetProcessOrdering(decayMakeSpin, idxPostStep);
                manager->SetProcessOrdering(decayMakeSpin, idxAtRest);
            }
        }};
    RegisterDecayMakeSpin(G4KaonPlus::Definition());
    RegisterDecayMakeSpin(G4KaonMinus::Definition());
    RegisterDecayMakeSpin(G4KaonZeroLong::Definition());
}

} // namespace MusAirS::inline Physics
