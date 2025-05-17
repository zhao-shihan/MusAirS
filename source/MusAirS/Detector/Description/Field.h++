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

#include "Mustard/Detector/Description/DescriptionBase.h++"

#include "muc/array"

namespace MusAirS::Detector::Description {

class Field final : public Mustard::Detector::Description::DescriptionBase<Field> {
    friend Mustard::Env::Memory::SingletonInstantiator;

private:
    Field();
    ~Field() = default;

public:
    auto MagneticField() const -> auto { return fMagneticField; }

    auto MagneticField(muc::array3d val) -> void { fMagneticField = val; }

private:
    auto ImportAllValue(const YAML::Node& node) -> void override;
    auto ExportAllValue(YAML::Node& node) const -> void override;

private:
    muc::array3d fMagneticField;
};

} // namespace MusAirS::Detector::Description
