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

# This file is included after "find_package"s and "LookFor"s.

# =============================================================================
# MusAirS at C++20
# =============================================================================

# Mustard 0.26.1121 requires C++20 without compiler extensions. The requirement
# is not exported through the Mustard::Mustard interface, so MusAirS must set it
# itself (an in-tree Mustard sets the same values before its own targets).
set(CMAKE_CXX_STANDARD 20 CACHE STRING "C++ standard.")
if(CMAKE_CXX_STANDARD LESS 20)
    message(FATAL_ERROR "MusAirS should be built, at least, with C++20")
endif()
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
message(STATUS "MusAirS will be compiled with C++${CMAKE_CXX_STANDARD}")

# =============================================================================
# LTO/IPO support check
# =============================================================================

set(MUSAIRS_ENABLE_IPO_SUPPORTED OFF)
if(MUSAIRS_ENABLE_IPO)
    include(CheckIPOSupported)
    # Enable C by a subproject (e.g. mimalloc) also enables C for the whole
    # configure, but MusAirS is C++ only, so test IPO for CXX only.
    check_ipo_supported(RESULT MUSAIRS_ENABLE_IPO_SUPPORTED
        OUTPUT MUSAIRS_IPO_SUPPORTED_ERROR
        LANGUAGES CXX)
    if(MUSAIRS_ENABLE_IPO_SUPPORTED)
        message(STATUS "LTO/IPO enabled for MusAirS (Release)")
    else()
        message(NOTICE "***Notice: LTO/IPO not supported. Turning off interprocedural optimization for MusAirS: ${MUSAIRS_IPO_SUPPORTED_ERROR}")
    endif()
endif()

# =============================================================================
# Compile options and definitions for MusAirS
# =============================================================================

set(MUSAIRS_PRIVATE_COMPILE_OPTIONS "")
set(MUSAIRS_PRIVATE_COMPILE_DEFINITIONS "")

# =============================================================================
# Compile warnings for MusAirS
# =============================================================================

# Mustard 0.26.1121 moved its warning flags from the public to the private
# interface, so a consumer no longer inherits them: MusAirS must set its own.
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    list(APPEND MUSAIRS_PRIVATE_COMPILE_OPTIONS -Wall -Wextra -Wduplicated-cond -Wnon-virtual-dtor -pedantic -Wundef -Wunused-macros)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "^(Clang|IntelLLVM)$")
    list(APPEND MUSAIRS_PRIVATE_COMPILE_OPTIONS -WCL4 -Wmove -Wnon-virtual-dtor -pedantic -Wundef -Wunused-macros)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    list(APPEND MUSAIRS_PRIVATE_COMPILE_OPTIONS /W4)
endif()

if(MUSAIRS_SHOW_EVEN_MORE_COMPILER_WARNINGS)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        list(APPEND MUSAIRS_PRIVATE_COMPILE_OPTIONS -Weffc++)
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "^(Clang|IntelLLVM)$")
        list(APPEND MUSAIRS_PRIVATE_COMPILE_OPTIONS -Weverything)
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        list(APPEND MUSAIRS_PRIVATE_COMPILE_OPTIONS /Wall)
    endif()
endif()

if(MUSAIRS_WERROR)
    if(CMAKE_CXX_COMPILER_ID MATCHES "^(GNU|Clang|IntelLLVM)$")
        list(APPEND MUSAIRS_PRIVATE_COMPILE_OPTIONS -Werror)
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        list(APPEND MUSAIRS_PRIVATE_COMPILE_OPTIONS /WX)
    endif()
endif()

# =============================================================================
# Definitions propagated by Mustard::Mustard
# =============================================================================

# Nothing must be duplicated here. Mustard 0.26.1121 exports its public compile
# definitions (OMPI_SKIP_MPICXX, MPICH_SKIP_MPICXX, EIGEN_DONT_PARALLELIZE,
# EIGEN_FFTW_DEFAULT when MUSTARD_USE_FFTW is on, MUSTARD_USE_G4VIS, ...) and
# its public warning suppressions for Eigen/GTL through the imported target.
