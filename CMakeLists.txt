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

cmake_minimum_required(VERSION 3.25)

# Enable MSVC_RUNTIME_LIBRARY target property
# See https://cmake.org/cmake/help/latest/policy/CMP0091.html
cmake_policy(SET CMP0091 NEW)

set(MUSAIRS_VERSION_MAJOR 0)
set(MUSAIRS_VERSION_MINOR 26)
set(MUSAIRS_VERSION_PATCH 1120)
set(MUSAIRS_VERSION ${MUSAIRS_VERSION_MAJOR}.${MUSAIRS_VERSION_MINOR}.${MUSAIRS_VERSION_PATCH})

project(MusAirS
    VERSION ${MUSAIRS_VERSION}
    LANGUAGES CXX)

# =============================================================================
# Dirs
# =============================================================================

set(MUSAIRS_CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
list(APPEND CMAKE_MODULE_PATH ${MUSAIRS_CMAKE_MODULE_PATH})

# =============================================================================
# Options
# =============================================================================

include(CMakeDependentOption)

# Options (ON/OFF)
option(MUSAIRS_ENABLE_IPO "Build MusAirS with interprocedural optimization (LTO in Release)." ON)
option(MUSAIRS_FULL_UNITY_BUILD "Build MusAirS with unity build technique COMPLETELY." OFF)
option(MUSAIRS_SHOW_EVEN_MORE_COMPILER_WARNINGS "Add compiler flag to show even more compiler warnings (GCC: -Weffc++, Clang: -Weverything, MSVC: /Wall)." OFF)
option(MUSAIRS_USE_SHARED_MSVC_RT "Select the MSVC runtime library for use (MSVC only). ON for shared (.dll), OFF for static (.lib)." ON)
option(MUSAIRS_WERROR "Treat MusAirS compilation warnings as errors." OFF)
cmake_dependent_option(MUSAIRS_UNITY_BUILD "Build MusAirS with unity build technique." OFF "NOT MUSAIRS_FULL_UNITY_BUILD" ON)

# =============================================================================
# Check compiler
# =============================================================================

# Keep in sync with Mustard 0.26.1127 (MUSTARD_MINIMUM_*_VERSION in Mustard's
# CMakeLists.txt). MusAirS must not be built with a compiler Mustard rejects.
set(MUSAIRS_MINIMUM_GCC_VERSION 13)
set(MUSAIRS_MINIMUM_CLANG_VERSION 19)
set(MUSAIRS_MINIMUM_INTELLLVM_VERSION 2025)
set(MUSAIRS_MINIMUM_MSVC_VERSION 19.50)

set(MUSAIRS_COMPILER_OKAY TRUE)
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS ${MUSAIRS_MINIMUM_GCC_VERSION})
        set(MUSAIRS_COMPILER_OKAY FALSE)
    endif()
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS ${MUSAIRS_MINIMUM_CLANG_VERSION})
        set(MUSAIRS_COMPILER_OKAY FALSE)
    endif()
elseif(CMAKE_CXX_COMPILER_ID MATCHES "IntelLLVM")
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS ${MUSAIRS_MINIMUM_INTELLLVM_VERSION})
        set(MUSAIRS_COMPILER_OKAY FALSE)
    endif()
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS ${MUSAIRS_MINIMUM_MSVC_VERSION})
        set(MUSAIRS_COMPILER_OKAY FALSE)
    endif()
endif()

if(NOT MUSAIRS_COMPILER_OKAY)
    message(FATAL_ERROR
        "MusAirS cannot be built with ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}.\n"
        "  Recommended compilers:\n"
        "    GNU Compiler (at least GCC ${MUSAIRS_MINIMUM_GCC_VERSION})\n"
        "    LLVM Clang (at least Clang ${MUSAIRS_MINIMUM_CLANG_VERSION})\n"
        "    Intel oneAPI DPC++/C++ Compiler (at least IntelLLVM ${MUSAIRS_MINIMUM_INTELLLVM_VERSION})\n"
        "    Microsoft Visual C++ (at least MSVC ${MUSAIRS_MINIMUM_MSVC_VERSION})")
endif()

# =============================================================================
# Installation path
# =============================================================================

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set_property(CACHE CMAKE_INSTALL_PREFIX PROPERTY VALUE ${PROJECT_SOURCE_DIR}/install)
    # An in-tree Mustard (and some of its dependencies) also redirects the
    # default install prefix to its own source tree. Clear this variable so that
    # only MusAirS chooses the default prefix.
    set(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT FALSE)
endif()
message(STATUS "MusAirS install path: ${CMAKE_INSTALL_PREFIX} (add -DCMAKE_INSTALL_PREFIX=<install_path> to the CMake command if you wish to change it)")

# =============================================================================
# Compile configs and dependencies
# =============================================================================

# Global compile configs, before finding dependencies.
include(MusAirSGlobalCompileConfig)
# Dependencies are built in-tree but never installed, so they must be static.
set(BUILD_SHARED_LIBS OFF)
# Find Mustard with CPM (an installed Mustard is preferred). ROOT, Geant4, MPI,
# Eigen, fmt, ... are all found by Mustard and propagated through the
# Mustard::Mustard usage requirements.
include(MusAirSLookForPackage)
# Compile configs for MusAirS itself, after dependencies are available.
include(MusAirSCompileConfig)

# =============================================================================
# Let's dive in...
# =============================================================================

# Generate the version header (same convention as Mustard's Version.inl).
configure_file(
    ${PROJECT_SOURCE_DIR}/source/MusAirS/Version.h++.in
    ${PROJECT_SOURCE_DIR}/source/MusAirS/Version.h++
    @ONLY)

# Add sources and declare the target executable.
file(GLOB_RECURSE MUSAIRS_SRC CONFIGURE_DEPENDS ${PROJECT_SOURCE_DIR}/source/MusAirS/*.c++)
add_executable(MusAirS ${MUSAIRS_SRC})
add_executable(MusAirS::MusAirS ALIAS MusAirS)
target_include_directories(MusAirS PRIVATE ${PROJECT_SOURCE_DIR}/source)
target_link_libraries(MusAirS PRIVATE Mustard::Mustard)
target_compile_options(MusAirS PRIVATE ${MUSAIRS_PRIVATE_COMPILE_OPTIONS})
target_compile_definitions(MusAirS PRIVATE ${MUSAIRS_PRIVATE_COMPILE_DEFINITIONS})
set_target_properties(MusAirS PROPERTIES
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF)

# Unity build: apply it to the MusAirS target only. An in-tree Mustard gets the
# same setting through MUSTARD_UNITY_BUILD / MUSTARD_FULL_UNITY_BUILD (forwarded
# in MusAirSLookForPackage). Setting CMAKE_UNITY_BUILD globally here would also
# enable unity build for the Mustard targets.
set_target_properties(MusAirS PROPERTIES UNITY_BUILD ${MUSAIRS_UNITY_BUILD})
if(MUSAIRS_UNITY_BUILD)
    if(MUSAIRS_FULL_UNITY_BUILD)
        set_target_properties(MusAirS PROPERTIES UNITY_BUILD_BATCH_SIZE 0)
        message(STATUS "Unity build enabled for MusAirS (batch size: unlimited)")
    else()
        set_target_properties(MusAirS PROPERTIES UNITY_BUILD_BATCH_SIZE 8)
        message(STATUS "Unity build enabled for MusAirS (batch size: 8)")
    endif()
endif()

# IPO/LTO: same reasoning as for unity build, scope it to the MusAirS target so
# that an in-tree Mustard keeps its own MUSTARD_ENABLE_IPO policy.
if(MUSAIRS_ENABLE_IPO AND MUSAIRS_ENABLE_IPO_SUPPORTED)
    set_property(TARGET MusAirS PROPERTY INTERPROCEDURAL_OPTIMIZATION_RELEASE ON)
endif()

# Copy scripts to the build directory (build-time copy, so a newly added script
# is picked up without re-configuring).
file(GLOB MUSAIRS_SCRIPTS CONFIGURE_DEPENDS ${PROJECT_SOURCE_DIR}/scripts/*)
if(MUSAIRS_SCRIPTS)
    add_custom_target(MusAirSCopyScripts ALL
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${MUSAIRS_SCRIPTS} ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Copying MusAirS scripts to the build directory")
endif()

# =============================================================================
# Install
# =============================================================================

include(GNUInstallDirs)
install(TARGETS MusAirS RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(DIRECTORY ${PROJECT_SOURCE_DIR}/scripts/
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/MusAirS)
install(FILES ${PROJECT_SOURCE_DIR}/COPYING DESTINATION ${CMAKE_INSTALL_DATADIR}/licenses/MusAirS)
install(FILES ${PROJECT_SOURCE_DIR}/README.md DESTINATION ${CMAKE_INSTALL_DATADIR}/doc/MusAirS)
