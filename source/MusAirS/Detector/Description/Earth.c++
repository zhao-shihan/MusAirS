#include "MusAirS/Detector/Description/Earth.h++"

#include "Mustard/Utility/LiteralUnit.h++"

namespace MusAirS::Detector::Description {

using namespace Mustard::LiteralUnit::Length;

Earth::Earth() :
    DescriptionBase{"Earth"},
    fDepth{1_km} {}

void Earth::ImportAllValue(const YAML::Node& node) {
    ImportValue(node, fDepth, "Depth");
}

void Earth::ExportAllValue(YAML::Node& node) const {
    ExportValue(node, fDepth, "Depth");
}

} // namespace MusAirS::Detector::Description
