#pragma once

#include "MusAirS/Data/EarthHit.h++"

#include "Mustard/Extension/Geant4X/Memory/UseG4Allocator.h++"

#include "G4THitsCollection.hh"
#include "G4VHit.hh"

namespace MusAirS::inline Hit {

class EarthHit final : public Mustard::Geant4X::UseG4Allocator<EarthHit>,
                         public G4VHit,
                         public Mustard::Data::Tuple<Data::EarthHit> {};

using EarthHitCollection = G4THitsCollection<EarthHit>;

} // namespace MusAirS::inline Hit
