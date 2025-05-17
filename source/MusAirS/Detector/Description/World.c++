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

#include "MusAirS/Detector/Description/World.h++"

#include "Mustard/Utility/LiteralUnit.h++"

namespace MusAirS::Detector::Description {

using namespace Mustard::LiteralUnit::Length;

World::World() :
    DescriptionBase{"World"},
    fWidth{2000_km},
    fMaxHeight{100_km} {}

void World::ImportAllValue(const YAML::Node& node) {
    ImportValue(node, fWidth, "Width");
    ImportValue(node, fMaxHeight, "MaxHeight");
}

void World::ExportAllValue(YAML::Node& node) const {
    ExportValue(node, fWidth, "Width");
    ExportValue(node, fMaxHeight, "MaxHeight");
}

} // namespace MusAirS::Detector::Description
