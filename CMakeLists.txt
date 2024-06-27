cmake_minimum_required(VERSION 3.21)

set(MusAirS_VERSION 0.0.1)

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

# Find or build-in Mustard.
# ROOT, Geant4, and MPI, etc. are also found here.
include(cmake/LookForMustard.cmake)

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
