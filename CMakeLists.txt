cmake_minimum_required(VERSION 3.21)

set(MusAirS_VERSION 0.0.1)

project(MusAirS VERSION ${MusAirS_VERSION}
                  LANGUAGES C CXX)

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
