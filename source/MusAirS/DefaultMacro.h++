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

namespace MusAirS {

constexpr auto defaultMacro = {
    "#############################################################################",
    "# Initialization settings",
    "#############################################################################",
    "",
    "/control/verbose 2",
    "#/control/saveHistory",
    "/run/verbose 2",
    "",
    "#/Mustard/Detector/Description/Ixport MusAirS.yaml",
    "",
    "/MusAirS/Numeric/MinDriverStep    1 cm",
    "/MusAirS/Numeric/DeltaChord       1 m",
    "",
    "/run/initialize",
    "",
    "/MusAirS/Action/SwitchToGPSX",
    "/gps/particle             proton",
    "/gps/direction            0 0 -1",
    "/gps/position             0 0 100 km",
    "/gps/energy               100 GeV",
    "",
    "/run/setCut 10 m",
    "/run/setCutForAGivenParticle proton 3 m",
    "/MusAirS/SD/EarthSD/DetectNeutrino yes",
    "",
    "/Mustard/Analysis/FilePath MusAirS_vis",
    "/Mustard/Analysis/FileMode RECREATE",
    "",

#if MUSTARD_USE_G4VIS

    "#############################################################################",
    "# Visualization settings",
    "#############################################################################",
    "",
    "/vis/open OGL",
    "",
    "/vis/viewer/set/autoRefresh no",
    "/vis/verbose errors",
    "",
    "/vis/drawVolume",
    "",
    "/vis/viewer/set/upVector 0 0 1",
    "/vis/viewer/set/viewpointVector 0 -1 0.01",
    "#/vis/viewer/set/rotationStyle freeRotation",
    "/vis/viewer/set/projection p",
    "/vis/viewer/set/style s",
    "#/vis/viewer/set/style w",
    "#/vis/viewer/set/hiddenEdge yes",
    "/vis/viewer/set/auxiliaryEdge no",
    "#/vis/viewer/set/lineSegmentsPerCircle 100",
    "/vis/viewer/zoom 50",
    "",
    "# Colors",
    "/vis/geometry/set/colour Atmosphere          0     1     1     1     0.005",
    "/vis/geometry/set/colour Earth               0     0     0     1     1",
    "/vis/geometry/set/colour World               0     1     1     1     0.001",
    "",
    "/vis/scene/add/date",
    "/vis/scene/add/axes 0 0 0 10 km",
    "/vis/scene/add/trajectories smooth #rich",
    "/vis/scene/add/hits",
    "",
    "/vis/filtering/trajectories/create/particleFilter",
    "/vis/filtering/trajectories/particleFilter-0/add nu_e",
    "/vis/filtering/trajectories/particleFilter-0/add nu_mu",
    "/vis/filtering/trajectories/particleFilter-0/add nu_tau",
    "/vis/filtering/trajectories/particleFilter-0/add anti_nu_e",
    "/vis/filtering/trajectories/particleFilter-0/add anti_nu_mu",
    "/vis/filtering/trajectories/particleFilter-0/add anti_nu_tau",
    "/vis/filtering/trajectories/particleFilter-0/invert yes",
    "",
    "/vis/modeling/trajectories/create/drawByParticleID",
    "/vis/modeling/trajectories/drawByParticleID-0/default/setDrawAuxPts no",
    "/vis/modeling/trajectories/drawByParticleID-0/default/setAuxPtsSize 1",
    "/vis/modeling/trajectories/drawByParticleID-0/default/setAuxPtsSizeType screen",
    "/vis/modeling/trajectories/drawByParticleID-0/default/setAuxPtsType dots",
    "/vis/modeling/trajectories/drawByParticleID-0/default/setDrawLine yes",
    "/vis/modeling/trajectories/drawByParticleID-0/default/setLineWidth 1",
    "/vis/modeling/trajectories/drawByParticleID-0/default/setDrawStepPts no",
    "/vis/modeling/trajectories/drawByParticleID-0/default/setStepPtsSize 1",
    "/vis/modeling/trajectories/drawByParticleID-0/default/setStepPtsSizeType screen",
    "/vis/modeling/trajectories/drawByParticleID-0/default/setStepPtsType dots",
    "",
    "/vis/scene/endOfEventAction accumulate",
    "",
    "/vis/verbose warnings",
    "/vis/viewer/set/autoRefresh yes",

#endif

    ""};

} // namespace MusAirS
