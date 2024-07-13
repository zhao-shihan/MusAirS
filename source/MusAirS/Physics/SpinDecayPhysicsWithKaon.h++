#pragma once

#include "G4SpinDecayPhysics.hh"

namespace MusAirS::inline Physics {

class SpinDecayPhysicsWithKaon : public G4SpinDecayPhysics {
public:
    using G4SpinDecayPhysics::G4SpinDecayPhysics;

    auto ConstructProcess() -> void override;
};

} // namespace MusAirS::inline Physics
