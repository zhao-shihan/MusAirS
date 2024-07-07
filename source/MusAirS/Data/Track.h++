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
    Mustard::Data::Value<float, "t", "Final time">,
    Mustard::Data::Value<muc::array3f, "x", "Final position">,
    Mustard::Data::Value<float, "Ek", "Final kinetic energy">,
    Mustard::Data::Value<muc::array3f, "p", "Final momentum">,
    Mustard::Data::Value<float, "Zenith", "Final 1 - cos(zenith angle)">,
    Mustard::Data::Value<float, "phi", "Final azimuth angle">,
    Mustard::Data::Value<float, "TrkLen", "Track total length">,
    Mustard::Data::Value<std::string, "KillProc", "Track killer process">>;

} // namespace MusAirS::Data
