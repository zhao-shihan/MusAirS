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

#include "Mustard/Detector/Description/DescriptionWithCacheBase.h++"

#include <vector>

namespace MusAirS::Detector::Description {

class Atmosphere final : public Mustard::Detector::Description::DescriptionWithCacheBase<Atmosphere> {
    friend Mustard::Env::Memory::SingletonInstantiator;

private:
    Atmosphere();
    ~Atmosphere() = default;

public:
    enum struct SliceMode {
        Altitude,
        Pressure
    };

public:
    auto MaxAltitude() const -> auto { return *fMaxAltitude; }
    auto SliceMode() const -> auto { return *fSliceMode; }
    auto NSlice() const -> auto { return *fNSlice; }
    auto AltitudeSlice() const -> const auto& { return *fAltitudeSlice; }
    auto StateSlice() const -> const auto& { return *fStateSlice; }

    auto MaxAltitude(double val) -> void { fMaxAltitude = val; }
    auto SliceMode(enum SliceMode val) -> void { fSliceMode = val; }
    auto NSlice(int val) -> void { fNSlice = val; }

private:
    struct AtmoState {
        double pressure;
        double density;
        double temperature;
    };

private:
    auto CalculateAltitudeSlice() const -> std::vector<double>;
    auto CalculateStateSlice() const -> std::vector<AtmoState>;

    auto ImportAllValue(const YAML::Node& node) -> void override;
    auto ExportAllValue(YAML::Node& node) const -> void override;

private:
    Simple<double> fMaxAltitude;
    Simple<enum SliceMode> fSliceMode;
    Simple<int> fNSlice;
    Cached<std::vector<double>> fAltitudeSlice;
    Cached<std::vector<AtmoState>> fStateSlice;
};

} // namespace MusAirS::Detector::Description
