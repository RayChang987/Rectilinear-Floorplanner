cmake_minimum_required(VERSION 3.18)
project(iccad2023PD LANGUAGES C CXX)

# Global settings must be defined before add_subdirectory() to take effect.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Optimised build by default; configure with -DCMAKE_BUILD_TYPE=Debug to debug.
if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif ()

# ---------------------------------------------------------------------------
# GLPK: the static libraries are vendored in glpk-5.0/lib. The header is
# searched in glpk-5.0/include, Homebrew and the system include paths; it can
# also be given explicitly with -DGLPK_INCLUDE_DIR=<dir containing glpk.h>.
# ---------------------------------------------------------------------------
set(GLPK_ROOT ${CMAKE_SOURCE_DIR}/glpk-5.0)
if (APPLE)
    set(GLPK_LIBRARY ${GLPK_ROOT}/lib/libglpk_mac.a CACHE FILEPATH "GLPK library")
else ()
    set(GLPK_LIBRARY ${GLPK_ROOT}/lib/libglpk.a CACHE FILEPATH "GLPK library")
endif ()
find_path(GLPK_INCLUDE_DIR glpk.h
        HINTS ${GLPK_ROOT}/include /opt/homebrew/Cellar/glpk/5.0/include
              /opt/homebrew/include /usr/local/include)
if (NOT GLPK_INCLUDE_DIR)
    message(FATAL_ERROR "glpk.h not found. Install GLPK 5.0 (e.g. "
            "`brew install glpk` / `apt install libglpk-dev`) or pass "
            "-DGLPK_INCLUDE_DIR=<directory containing glpk.h>.")
endif ()
add_library(glpk::glpk STATIC IMPORTED GLOBAL)
set_target_properties(glpk::glpk PROPERTIES
        IMPORTED_LOCATION ${GLPK_LIBRARY}
        INTERFACE_INCLUDE_DIRECTORIES ${GLPK_INCLUDE_DIR})

find_package(Threads REQUIRED)

add_subdirectory(src)
