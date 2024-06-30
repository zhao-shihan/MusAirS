#include "MusAirS/Detector/Definition/Atmosphere.h++"
#include "MusAirS/Detector/Description/Atmosphere.h++"
#include "MusAirS/Detector/Description/Earth.h++"
#include "MusAirS/Detector/Description/World.h++"

#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4NistManager.hh"
#include "G4PVPlacement.hh"
#include "G4Transform3D.hh"

#include "gsl/gsl"

namespace MusAirS::Detector::Definition {

auto Atmosphere::Construct(bool checkOverlaps) -> void {
    const auto& atmosphere{Description::Atmosphere::Instance()};
    const auto& earth{Description::Earth::Instance()};
    const auto& world{Description::World::Instance()};

    switch (atmosphere.SliceMode()) {
    case Description::Atmosphere::SliceMode::Altitude: {
        const auto sliceThickness{(atmosphere.MaxAltitude() - earth.GroundAltitude()) / atmosphere.NSlice()};
        const auto solid{Make<G4Box>(
            atmosphere.Name(),
            world.Width() / 2,
            world.Width() / 2,
            sliceThickness / 2)};
        for (gsl::index i{}; i < atmosphere.NSlice(); ++i) {
            const auto logic{Make<G4LogicalVolume>(
                solid,
                G4NistManager::Instance()->BuildMaterialWithNewDensity(
                    fmt::format("AtmosphereSlice{}", i), "G4_AIR",
                    atmosphere.StateSlice()[i].density,
                    atmosphere.StateSlice()[i].temperature,
                    atmosphere.StateSlice()[i].pressure),
                atmosphere.Name())};
            Make<G4PVPlacement>(
                G4TranslateZ3D{-sliceThickness / 2 + atmosphere.AltitudeSlice()[i]},
                logic,
                atmosphere.Name(),
                Mother().LogicalVolume(),
                false,
                i,
                checkOverlaps);
        }
    } break;
    case Description::Atmosphere::SliceMode::Pressure: {
        for (gsl::index i{}; i < atmosphere.NSlice(); ++i) {
            const auto sliceThickness{i == 0 ? atmosphere.AltitudeSlice()[i] - earth.GroundAltitude() :
                                               atmosphere.AltitudeSlice()[i] - atmosphere.AltitudeSlice()[i - 1]};
            const auto solid{Make<G4Box>(
                atmosphere.Name(),
                world.Width() / 2,
                world.Width() / 2,
                sliceThickness / 2)};
            const auto logic{Make<G4LogicalVolume>(
                solid,
                G4NistManager::Instance()->BuildMaterialWithNewDensity(
                    fmt::format("AtmosphereSlice{}", i), "G4_AIR",
                    atmosphere.StateSlice()[i].density,
                    atmosphere.StateSlice()[i].temperature,
                    atmosphere.StateSlice()[i].pressure),
                atmosphere.Name())};
            Make<G4PVPlacement>(
                G4TranslateZ3D{-sliceThickness / 2 + atmosphere.AltitudeSlice()[i]},
                logic,
                atmosphere.Name(),
                Mother().LogicalVolume(),
                false,
                i,
                checkOverlaps);
        }
    } break;
    }
}

} // namespace MusAirS::Detector::Definition
