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

#include "Mustard/Data/Tuple.h++"
#include "Mustard/Data/Value.h++"

#include "muc/array"

#include <string>
#include <vector>

namespace MusAirS::Data {

using Track = Mustard::Data::TupleModel<
    Mustard::Data::Value<int, "EvtID", "Event ID">,
    Mustard::Data::Value<int, "ParTrkID", "Parent track ID">,
    Mustard::Data::Value<int, "ParPDGID", "Parent PDG ID">,
    Mustard::Data::Value<std::string, "CreatProc", "Track creator process">,
    Mustard::Data::Value<int, "TrkID", "Track ID">,
    Mustard::Data::Value<int, "PDGID", "Particle PDG ID">,
    Mustard::Data::Value<float, "t0", "Initial time">,
    Mustard::Data::Value<muc::array3f, "x0", "Initial position">,
    Mustard::Data::Value<float, "Ek0", "Initial kinetic energy">,
    Mustard::Data::Value<muc::array3f, "p0", "Initial momentum">,
    Mustard::Data::Value<muc::array3f, "Sigma0", "Initial spin direction">,
    Mustard::Data::Value<float, "Helicity0", "Initial helicity">,
    Mustard::Data::Value<float, "t", "Final time">,
    Mustard::Data::Value<muc::array3f, "x", "Final position">,
    Mustard::Data::Value<float, "Ek", "Final kinetic energy">,
    Mustard::Data::Value<muc::array3f, "p", "Final momentum">,
    Mustard::Data::Value<muc::array3f, "Sigma", "Final spin direction">,
    Mustard::Data::Value<float, "Helicity", "Final helicity">,
    Mustard::Data::Value<float, "Zenith", "Final 1 - cos(zenith angle)">,
    Mustard::Data::Value<float, "phi", "Final azimuth angle">,
    Mustard::Data::Value<float, "TrkLen", "Track total length">,
    Mustard::Data::Value<std::string, "KillProc", "Track killer process">,
    Mustard::Data::Value<float, "Weight", "Track weight">>;

} // namespace MusAirS::Data
