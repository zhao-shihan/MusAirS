// -*- C++ -*-
//
// Copyright 2024-2026  Shihan Zhao
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

#include "Mustard/Data/Model.h++"
#include "Mustard/Data/Object/Field.h++"

#include "muc/array"

#include <cstdint>

namespace MusAirS::Data {

struct PrimaryVertex final
    : Mustard::Data::Model<
          Mustard::Data::Field<int, "EvtID", std::int32_t, "Event ID">,
          Mustard::Data::Field<int, "PDGID", std::int32_t, "PDG ID">,
          Mustard::Data::Field<double, "t0", double, "Primary time">,
          Mustard::Data::Field<muc::array3f, "x0", muc::array3f, "Primary position">,
          Mustard::Data::Field<float, "Ek0", float, "Primary kinetic energy">,
          Mustard::Data::Field<muc::array3f, "p0", muc::array3f, "Primary momentum">,
          Mustard::Data::Field<float, "Weight", float, "Track weight">> {};

} // namespace MusAirS::Data
