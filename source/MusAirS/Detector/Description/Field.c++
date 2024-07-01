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
