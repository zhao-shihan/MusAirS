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
#include <string>

namespace MusAirS::Data {

struct Track final
    : Mustard::Data::Model<
          Mustard::Data::Field<int, "EvtID", std::int32_t, "Event ID">,
          Mustard::Data::Field<int, "ParTrkID", std::int32_t, "Parent track ID">,
          Mustard::Data::Field<int, "ParPDGID", std::int32_t, "Parent PDG ID">,
          Mustard::Data::Field<std::string, "CreatProc", std::string, "Track creator process">,
          Mustard::Data::Field<int, "TrkID", std::int32_t, "Track ID">,
          Mustard::Data::Field<int, "PDGID", std::int32_t, "Particle PDG ID">,
          Mustard::Data::Field<float, "t0", float, "Initial time">,
          Mustard::Data::Field<muc::array3f, "x0", muc::array3f, "Initial position">,
          Mustard::Data::Field<float, "Ek0", float, "Initial kinetic energy">,
          Mustard::Data::Field<muc::array3f, "p0", muc::array3f, "Initial momentum">,
          Mustard::Data::Field<muc::array3f, "Sigma0", muc::array3f, "Initial spin direction">,
          Mustard::Data::Field<float, "Helicity0", float, "Initial helicity">,
          Mustard::Data::Field<float, "t", float, "Final time">,
          Mustard::Data::Field<muc::array3f, "x", muc::array3f, "Final position">,
          Mustard::Data::Field<float, "Ek", float, "Final kinetic energy">,
          Mustard::Data::Field<muc::array3f, "p", muc::array3f, "Final momentum">,
          Mustard::Data::Field<muc::array3f, "Sigma", muc::array3f, "Final spin direction">,
          Mustard::Data::Field<float, "Helicity", float, "Final helicity">,
          Mustard::Data::Field<float, "Zenith", float, "Final 1 - cos(zenith angle)">,
          Mustard::Data::Field<float, "phi", float, "Final azimuth angle">,
          Mustard::Data::Field<float, "TrkLen", float, "Track total length">,
          Mustard::Data::Field<std::string, "KillProc", std::string, "Track killer process">,
          Mustard::Data::Field<float, "Weight", float, "Track weight">> {};

} // namespace MusAirS::Data
