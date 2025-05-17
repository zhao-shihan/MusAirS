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

#include "Mustard/Data/TupleModel.h++"
#include "Mustard/Data/Value.h++"

#include "muc/array"

namespace MusAirS::Data {

using PrimaryVertex = Mustard::Data::TupleModel<
    Mustard::Data::Value<int, "EvtID", "Event ID">,
    Mustard::Data::Value<int, "PDGID", "PDG ID">,
    Mustard::Data::Value<double, "t0", "Primary time">,
    Mustard::Data::Value<muc::array3f, "x0", "Primary position">,
    Mustard::Data::Value<float, "Ek0", "Primary kinetic energy">,
    Mustard::Data::Value<muc::array3f, "p0", "Primary momentum">,
    Mustard::Data::Value<float, "Weight", "Track weight">>;

} // namespace MusAirS::Data
