cmake_minimum_required(VERSION 3.21)

set(MusAirS_VERSION_MAJOR 0)
set(MusAirS_VERSION_MINOR 25)
set(MusAirS_VERSION_PATCH 517)
set(MusAirS_VERSION ${MusAirS_VERSION_MAJOR}.${MusAirS_VERSION_MINOR}.${MusAirS_VERSION_PATCH})

project(MusAirS VERSION ${MusAirS_VERSION}
                LANGUAGES C CXX)

# Set default build type
if(DEFINED CMAKE_CONFIGURATION_TYPES)
    # multi-config generator
    message(STATUS "Using multi-config generator \"${CMAKE_GENERATOR}\"")
    message(STATUS "It provides the following build types: ${CMAKE_CONFIGURATION_TYPES}")
    if(DEFINED CMAKE_BUILD_TYPE)
        message(NOTICE "***Notice: CMAKE_BUILD_TYPE is defined while using a multi-config generator \"${CMAKE_GENERATOR}\".")
        message(NOTICE "           CMAKE_BUILD_TYPE is ignored by the multi-config generator and the build type should be specified at build time")
    endif()
else()
    # single-config generator
    message(STATUS "Using single-config generator \"${CMAKE_GENERATOR}\"")
    if(CMAKE_BUILD_TYPE STREQUAL "")
        # Default to "Release"
        set(CMAKE_BUILD_TYPE "Release")
    endif()
    string(TOLOWER ${CMAKE_BUILD_TYPE} LOWERCASED_CMAKE_BUILD_TYPE)
    message(STATUS "MusAirS build type: ${CMAKE_BUILD_TYPE}")
endif()

# Turn on unity build
set(CMAKE_UNITY_BUILD ON)
set(CMAKE_UNITY_BUILD_BATCH_SIZE 0)
message(STATUS "Unity build enabled (batch size: unlimited)")

# Turn on LTO/IPO
include(CheckIPOSupported)
check_ipo_supported(RESULT MusAirS_ENABLE_IPO_SUPPORTED
                    OUTPUT MusAirS_IPO_SUPPORTED_ERROR)
if(MusAirS_ENABLE_IPO_SUPPORTED)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE ON)
    message(STATUS "LTO/IPO enabled")
else()
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE OFF)
    message(NOTICE "***Notice: LTO/IPO not supported")
endif()

# Always static
set(BUILD_SHARED_LIBS OFF)

# Find or build-in Mustard.
# ROOT, Geant4, and MPI, etc. are also found here.
include(cmake/LookForMustard.cmake)

# Generate version header
configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/source/MusAirS/Version.h++.in
                              ${CMAKE_CURRENT_SOURCE_DIR}/source/MusAirS/Version.h++
                              INSTALL_DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/source/MusAirS)

# Add sources and declare the target executable.
file(GLOB_RECURSE MusAirS_srcs ${CMAKE_CURRENT_SOURCE_DIR}/source/*.c++)
add_executable(MusAirS ${MusAirS_srcs})
target_include_directories(MusAirS PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/source)
target_link_libraries(MusAirS PRIVATE Mustard::Mustard)

# Copy scripts to build dir.
file(GLOB MusAirS_SCRIPTS ${CMAKE_CURRENT_SOURCE_DIR}/scripts/*)
foreach(_scripts ${MusAirS_SCRIPTS})
    configure_file(${_scripts} ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)
endforeach()

include(GNUInstallDirs)

# Install targets and scripts.
install(TARGETS MusAirS DESTINATION ${CMAKE_INSTALL_BINDIR})
foreach(_scripts ${MusAirS_SCRIPTS})
    install(FILES ${_scripts} DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/MusAirS)
endforeach()
