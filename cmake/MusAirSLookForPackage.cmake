# Copyright 2024-2026  Shihan Zhao
#
# This file is part of MusAirS, an air shower simulation tool.
#
# MusAirS is free software: you can redistribute it and/or modify it under the
# terms of the GNU General Public License as published by the Free Software
# Foundation, either version 3 of the License, or (at your option) any later
# version.
#
# MusAirS is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
# A PARTICULAR PURPOSE. See the GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along with
# MusAirS. If not, see <https://www.gnu.org/licenses/>.

# This module finds Mustard with CPM.cmake. CPMFindPackage first tries an
# installed Mustard (find_package), and otherwise downloads the pinned git tag
# into the CPM source cache (.cache/cpm by default), so later configures do not
# need the network. ROOT, Geant4, MPI, Eigen, fmt, ... are found by Mustard and
# propagated through the Mustard::Mustard usage requirements.

# Avoid reusing packages registered in the CMake user package registry (e.g. a
# build-tree Mustard left behind by a previous configure of some other project).
set(CMAKE_FIND_PACKAGE_NO_PACKAGE_REGISTRY ON)

# CPM.cmake, for package management (same version as Mustard).
file(DOWNLOAD
    https://github.com/cpm-cmake/CPM.cmake/releases/download/v0.43.2/CPM.cmake
    ${MUSAIRS_CMAKE_MODULE_PATH}/CPM.cmake
    EXPECTED_HASH SHA256=49a3bef91ceb65bb66d57255e12d1ffd22abc2f6408fa9fe4534c544a2f232aa)
set(CPM_SOURCE_CACHE "${PROJECT_SOURCE_DIR}/.cache/cpm" CACHE PATH "CPM source cache directory")
include(CPM)

# Mustard. OPTIONS only configure an in-tree build of Mustard; they are ignored
# when an installed Mustard is found, whose configuration is already fixed.
set(MUSAIRS_MUSTARD_MINIMUM_REQUIRED 0.26.1123)
CPMFindPackage(
    NAME Mustard
    GITHUB_REPOSITORY zhao-shihan/Mustard
    VERSION ${MUSAIRS_MUSTARD_MINIMUM_REQUIRED}
    EXCLUDE_FROM_ALL ON
    OPTIONS "MUSTARD_UNITY_BUILD ${MUSAIRS_UNITY_BUILD}"
            "MUSTARD_FULL_UNITY_BUILD ${MUSAIRS_FULL_UNITY_BUILD}"
            "MUSTARD_USE_FFTW OFF"
            "MUSTARD_USE_SHARED_MSVC_RT ${MUSAIRS_USE_SHARED_MSVC_RT}")
