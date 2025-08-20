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

#include "MusAirS/Messenger/SDMessenger.h++"

#include "Mustard/Utility/NonCopyableBase.h++"

#include "G4VSensitiveDetector.hh"

#include <vector>

namespace MusAirS::inline SD {

class EarthSD : public Mustard::NonCopyableBase,
                public G4VSensitiveDetector {
public:
    using HitTrackIDDataType = std::vector<int>;

public:
    EarthSD(const G4String& sdName);

    auto DetectNeutrino() const -> auto { return fDetectNeutrino; }
    auto DetectNeutrino(bool val) -> void { fDetectNeutrino = val; }

    virtual auto Initialize(G4HCofThisEvent* hitsCollection) -> void override;
    virtual auto ProcessHits(G4Step* theStep, G4TouchableHistory*) -> G4bool override;
    virtual auto EndOfEvent(G4HCofThisEvent*) -> void override;

protected:
    bool fDetectNeutrino;

    HitTrackIDDataType fHitTrackIDData;

    SDMessenger::Register<EarthSD> fSDMessengerRegister;
};

} // namespace MusAirS::inline SD
