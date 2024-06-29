#pragma once

#include "Mustard/Detector/Description/DescriptionBase.h++"

namespace MusAirS::Detector::Description {

class Earth final : public Mustard::Detector::Description::DescriptionBase<Earth> {
    friend Mustard::Env::Memory::SingletonInstantiator;

private:
    Earth();
    ~Earth() = default;

public:
    auto GroundAltitude() const -> auto { return fGroundAltitude; }
    auto Depth() const -> auto { return fDepth; }

    auto GroundAltitude(double val) -> auto { fGroundAltitude = val; }
    auto Depth(double val) -> void { fDepth = val; }

private:
    auto ImportAllValue(const YAML::Node& node) -> void override;
    auto ExportAllValue(YAML::Node& node) const -> void override;

private:
    double fGroundAltitude;
    double fDepth;
};

} // namespace MusAirS::Detector::Description
