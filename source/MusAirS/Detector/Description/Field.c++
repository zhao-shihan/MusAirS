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

#include "MusAirS/Detector/Description/Field.h++"

#include "Mustard/Utility/LiteralUnit.h++"

namespace MusAirS::Detector::Description {

using namespace Mustard::LiteralUnit::MagneticFluxDensity;

Field::Field() :
    DescriptionBase{"Field"},
    // 肇庆地磁场。参见：罗玉芬, 陆镜辉, 陈政雷. 华南地区地磁场长期变化特征[J]. 地震地磁观测与研究, 2021, 42(3): 121-128. DOI: 10.3969/j.issn.1003-3246.2021.03.014
    fMagneticField{-1762.0_nT, 37638.2_nT, -26120.2_nT} {}

void Field::ImportAllValue(const YAML::Node& node) {
    ImportValue(node, fMagneticField, "MagneticField");
}

void Field::ExportAllValue(YAML::Node& node) const {
    ExportValue(node, fMagneticField, "MagneticField");
}

} // namespace MusAirS::Detector::Description
