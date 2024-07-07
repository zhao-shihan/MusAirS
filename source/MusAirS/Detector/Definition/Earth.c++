#include "MusAirS/Detector/Definition/Earth.h++"
#include "MusAirS/Detector/Description/Earth.h++"
#include "MusAirS/Detector/Description/World.h++"

#include "G4Box.hh"
#include "G4GeometryTolerance.hh"
#include "G4LogicalVolume.hh"
#include "G4NistManager.hh"
#include "G4PVPlacement.hh"
#include "G4Transform3D.hh"

namespace MusAirS::Detector::Definition {

auto Earth::Construct(bool checkOverlaps) -> void {
    const auto& earth{Description::Earth::Instance()};
    const auto& world{Description::World::Instance()};

    const auto halfThickness{1e3 * G4GeometryTolerance::GetInstance()->GetSurfaceTolerance()};
    const auto solid{Make<G4Box>(
        earth.Name(),
        world.Width() / 2,
        world.Width() / 2,
        halfThickness)};
    const auto logic{Make<G4LogicalVolume>(
        solid,
        G4NistManager::Instance()->FindOrBuildMaterial("G4_WATER"),
        earth.Name())};
    Make<G4PVPlacement>(
        G4TranslateZ3D{-halfThickness + earth.GroundAltitude()},
        logic,
        earth.Name(),
        Mother().LogicalVolume(),
        false,
        0,
        checkOverlaps);
}

} // namespace MusAirS::Detector::Definition
