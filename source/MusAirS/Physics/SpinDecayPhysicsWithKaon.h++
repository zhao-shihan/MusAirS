// -*- C++ -*-
//
// Copyright 2024-2025  Shihan Zhao
//
// This file is part of MusAirS, an air shower simulation tool.
//
// MusAirS is free software: you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the Free Software
// Foundation, either version 3 of the License, or (at your option) any later
// version.
//
// MusAirS is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with
// MusAirS. If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include "G4SpinDecayPhysics.hh"

namespace MusAirS::inline Physics {

class SpinDecayPhysicsWithKaon : public G4SpinDecayPhysics {
public:
    using G4SpinDecayPhysics::G4SpinDecayPhysics;

    auto ConstructProcess() -> void override;
};

} // namespace MusAirS::inline Physics
